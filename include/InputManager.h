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


#ifndef COAJNR_INPUTMANAGER_INCLUDED
#define COAJNR_INPUTMANAGER_INCLUDED

#include "Dependencies.h"

#include "InputEvent.h"
#include "BufferedInputHandlerComponent.h"
#include "UnbufferedInputHandlerComponent.h"
#include "Singleton.h"

namespace CoAJnR
{


/**
 * handles input events
 */
class InputManager : public rtils::Singleton<InputManager>,
                     public OIS::KeyListener,
                     public OIS::MouseListener,
                     public OIS::JoyStickListener
{
    /// the ois input sub system
    OIS::InputManager* m_inputManager;
    
    /// the mouse input device
	OIS::Mouse* m_mouse;
	
	/// the keyboard input device
	OIS::Keyboard* m_keyboard;
	
	/// the joystick input device
	OIS::JoyStick* m_joystick;

    /// true if manager initialized
    bool m_initialized;
    
    /// list of registered UnbufferedInputHandlerComponent objects
    std::vector<UnbufferedInputHandlerComponentPtr> m_unbufferedHandlers;

    /// mutex to synchronize access to list of UnbufferedInputHandlerComponent objects    
    boost::mutex m_unbufferedHandlerVectorMutex;
    
    /// list of registered BufferedInputHandlerComponent objects for key press events
    std::vector<BufferedInputHandlerComponentPtr> m_keyPressHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to key press events
    boost::mutex m_keyPressHandlerVectorMutex;

    /// list of registered BufferedInputHandlerComponent objects for key release events
    std::vector<BufferedInputHandlerComponentPtr> m_keyReleaseHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to key release events
    boost::mutex m_keyReleaseHandlerVectorMutex;

    /// list of registered BufferedInputHandlerComponent objects for mouse move events
    std::vector<BufferedInputHandlerComponentPtr> m_mouseMoveHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to mouse move events
    boost::mutex m_mouseMoveHandlerVectorMutex;

    /// list of registered BufferedInputHandlerComponent objects for mouse press events
    std::vector<BufferedInputHandlerComponentPtr> m_mousePressHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to mouse press events
    boost::mutex m_mousePressHandlerVectorMutex;

    /// list of registered BufferedInputHandlerComponent objects for mouse release events
    std::vector<BufferedInputHandlerComponentPtr> m_mouseReleaseHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to mouse release events
    boost::mutex m_mouseReleaseHandlerVectorMutex;

    /// list of registered BufferedInputHandlerComponent objects for 
    /// joystick press events
    std::vector<BufferedInputHandlerComponentPtr> m_joystickPressHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to joystick press events
    boost::mutex m_joystickPressHandlerVectorMutex;

    // list of registered BufferedInputHandlerComponent objects for 
    // joystick release events
    std::vector<BufferedInputHandlerComponentPtr> m_joystickReleaseHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to joystick release events
    boost::mutex m_joystickReleaseHandlerVectorMutex;

    // list of registered BufferedInputHandlerComponent objects for 
    // joystick axis move events
    std::vector<BufferedInputHandlerComponentPtr> m_joystickAxisMoveHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to joystick axis move events
    boost::mutex m_joystickAxisMoveHandlerVectorMutex;

    // list of registered BufferedInputHandlerComponent objects for 
    // joystick slider move events
    std::vector<BufferedInputHandlerComponentPtr> m_joystickSliderMoveHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to joystick slider move events
    boost::mutex m_joystickSliderMoveHandlerVectorMutex;

    // list of registered BufferedInputHandlerComponent objects for 
    // joystick pov move events
    std::vector<BufferedInputHandlerComponentPtr> m_joystickPovMoveHandlers;

    /// mutex to synchronize access to list of BufferedInputHandlerComponent object
    /// registered to joystick pov events
    boost::mutex m_joystickPovMoveHandlerVectorMutex;
    
    /**
     * schedules all unbuffered input handlers using ThreadPool.
     */
    void updateUnbufferedHandlers();

public:
    /**
     * ctor.
     */
    InputManager();
    
    /**
     * dtor.
     */
    virtual ~InputManager();
    
    /** 
     * registers an UnbufferedInputHandlerComponent.
     *
     * the newly registered UnbufferedInputHandlerComponent will be updated during the
     * next InputManager loop.
     *
     * @note this method if thread safe due to a lock.
     */
    void addUnbufferedHandler(UnbufferedInputHandlerComponentPtr handler);

    /**
     * unregisters a already registered UnbufferedInputHandlerComponent.
     *
     * if the UnbufferedInputHandlerComponent was not found, false will be returned.
     *
     * @note this method if thread safe due to a lock.
     */
    bool removeUnbufferedHandler(UnbufferedInputHandlerComponentPtr handler);
    
    /**
     * registeres a BufferedInputHandlerComponent for a given set of events.
     *
     * @note this method if thread safe due to a lock.
     */     
    void addBufferedHandler(
            BufferedInputHandlerComponentPtr handler, InputEvent event);

    /**
     * unregisters a already registered BufferedInputHandlerComponent
     *
     * if the BufferedInputHandlerComponent was not found, false will be 
     * returned.
     *
     * @note this method if thread safe due to a lock.
     */
    bool removeBufferedHandler(BufferedInputHandlerComponentPtr handler);

    /**
     * initialize the input system.
     *
     * @param windowHandle the window to listen for events on
     * @param height the height of the window (needed for mouse pos values)
     * @param width the width of the window (needed for mouse pos values)
     *
     * @note this method is meant to get called by the graphics thread
     */
    void init(size_t windowHandle, uint height, uint width);
    
    /**
     * processes all input events and calls handler callback.
     *
     * @note this method is meant to get called by the logics thread
     */
    void update();
    
    /**
     * release the input system
     */
    void release();
    
    /**
     * handle key press events
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool keyPressed(const OIS::KeyEvent &arg);

    /**
     * handle key release events
     *
     * @note this method is meant to get called by the OIS subsystem
     */    
    bool keyReleased(const OIS::KeyEvent &arg);
    
    /**
     * handle mouse movement event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool mouseMoved(const OIS::MouseEvent& arg);

    /**
     * handle mouse button press event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool mousePressed( const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    /**
     * handle mouse button released event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool mouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id);

    /**
     * handle joystick button pressed event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool buttonPressed(const OIS::JoyStickEvent &arg, int button);

    /**
     * handle joystick button released event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool buttonReleased(const OIS::JoyStickEvent &arg, int button);

    /**
     * handle joystick axis movement event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool axisMoved(const OIS::JoyStickEvent &arg, int axis);

    /**
     * handle joystick slider movement event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool sliderMoved(const OIS::JoyStickEvent&, int id);

    /**
     * handle joystick pov movement event
     *
     * @note this method is meant to get called by the OIS subsystem
     */
    bool povMoved(const OIS::JoyStickEvent&, int id);
    
    /**
     * returns a pointer to the keyboard object
     *
     * @note access to the keyboard object is not thread safe
     */
    OIS::Keyboard* keyboard() const 
    { 
        assert(m_keyboard);
        return m_keyboard; 
    }

    /**
     * returns a pointer to the mouse object
     *
     * @note access to the mouse object is not thread safe
     */
    OIS::Mouse* mouse() const
    { 
        assert(m_mouse);
        return m_mouse; 
    }

    /**
     * returns a pointer to the joystick object
     *
     * @note access to the joystick object is not thread safe
     */
    OIS::JoyStick* joystick() const
    { 
        assert(m_joystick);
        return m_joystick; 
    }
    
    /**
     * whether the InputManager is initialised
     */
    bool isInitialized() const
    {
        return m_initialized;
    }

};

}

#endif

