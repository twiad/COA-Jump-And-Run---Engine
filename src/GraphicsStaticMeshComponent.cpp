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

#include "GraphicsStaticMeshComponent.h"

#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

GraphicsStaticMeshComponent::GraphicsStaticMeshComponent(
        const std::string& p_meshName,
        const Ogre::Vector3& p_offset,
        const Ogre::Quaternion& p_offsetRotation) :
    // m_sceneNode(0), 
    m_staticGeometry(0), 
    m_entity(0), 
    m_meshName(p_meshName),
    m_offset(p_offset),
    m_offsetRotation(p_offsetRotation)
{
}

GraphicsStaticMeshComponent::~GraphicsStaticMeshComponent()
{
}

std::string
GraphicsStaticMeshComponent::defaultName() const
{
    return std::string("GraphicsStaticMeshMain");
}

std::vector<std::string>
GraphicsStaticMeshComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("Position");

    return dependencies;
}

void
GraphicsStaticMeshComponent::attachedCallback()
{
    assert(parent() && "parent must not be null");
    assert(parent()->id() && "component container has invalid ObjectId");

    // get dependencies
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));

    struct CreateGraphicsObjectCall : public DeferredCall
    {
        GraphicsStaticMeshComponentPtr component;
        std::string fileName;
        PositionComponentPtr positionComponent;
        Ogre::Vector3 offset;
        Ogre::Quaternion offsetRotation;

        bool operator()()
        {
            Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

            Ogre::Entity* entity = sm->createEntity(
                    Ogre::StringConverter::toString(component->localId()), 
                    fileName);

            Ogre::StaticGeometry* sg = sm->createStaticGeometry(
                    Ogre::StringConverter::toString(component->localId()));

            sg->addEntity(
                    entity,
                    positionComponent->sceneNode()->getPosition() + offset,
                    positionComponent->sceneNode()->getOrientation() * 
                            offsetRotation);

            sg->build();

            component->m_entity = entity;
            component->m_staticGeometry = sg;

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentAttachedCallback,
                    component->parent(), component));

            return true;
        }
    };

    CreateGraphicsObjectCall* call = new CreateGraphicsObjectCall();
    call->component = shared_from_this();
    call->fileName = m_meshName;
    call->positionComponent = m_positionComponent;
    call->offset = m_offset;
    call->offsetRotation = m_offsetRotation;

    GraphicsManager::get().enqueueCall(call);
}

void
GraphicsStaticMeshComponent::detachedCallback()
{
    struct DestroyGraphicsObjectCall : public DeferredCall
    {
        ObjectId id;
        GraphicsStaticMeshComponentPtr component;

        bool operator()()
        {
            if(!component->ready())
                return false;

            Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

            sm->destroyStaticGeometry(
                    Ogre::StringConverter::toString(component->localId()));

            sm->destroyEntity(
                    Ogre::StringConverter::toString(component->localId()));

            component->m_staticGeometry = 0;
            component->m_entity = 0;

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentDetachedCallback,
                    component->parent(), component));

            return true;
        }
    };

    DestroyGraphicsObjectCall* call = new DestroyGraphicsObjectCall();
    call->id = parent()->id();
    call->component = shared_from_this();

    GraphicsManager::get().enqueueCall(call);
}

}
