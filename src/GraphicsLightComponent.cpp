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

#include "GraphicsLightComponent.h"

#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{
GraphicsLightComponent::GraphicsLightComponent(
        LightType p_type,
        const Ogre::Vector3& p_offset,
        const Ogre::Quaternion& p_offsetRotation,
        Ogre::Radian p_outerAngle,
        Ogre::Radian p_innerAngle,
        double p_fallOff) :
    m_sceneNode(0),
    m_light(0), 
    m_type(p_type),
    m_offset(p_offset), 
    m_offsetRotation(p_offsetRotation),
    m_outerAngle(p_outerAngle),
    m_innerAngle(p_innerAngle),
    m_fallOff(p_fallOff)
{
    
}

GraphicsLightComponent::~GraphicsLightComponent()
{
}

std::string
GraphicsLightComponent::defaultName() const
{
    return std::string("Light");
}

std::vector<std::string>
GraphicsLightComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("Position");

    return dependencies;
}

void
GraphicsLightComponent::attachedCallback()
{
    // get dependencies
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));


    DEFERRED_CALL_VARS(CreateGraphicsObjectCall)
        GraphicsLightComponentPtr component;
        PositionComponentPtr positionComponent;
        Ogre::Vector3 offset;
        Ogre::Quaternion offsetRotation;
        LightType type;
        Ogre::Radian outerAngle;
        Ogre::Radian innerAngle;
        double fallOff;

    DEFERRED_CALL_CODE()
        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        Ogre::Light* light = sm->createLight(
                Ogre::StringConverter::toString(component->localId()));

        Ogre::SceneNode* sceneNode =
                positionComponent->sceneNode()->createChildSceneNode(
                        Ogre::StringConverter::toString(component->localId()),
                        offset, 
                        offsetRotation);


        switch(type)
        {
            case LT_Directional:
                light->setType(Ogre::Light::LT_DIRECTIONAL);
            break;
            case LT_Spot:
                light->setType(Ogre::Light::LT_SPOTLIGHT);
                light->setSpotlightInnerAngle(innerAngle);
                light->setSpotlightOuterAngle(outerAngle);
                light->setSpotlightFalloff(fallOff);
            break;
            default:
                light->setType(Ogre::Light::LT_POINT);
        }

        sceneNode->attachObject(light);

        component->m_light = light;
        component->m_sceneNode = sceneNode;

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(CreateGraphicsObjectCall)
        call->component = shared_from_this();
        call->positionComponent = m_positionComponent;
        call->offset = m_offset;
        call->offsetRotation = m_offsetRotation;
        call->fallOff = m_fallOff;
        call->innerAngle = m_innerAngle;
        call->outerAngle = m_outerAngle;
        call->type = m_type;
        
    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void
GraphicsLightComponent::detachedCallback()
{
    DEFERRED_CALL_VARS(DestroyGraphicsObjectCall)
        ObjectId id;
        GraphicsLightComponentPtr component;

    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;

        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        sm->destroyLight(
                Ogre::StringConverter::toString(component->localId()));

        component->m_light = 0;
        
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
GraphicsLightComponent::setPositionOffset(Ogre::Vector3 p_position)
{
    DEFERRED_CALL_VARS(SetPositionCall)
        GraphicsLightComponentPtr component;
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
GraphicsLightComponent::setRotationOffset(Ogre::Quaternion p_rotation)
{
    DEFERRED_CALL_VARS(SetOrientationCall)
        GraphicsLightComponentPtr component;
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
