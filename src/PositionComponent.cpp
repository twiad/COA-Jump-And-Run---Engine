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

#include "PositionComponent.h"

#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

PositionComponent::PositionComponent(
        const Ogre::Vector3 p_position, 
        const Ogre::Quaternion p_orientation,
        bool p_manualRotationUpdate) 
    :   m_position(p_position), 
        m_orientation(p_orientation),
        m_sceneNode(0),
        m_manualRotationUpdate(p_manualRotationUpdate)
{
}

PositionComponent::~PositionComponent()
{
    std::cout << "position component dtor" << std::endl;
}

std::string
PositionComponent::defaultName() const
{
    return std::string("Position");
}

void
PositionComponent::attachedCallback()
{
    DEFERRED_CALL_VARS(CreatePositionCall)
        PositionComponentPtr component;
        Ogre::Vector3 position;
        Ogre::Quaternion orientation;

    DEFERRED_CALL_CODE()
        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        Ogre::SceneNode* sceneNode =
                sm->getRootSceneNode()->createChildSceneNode(
                        Ogre::StringConverter::toString(
                                component->localId()),
                        position,
                        orientation);

        component->m_sceneNode = sceneNode;

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));

        return true;
    
    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(CreatePositionCall)
        call->component = shared_from_this();
        call->position = m_position;
        call->orientation = m_orientation;

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void
PositionComponent::detachedCallback()
{
    DEFERRED_CALL_VARS(DestroyPositionCall)
        PositionComponentPtr component;
    
    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        sm->destroySceneNode(
                Ogre::StringConverter::toString(component->localId()));

        component->m_sceneNode = 0;

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentDetachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(DestroyPositionCall)
        call->component = shared_from_this();

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

Ogre::Vector3 
PositionComponent::position() const
{
    boost::mutex::scoped_lock lock(m_mutex);
    return m_position;
}

Ogre::Quaternion 
PositionComponent::orientation() const
{
    boost::mutex::scoped_lock lock(m_mutex);
    return m_orientation;
}


Ogre::Vector3 
PositionComponent::velocity() const
{
    boost::mutex::scoped_lock lock(m_mutex);
    return m_velocity;
}

void 
PositionComponent::setPosition(const Ogre::Vector3& p_position)
{
    dataChangedCallback(p_position, m_orientation, m_velocity);
}

void 
PositionComponent::setOrientation(const Ogre::Quaternion& p_orientation)
{
    dataChangedCallback(m_position, p_orientation, m_velocity, true);
}

void 
PositionComponent::dataChangedCallback(
        const Ogre::Vector3& p_position, 
        const Ogre::Quaternion& p_orientation,
        const Ogre::Vector3& p_velocity,
        bool p_force)
{
    struct GraphicsMeshComponentUpdateCall : public DeferredCall
    {
        PositionComponentPtr component;
        Ogre::SceneNode* sceneNode;
        Ogre::Vector3 position;
        Ogre::Quaternion orientation;
        bool doRotationUpdate;

        bool operator()()
        {
            if(!component->ready())
                return false;

            component->sceneNode()->setPosition(position);

            if(doRotationUpdate)
                component->sceneNode()->setOrientation(orientation);        

            return true;
        }
    };

    GraphicsMeshComponentUpdateCall* call = 
            new GraphicsMeshComponentUpdateCall;

    call->component = shared_from_this();
    call->orientation = p_orientation;
    call->position = p_position;

    call->doRotationUpdate = p_force || !m_manualRotationUpdate;

    GraphicsManager::get().enqueueCallDroppable(call);

    boost::mutex::scoped_lock lock(m_mutex);
    m_velocity = p_velocity;
    m_position = p_position;
    
    if(p_force || !m_manualRotationUpdate)
        m_orientation = p_orientation;
}

void 
PositionComponent::setManualRotationUpdate(bool p_flag)
{
    m_manualRotationUpdate = p_flag;
}

}
