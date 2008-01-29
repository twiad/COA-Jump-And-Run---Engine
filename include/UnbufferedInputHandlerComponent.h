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


#ifndef COAJNR_UNBUFFEREDINPUTHANDLERCOMPONENT_INCLUDED
#define COAJNR_UNBUFFEREDINPUTHANDLERCOMPONENT_INCLUDED

#include "Dependencies.h"

#include "Component.h"
#include "TimeManager.h"

namespace CoAJnR
{

/**
 * handles unbuffered input events from the InputManager.
 */
class UnbufferedInputHandlerComponent : 
        public Component,
        public boost::enable_shared_from_this<UnbufferedInputHandlerComponent>
{

    virtual void attachedCallback();
    virtual void detachedCallback();

    TimeStamp m_lastUpdate;
    mutable boost::mutex m_mutex;

public:
    UnbufferedInputHandlerComponent();
    virtual ~UnbufferedInputHandlerComponent();
    
    /**
     * handles unbuffered input.
     *
     * @note this method will be called from the thread pool.
     *
     * @note to handle keyboard input, you have to ask the keyboard object
     * provided as a parameter. this is the same object used by OIS and will
     * be potetially written by OIS while this method is checking for key downs.
     * this is only possible, because OIS Version 1.0 uses a char array for
     * mac, linux and windows key states. and accessing this char array as 
     * boolean values should be thread safe. 
     * this may break with newer version of OIS. BE WARNED!!!
     * if provided, use a key state struct instead.
     */
    virtual void update(
            TimeValue timeSinceLastUpdate,
            OIS::Keyboard* keyboard,
            OIS::MouseState mouseState, 
            OIS::JoyStickState joystickState) = 0;
    
    void updateInternal(
                OIS::Keyboard* keyboard,
                OIS::MouseState mouseState, 
                OIS::JoyStickState joystickState);
};

typedef boost::shared_ptr<UnbufferedInputHandlerComponent>
        UnbufferedInputHandlerComponentPtr;

}

#endif

