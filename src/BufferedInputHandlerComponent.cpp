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

#include "BufferedInputHandlerComponent.h"

#include "InputManager.h"
#include "ThreadPool.h"
#include "ComponentContainer.h"

namespace CoAJnR
{

BufferedInputHandlerComponent::BufferedInputHandlerComponent(
        InputEvent p_events) : 
    m_events(p_events)
{

}

BufferedInputHandlerComponent::~BufferedInputHandlerComponent()
{
    
}

void 
BufferedInputHandlerComponent::attachedCallback()
{
    InputManager::get().addBufferedHandler(shared_from_this(), m_events);
    
    ThreadPool::get().schedule(boost::bind(
            &ComponentContainer::componentAttachedCallback,
            this->parent(), 
            shared_from_this()));
    
}

void 
BufferedInputHandlerComponent::detachedCallback()
{
    InputManager::get().removeBufferedHandler(shared_from_this());

    ThreadPool::get().schedule(boost::bind(
            &ComponentContainer::componentDetachedCallback,
            this->parent(), 
            shared_from_this()));
}

void 
BufferedInputHandlerComponent::keyPressedInternal(const OIS::KeyEvent arg)
{
    boost::mutex::scoped_lock lock(m_mutex);
    keyPressed(arg);
}

void 
BufferedInputHandlerComponent::keyReleasedInternal(const OIS::KeyEvent arg)
{
    boost::mutex::scoped_lock lock(m_mutex);
    keyReleased(arg);
}

void 
BufferedInputHandlerComponent::mouseMovedInternal(const OIS::MouseEvent arg)
{
    boost::mutex::scoped_lock lock(m_mutex);
    mouseMoved(arg);    
}

void 
BufferedInputHandlerComponent::mousePressedInternal(
        const OIS::MouseEvent arg, OIS::MouseButtonID id)
{
    boost::mutex::scoped_lock lock(m_mutex);
    mousePressed(arg, id);
}

void 
BufferedInputHandlerComponent::mouseReleasedInternal(
        const OIS::MouseEvent arg, OIS::MouseButtonID id)
{
    boost::mutex::scoped_lock lock(m_mutex);
    mouseReleased(arg, id);
}

void 
BufferedInputHandlerComponent::buttonPressedInternal(
        const OIS::JoyStickEvent arg, int button)
{
    boost::mutex::scoped_lock lock(m_mutex);
    buttonPressed(arg, button);
}

void 
BufferedInputHandlerComponent::buttonReleasedInternal(
        const OIS::JoyStickEvent arg, int button)
{
    boost::mutex::scoped_lock lock(m_mutex);
    buttonReleased(arg, button);
}

void 
BufferedInputHandlerComponent::axisMovedInternal(
        const OIS::JoyStickEvent arg, int axis)
{
    boost::mutex::scoped_lock lock(m_mutex);
    axisMoved(arg, axis);
}

void 
BufferedInputHandlerComponent::sliderMovedInternal(
        const OIS::JoyStickEvent arg, int id)
{
    boost::mutex::scoped_lock lock(m_mutex);
    sliderMoved(arg, id);
}

void 
BufferedInputHandlerComponent::povMovedInternal(
        const OIS::JoyStickEvent arg, int id)
{
    boost::mutex::scoped_lock lock(m_mutex);
    povMoved(arg, id);
}

}
