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

#include "UnbufferedInputHandlerComponent.h"

#include "InputManager.h"
#include "ThreadPool.h"
#include "ComponentContainer.h"

namespace CoAJnR
{

UnbufferedInputHandlerComponent::UnbufferedInputHandlerComponent() :
    m_lastUpdate(0)
{
    
}

UnbufferedInputHandlerComponent::~UnbufferedInputHandlerComponent()
{
    
}

void 
UnbufferedInputHandlerComponent::attachedCallback()
{
    InputManager::get().addUnbufferedHandler(shared_from_this());

    ThreadPool::get().schedule(boost::bind(
            &ComponentContainer::componentAttachedCallback,
            this->parent(), 
            shared_from_this()));
}

void 
UnbufferedInputHandlerComponent::detachedCallback()
{
    InputManager::get().removeUnbufferedHandler(shared_from_this());

    ThreadPool::get().schedule(boost::bind(
            &ComponentContainer::componentDetachedCallback,
            this->parent(), 
            shared_from_this()));
}

void 
UnbufferedInputHandlerComponent::updateInternal(
            OIS::Keyboard* keyboard,
            OIS::MouseState mouseState, 
            OIS::JoyStickState joystickState)
{
    boost::mutex::scoped_lock lock(m_mutex);

    TimeStamp ts = TimeManager::get().timeStamp();
    TimeValue timeSinceLastUpdate = ts - m_lastUpdate;
    m_lastUpdate = ts;
    
    update(timeSinceLastUpdate, keyboard, mouseState, joystickState);
}

}
