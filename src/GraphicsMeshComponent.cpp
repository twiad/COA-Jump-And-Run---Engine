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

#include "GraphicsMeshComponent.h"

#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

GraphicsMeshComponent::GraphicsMeshComponent(
        const std::string& p_meshName, 
        const Ogre::Vector3& p_offset,
        const Ogre::Quaternion& p_offsetRotation) :
    m_sceneNode(0),
    m_entity(0), 
    m_meshName(p_meshName), 
    m_offset(p_offset), 
    m_offsetRotation(p_offsetRotation)
{
}

GraphicsMeshComponent::~GraphicsMeshComponent()
{
}

std::string
GraphicsMeshComponent::defaultName() const
{
    return std::string("GraphicsMeshMain");
}

std::vector<std::string>
GraphicsMeshComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("Position");

    return dependencies;
}

void
GraphicsMeshComponent::attachedCallback()
{
    // get dependencies
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));


    DEFERRED_CALL_VARS(CreateGraphicsObjectCall)
        ObjectId id;
        GraphicsMeshComponentPtr component;
        PositionComponentPtr positionComponent;
        std::string fileName;
        Ogre::Vector3 offset;
        Ogre::Quaternion offsetRotation;

    DEFERRED_CALL_CODE()
        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        Ogre::Entity* entity = sm->createEntity(
                Ogre::StringConverter::toString(component->localId()),
                fileName);

        Ogre::SceneNode* sceneNode =
                positionComponent->sceneNode()->createChildSceneNode(
                        Ogre::StringConverter::toString(component->localId()),
                        offset, 
                        offsetRotation);

        sceneNode->attachObject(entity);

        component->m_entity = entity;
        component->m_sceneNode = sceneNode;

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(CreateGraphicsObjectCall)
        call->id = parent()->id();
        call->component = shared_from_this();
        call->positionComponent = m_positionComponent;
        call->fileName = m_meshName;
        call->offset = m_offset;
        call->offsetRotation = m_offsetRotation;

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void
GraphicsMeshComponent::detachedCallback()
{
    DEFERRED_CALL_VARS(DestroyGraphicsObjectCall)
        ObjectId id;
        GraphicsMeshComponentPtr component;

    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        sm->destroyEntity(
                Ogre::StringConverter::toString(component->localId()));

        component->m_entity = 0;
        
        sm->destroySceneNode(
                Ogre::StringConverter::toString(component->localId()));
                
        component->m_sceneNode = 0;

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentDetachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(DestroyGraphicsObjectCall)
        call->id = parent()->id();
        call->component = shared_from_this();

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void 
GraphicsMeshComponent::setPositionOffset(Ogre::Vector3 p_position)
{
    DEFERRED_CALL_VARS(SetPositionCall)
        GraphicsMeshComponentPtr component;
        Ogre::Vector3 position;

    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        component->sceneNode()->setPosition(position);

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(SetPositionCall)
        call->component = shared_from_this();
        call->position = p_position;

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void 
GraphicsMeshComponent::setRotationOffset(Ogre::Quaternion p_rotation)
{
    DEFERRED_CALL_VARS(SetOrientationCall)
        GraphicsMeshComponentPtr component;
        Ogre::Quaternion rotation;

    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        component->sceneNode()->setOrientation(rotation);

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(SetOrientationCall)
        call->component = shared_from_this();
        call->rotation = p_rotation;

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

}
