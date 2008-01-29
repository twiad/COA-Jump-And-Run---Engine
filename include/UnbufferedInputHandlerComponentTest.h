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


#ifndef COAJNR_UNBUFFEREDINPUTHANDLERTEST_INCLUDED
#define COAJNR_UNBUFFEREDINPUTHANDLERTEST_INCLUDED

#include "Dependencies.h"

#include "ComponentContainer.h"
#include "DataStorage.h"
#include "GraphicsMeshComponent.h"
#include "PhysicsComponent.h"
#include "UnbufferedInputHandlerComponent.h"
#include "TimeManager.h"

namespace CoAJnR
{

class UnbufferedInputHandlerComponentTest : 
        public UnbufferedInputHandlerComponent
{
    ComponentContainerPtr m_obj;

    virtual std::string defaultName() const;


public:
    UnbufferedInputHandlerComponentTest();
    
    virtual void update(
            TimeValue timeSinceLastUpdate,
            OIS::Keyboard* keyboard,
            OIS::MouseState mouseState, 
            OIS::JoyStickState joystickState);
};

}

#endif

