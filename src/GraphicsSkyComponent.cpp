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

#include "GraphicsSkyComponent.h"

#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

GraphicsSkyComponent::GraphicsSkyComponent(
        const std::string& p_name, 
        SkyType p_skyType) :
    m_name(p_name), m_type(p_skyType)
{
}

GraphicsSkyComponent::~GraphicsSkyComponent()
{
}

std::string
GraphicsSkyComponent::defaultName() const
{
    return std::string("GraphicsSkybox");
}

void
GraphicsSkyComponent::attachedCallback()
{
    DEFERRED_CALL_VARS(CreateSkyboxCall)
        GraphicsSkyComponentPtr component;
        std::string name;
        SkyType type;

    DEFERRED_CALL_CODE()
        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        switch(type)
        {
            // case ST_Plane: 
            //     sm->setSkyPlane(true, name);
            // break;
            case ST_Dome:
                sm->setSkyDome(true, name, 10, -1);
            break;
            default:
                sm->setSkyBox(true, name);
        }
        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(CreateSkyboxCall)
        call->component = shared_from_this();
        call->name = m_name;
        call->type = m_type;
    
    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void
GraphicsSkyComponent::detachedCallback()
{
    DEFERRED_CALL_VARS(DisableSkyboxCall)
        GraphicsSkyComponentPtr component;
        std::string name;
        SkyType type;

    DEFERRED_CALL_CODE()
        Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

        switch(type)
        {
            // case ST_Plane: 
            //     sm->setSkyPlane(false, name);
            // break;
            case ST_Dome:
                sm->setSkyDome(false, name);
            break;
            default:
                sm->setSkyBox(false, name);
        }

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentDetachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(DisableSkyboxCall)
        call->component = shared_from_this();
        call->name = m_name;
        call->type = m_type;
    
    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

}
