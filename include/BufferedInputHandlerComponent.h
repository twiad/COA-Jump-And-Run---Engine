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


#ifndef COAJNR_BUFFEREDINPUTHANDLERCOMPONENT_INCLUDED
#define COAJNR_BUFFEREDINPUTHANDLERCOMPONENT_INCLUDED

#include "Dependencies.h"

#include "Component.h"

#include "InputEvent.h"

namespace CoAJnR
{

/**
 * handles buffered input events from the InputManager.
 */
class BufferedInputHandlerComponent :
        public Component,
        public boost::enable_shared_from_this<
                BufferedInputHandlerComponent>
{

    virtual void attachedCallback();
    virtual void detachedCallback();
    
    InputEvent m_events;
    
    mutable boost::mutex m_mutex;

public:
    BufferedInputHandlerComponent(InputEvent events = InputEvent(0xFFFFFFFF));
    
    virtual ~BufferedInputHandlerComponent();
    
    /**
     * handle key press events
     *
     * @note this method will be called from the thread pool.
     */
    virtual void keyPressed(const OIS::KeyEvent arg) { }

    void keyPressedInternal(const OIS::KeyEvent arg);

    /**
     * handle key release events
     *
     * @note this method will be called from the thread pool.
     */
    virtual void keyReleased(const OIS::KeyEvent arg) { }

    void keyReleasedInternal(const OIS::KeyEvent arg);
    
    /**
     * handle mouse movement event
     * 
     * @note this method will be called from the thread pool.
     */
    virtual void mouseMoved(const OIS::MouseEvent arg) { }

    void mouseMovedInternal(const OIS::MouseEvent arg);

    /**
     * handle mouse button press event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void mousePressed(
            const OIS::MouseEvent arg, OIS::MouseButtonID id) { }

    void mousePressedInternal(
            const OIS::MouseEvent arg, OIS::MouseButtonID id);

    /**
     * handle mouse button released event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void mouseReleased(
            const OIS::MouseEvent arg, OIS::MouseButtonID id) { }

    void mouseReleasedInternal(
            const OIS::MouseEvent arg, OIS::MouseButtonID id);

    /**
     * handle joystick button pressed event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void buttonPressed(const OIS::JoyStickEvent arg, int button) { }
    
    void buttonPressedInternal(const OIS::JoyStickEvent arg, int button);

    /**
     * handle joystick button released event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void buttonReleased(const OIS::JoyStickEvent arg, int button) { }

    void buttonReleasedInternal(const OIS::JoyStickEvent arg, int button);

    /**
     * handle joystick axis movement event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void axisMoved(const OIS::JoyStickEvent arg, int axis) { }

    void axisMovedInternal(const OIS::JoyStickEvent arg, int axis);

    /**
     * handle joystick slider movement event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void sliderMoved(const OIS::JoyStickEvent arg, int id) { }

    void sliderMovedInternal(const OIS::JoyStickEvent arg, int id);

    /**
     * handle joystick pov movement event
     *
     * @note this method will be called from the thread pool.
     */
    virtual void povMoved(const OIS::JoyStickEvent arg, int id) { }

    void povMovedInternal(const OIS::JoyStickEvent arg, int id);    
};

typedef boost::shared_ptr<BufferedInputHandlerComponent>
        BufferedInputHandlerComponentPtr;

}

#endif

