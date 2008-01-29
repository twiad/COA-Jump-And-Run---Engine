/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2007  Adrian Jäkel                       *
 *                     Copyright (C) 2007  Franz Köhler                       *
 *                     Copyright (C) 2007  Robert Timm                        *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/


#include "InputManager.h"

#include "MainApplication.h"
#include "BufferedInputHandlerComponent.h"
#include "UnbufferedInputHandlerComponent.h"
#include "UserThreadPool.h"

#ifdef __MAC__
#  include <Carbon/Carbon.h>
#endif

namespace CoAJnR
{

InputManager::InputManager()
{
    m_initialized = false;
    
    m_inputManager = 0;
    m_keyboard = 0;
    m_mouse = 0;
    m_joystick = 0;
}

InputManager::~InputManager()
{
}

void
InputManager::init(size_t p_windowHandle, uint p_width, uint p_height)
{
    std::ostringstream windowHndStr;
    windowHndStr << p_windowHandle;

    OIS::ParamList paramList;
    paramList.insert(std::make_pair(
            std::string("WINDOW"), 
            windowHndStr.str()));

#if defined OIS_WIN32_PLATFORM
    paramList.insert(std::make_pair(
            std::string("w32_mouse"), 
            std::string("DISCL_FOREGROUND" )));
    paramList.insert(std::make_pair(
            std::string("w32_mouse"), 
            std::string("DISCL_NONEXCLUSIVE")));
    paramList.insert(std::make_pair(
            std::string("w32_keyboard"), 
            std::string("DISCL_FOREGROUND")));
    paramList.insert(std::make_pair(
            std::string("w32_keyboard"), 
            std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
    paramList.insert(std::make_pair(
            std::string("x11_mouse_grab"), 
            std::string("false")));
    paramList.insert(std::make_pair(
            std::string("x11_mouse_hide"), 
            std::string("false")));
    paramList.insert(std::make_pair(
            std::string("x11_keyboard_grab"), 
            std::string("false")));
    paramList.insert(std::make_pair(
            std::string("XAutoRepeatOn"), 
            std::string("true")));
#elif defined OIS_APPLE_PLATFORM

#endif

    /// @todo TODO: disable key repeat on all platforms

    m_inputManager = OIS::InputManager::createInputSystem(paramList);

    m_keyboard = static_cast<OIS::Keyboard*>(
            m_inputManager->createInputObject(OIS::OISKeyboard, true));
            
    m_keyboard->setEventCallback(this);

    m_mouse = static_cast<OIS::Mouse*>(
        m_inputManager->createInputObject(OIS::OISMouse, true));

    m_mouse->setEventCallback(this);

    const OIS::MouseState& ms = m_mouse->getMouseState();
    ms.width = p_width;
    ms.height = p_height;

#ifdef __MAC__
    CGAssociateMouseAndMouseCursorPosition(TRUE);
    CGDisplayShowCursor(kCGDirectMainDisplay);
#endif

    try 
    {
        m_joystick = static_cast<OIS::JoyStick*>(
                m_inputManager->createInputObject(OIS::OISJoyStick, false));
    }
    catch(...) 
    {
        m_joystick = 0;
    }

    if(m_joystick)
        m_joystick->setEventCallback(this);

    m_initialized = true;
}

void
InputManager::update()
{
    if(!m_initialized)
        return;

    {
        /// @todo TODO: this lock is needed because xlib is not thread safe
        /// check if this is really needed on windows and osx
        /// the current approach locks the FreeRunner, which is somehow bad
        boost::mutex::scoped_lock scopedAutoLock(
                MainApplication::windowMessageQueueMutex());

        m_keyboard->capture();
        m_mouse->capture();
    
        if(m_joystick) 
            m_joystick->capture();
    }
    
    updateUnbufferedHandlers();
}

void
InputManager::updateUnbufferedHandlers()
{
    OIS::MouseState mouseState;
    OIS::JoyStickState joystickState;

    mouseState = m_mouse->getMouseState();
    
    if(m_joystick)
        joystickState = m_joystick->getJoyStickState();
        
    boost::mutex::scoped_lock scopedAutoLock(m_unbufferedHandlerVectorMutex);
        
    std::vector<UnbufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_unbufferedHandlers.begin(); 
            it != m_unbufferedHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&UnbufferedInputHandlerComponent::updateInternal,
                            (*it),
                            keyboard(),
                            mouseState,
                            joystickState));
}

void
InputManager::release()
{
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_unbufferedHandlerVectorMutex);
    
        while(m_unbufferedHandlers.size())
            m_unbufferedHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_keyPressHandlerVectorMutex);
    
        while(m_keyPressHandlers.size())
            m_keyPressHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_keyReleaseHandlerVectorMutex);
    
        while(m_keyReleaseHandlers.size())
            m_keyReleaseHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mouseMoveHandlerVectorMutex);
    
        while(m_mouseMoveHandlers.size())
            m_mouseMoveHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mousePressHandlerVectorMutex);
    
        while(m_mousePressHandlers.size())
            m_mousePressHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mouseReleaseHandlerVectorMutex);
    
        while(m_mouseReleaseHandlers.size())
            m_mouseReleaseHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickPressHandlerVectorMutex);
    
        while(m_joystickPressHandlers.size())
            m_joystickPressHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickReleaseHandlerVectorMutex);
    
        while(m_joystickReleaseHandlers.size())
            m_joystickReleaseHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickAxisMoveHandlerVectorMutex);
    
        while(m_joystickAxisMoveHandlers.size())
            m_joystickAxisMoveHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickSliderMoveHandlerVectorMutex);
    
        while(m_joystickSliderMoveHandlers.size())
            m_joystickSliderMoveHandlers.pop_back();    
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickPovMoveHandlerVectorMutex);
    
        while(m_joystickPovMoveHandlers.size())
            m_joystickPovMoveHandlers.pop_back();    
    }
    
    if(m_initialized && m_inputManager)
        OIS::InputManager::destroyInputSystem(m_inputManager);
}

void 
InputManager::addUnbufferedHandler(UnbufferedInputHandlerComponentPtr p_handler)
{
    assert(p_handler && "handler must not be 0");

    boost::mutex::scoped_lock scopedAutoLock(m_unbufferedHandlerVectorMutex);

    m_unbufferedHandlers.push_back(p_handler);
}

bool 
InputManager::removeUnbufferedHandler(UnbufferedInputHandlerComponentPtr p_handler)
{
    assert(p_handler && "handler must not be 0");

    boost::mutex::scoped_lock scopedAutoLock(m_unbufferedHandlerVectorMutex);

    std::vector<UnbufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_unbufferedHandlers.begin(); 
            it != m_unbufferedHandlers.end(); it++)
        if((*it) == p_handler)
        {
            m_unbufferedHandlers.erase(it);
            return true;
        }

    return false;
}

void
InputManager::addBufferedHandler(
        BufferedInputHandlerComponentPtr p_handler, InputEvent p_event)
{
    assert(p_handler && "handler must not be 0");

    if(p_event & KEY_PRESS)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_keyPressHandlerVectorMutex);
        m_keyPressHandlers.push_back(p_handler);
    }

    if(p_event & KEY_RELEASE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_keyReleaseHandlerVectorMutex);
        m_keyReleaseHandlers.push_back(p_handler);
    }

    if(p_event & MOUSE_MOVE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mouseMoveHandlerVectorMutex);
        m_mouseMoveHandlers.push_back(p_handler);
    }

    if(p_event & MOUSE_PRESS)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mousePressHandlerVectorMutex);
        m_mousePressHandlers.push_back(p_handler);
    }

    if(p_event & MOUSE_RELEASE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mouseReleaseHandlerVectorMutex);
        m_mouseReleaseHandlers.push_back(p_handler);
    }

    if(p_event & JOYSTICK_PRESS)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickPressHandlerVectorMutex);
        m_joystickPressHandlers.push_back(p_handler);
    }

    if(p_event & JOYSTICK_RELEASE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickReleaseHandlerVectorMutex);
        m_joystickReleaseHandlers.push_back(p_handler);
    }

    if(p_event & JOYSTICK_AXIS_MOVE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickAxisMoveHandlerVectorMutex);
        m_joystickAxisMoveHandlers.push_back(p_handler);
    }

    if(p_event & JOYSTICK_SLIDER_MOVE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickSliderMoveHandlerVectorMutex);
        m_joystickSliderMoveHandlers.push_back(p_handler);
    }

    if(p_event & JOYSTICK_POV_MOVE)
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickPovMoveHandlerVectorMutex);
        m_joystickPovMoveHandlers.push_back(p_handler);
    }
}

bool
InputManager::removeBufferedHandler(BufferedInputHandlerComponentPtr p_handler)
{
    assert(p_handler && "handler must not be 0");

    bool alreadyFound = false;
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_keyPressHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_keyPressHandlers.begin(); 
                it != m_keyPressHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_keyPressHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_keyReleaseHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_keyReleaseHandlers.begin(); 
                it != m_keyReleaseHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_keyReleaseHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }
    
    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mouseMoveHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_mouseMoveHandlers.begin(); 
                it != m_mouseMoveHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_mouseMoveHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mousePressHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_mousePressHandlers.begin(); 
                it != m_mousePressHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_mousePressHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_mouseReleaseHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_mouseReleaseHandlers.begin(); 
                it != m_mouseReleaseHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_mouseReleaseHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickPressHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_joystickPressHandlers.begin(); 
                it != m_joystickPressHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_joystickPressHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickReleaseHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_joystickReleaseHandlers.begin(); 
                it != m_joystickReleaseHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_joystickReleaseHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickAxisMoveHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_joystickAxisMoveHandlers.begin(); 
                it != m_joystickAxisMoveHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_joystickAxisMoveHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickSliderMoveHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_joystickSliderMoveHandlers.begin(); 
                it != m_joystickSliderMoveHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_joystickSliderMoveHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    {
        boost::mutex::scoped_lock scopedAutoLock(
                m_joystickPovMoveHandlerVectorMutex);

        std::vector<BufferedInputHandlerComponentPtr>::iterator it;
        for(it = m_joystickPovMoveHandlers.begin(); 
                it != m_joystickPovMoveHandlers.end(); it++)
            if((*it) == p_handler)
            {
                m_joystickPovMoveHandlers.erase(it);
                alreadyFound = true;
                break;
            }
    }

    return alreadyFound;   
}

bool 
InputManager::keyPressed(const OIS::KeyEvent& p_arg)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_keyPressHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_keyPressHandlers.begin(); 
            it != m_keyPressHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&BufferedInputHandlerComponent::keyPressedInternal,
                            *it,
                            p_arg));

    return true;
}

bool 
InputManager::keyReleased(const OIS::KeyEvent& p_arg)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_keyReleaseHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_keyReleaseHandlers.begin(); 
            it != m_keyReleaseHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&BufferedInputHandlerComponent::keyReleasedInternal,
                            *it, 
                            p_arg));

    return true;
}

bool 
InputManager::mouseMoved(const OIS::MouseEvent& p_arg)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_mouseMoveHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_mouseMoveHandlers.begin(); 
            it != m_mouseMoveHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&BufferedInputHandlerComponent::mouseMovedInternal,
                            *it, 
                            p_arg));

    return true;
}

bool 
InputManager::mousePressed(const OIS::MouseEvent& p_arg, 
                           OIS::MouseButtonID p_id)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_mousePressHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_mousePressHandlers.begin(); 
            it != m_mousePressHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(
                        &BufferedInputHandlerComponent::mousePressedInternal, 
                        *it, 
                        p_arg, 
                        p_id));

    return true;
}

bool 
InputManager::mouseReleased(const OIS::MouseEvent& p_arg, 
                            OIS::MouseButtonID p_id)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_mouseReleaseHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_mouseReleaseHandlers.begin(); 
            it != m_mouseReleaseHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(
                        &BufferedInputHandlerComponent::mouseReleasedInternal, 
                        *it, 
                        p_arg, 
                        p_id));

    return true;
}

bool 
InputManager::buttonPressed(const OIS::JoyStickEvent& p_arg, int p_button)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_joystickPressHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_joystickPressHandlers.begin(); 
            it != m_joystickPressHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(
                        &BufferedInputHandlerComponent::buttonPressedInternal, 
                        *it, 
                        p_arg, 
                        p_button));

    return true;
}

bool 
InputManager::buttonReleased(const OIS::JoyStickEvent& p_arg, int p_button)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_joystickReleaseHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_joystickReleaseHandlers.begin(); 
            it != m_joystickReleaseHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(
                        &BufferedInputHandlerComponent::buttonReleasedInternal, 
                        *it, 
                        p_arg, 
                        p_button));

    return true;
}

bool 
InputManager::axisMoved(const OIS::JoyStickEvent& p_arg, int p_axis)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_joystickAxisMoveHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_joystickAxisMoveHandlers.begin(); 
            it != m_joystickAxisMoveHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&BufferedInputHandlerComponent::axisMovedInternal,
                            *it, 
                            p_arg, 
                            p_axis));

    return true;
}

bool 
InputManager::sliderMoved(const OIS::JoyStickEvent& p_arg, int p_id)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_joystickSliderMoveHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_joystickSliderMoveHandlers.begin(); 
            it != m_joystickSliderMoveHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&BufferedInputHandlerComponent::sliderMovedInternal,
                            *it, 
                            p_arg, 
                            p_id));

    return true;
}

bool 
InputManager::povMoved(const OIS::JoyStickEvent& p_arg, int p_id)
{
    boost::mutex::scoped_lock scopedAutoLock(
            m_joystickPovMoveHandlerVectorMutex);
        
    std::vector<BufferedInputHandlerComponentPtr>::iterator it;
    for(it = m_joystickPovMoveHandlers.begin(); 
            it != m_joystickPovMoveHandlers.end(); it++)
        UserThreadPool::get().schedule(
                boost::bind(&BufferedInputHandlerComponent::povMovedInternal, 
                            *it, 
                            p_arg, 
                            p_id));

    return true;
}

}

