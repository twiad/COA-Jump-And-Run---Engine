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

#include "GraphicsCameraComponent.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ComponentContainer.h"
#include "ThreadPool.h"

namespace CoAJnR
{

GraphicsCameraComponent::GraphicsCameraComponent(
        bool p_autoEnable,
        const Ogre::Vector3& p_offset,
        const Ogre::Quaternion& p_offsetRotation) :
    m_sceneNode(0),
    m_camera(0), 
    m_offset(p_offset), 
    m_offsetRotation(p_offsetRotation),
    m_autoEnable(p_autoEnable)
{

}

GraphicsCameraComponent::~GraphicsCameraComponent()
{

}

std::vector<std::string>
GraphicsCameraComponent::dependencies()
{
    std::vector<std::string> deps;
    
    deps.push_back("Position");
    
    return deps;
}

std::string
GraphicsCameraComponent::defaultName() const
{
    return std::string("Camera");
}

void
GraphicsCameraComponent::attachedCallback()
{
    // get dependencies
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));


    DEFERRED_CALL_VARS(CreateCameraCall)
        GraphicsCameraComponentPtr component;
        PositionComponentPtr positionComponent;
        Ogre::Vector3 offset;
        Ogre::Quaternion offsetRotation;
        bool autoEnable;

    DEFERRED_CALL_CODE()
        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        Ogre::SceneNode* sceneNode =
                positionComponent->sceneNode()->createChildSceneNode(
                        Ogre::StringConverter::toString(
                                component->localId()),
                        offset, 
                        offsetRotation);

        component->m_sceneNode = sceneNode;

        Ogre::Camera* camera = sm->createCamera(
                Ogre::StringConverter::toString(component->localId()));

        camera->setNearClipDistance(1);
        camera->setFarClipDistance(100);

        component->m_camera = camera;

        sceneNode->attachObject(camera);

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));

        if(autoEnable)
            component->enable();

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(CreateCameraCall)
        call->component = shared_from_this();
        call->positionComponent = m_positionComponent;
        call->offset = m_offset;
        call->offsetRotation = m_offsetRotation;
        call->autoEnable = m_autoEnable;

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void
GraphicsCameraComponent::detachedCallback()
{
    DEFERRED_CALL_VARS(DestroyCameraCall)
        GraphicsCameraComponentPtr component;
        Ogre::Vector3 offset;
        Ogre::Quaternion offsetRotation;

    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        sm->destroyCamera(
                Ogre::StringConverter::toString(component->localId()));

        component->m_camera = 0;
    
        sm->destroySceneNode(
                Ogre::StringConverter::toString(component->localId()));
        
        component->m_sceneNode = 0;

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentDetachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(DestroyCameraCall)
        call->component = shared_from_this();
        call->offset = m_offset;
        call->offsetRotation = m_offsetRotation;

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void
GraphicsCameraComponent::enable()
{
    DEFERRED_CALL_VARS(EnableCameraCall)
        GraphicsCameraComponentPtr component;

    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        Ogre::Viewport* vp = GraphicsManager::get().viewport();

        vp->setCamera(component->m_camera);
        component->m_camera->setAspectRatio(
                vp->getActualWidth() / vp->getActualHeight());
        
        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(EnableCameraCall)
        call->component = shared_from_this();

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

}
