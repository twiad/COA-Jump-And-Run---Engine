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

#include "GraphicsParticleComponent.h"

#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

GraphicsParticleComponent::GraphicsParticleComponent(
        const std::string& p_particleSystemName, 
        const Ogre::Vector3& p_offset,
        const Ogre::Quaternion& p_offsetRotation) :
    m_sceneNode(0), 
    m_particleSystem(0), 
    m_particleSystemName(p_particleSystemName),
    m_offset(p_offset),
    m_offsetRotation(p_offsetRotation)
{
}

GraphicsParticleComponent::~GraphicsParticleComponent()
{
}

std::string
GraphicsParticleComponent::defaultName() const
{
    return std::string("GraphicsParticle");
}

std::vector<std::string>
GraphicsParticleComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("Position");

    return dependencies;
}

void
GraphicsParticleComponent::attachedCallback()
{
    // get dependencies
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));

    struct CreateGraphicsObjectCall : public DeferredCall
    {
        GraphicsParticleComponentPtr component;
        PositionComponentPtr positionComponent;
        std::string fileName;
        Ogre::Vector3 offset;
        Ogre::Quaternion offsetRotation;

        bool operator()()
        {
            Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

            Ogre::SceneNode* sceneNode =
                    positionComponent->sceneNode()->createChildSceneNode(
                            Ogre::StringConverter::toString(
                                    component->localId()),
                            offset, 
                            offsetRotation);

            Ogre::ParticleSystem* ps = sm->createParticleSystem(
                    Ogre::StringConverter::toString(component->localId()), 
                    fileName);

            sceneNode->attachObject(ps);

            component->m_sceneNode = sceneNode;
            component->m_particleSystem = ps;

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentAttachedCallback,
                    component->parent(), component));

            return true;
        }
    };

    CreateGraphicsObjectCall* call = new CreateGraphicsObjectCall();
    call->component = shared_from_this();
    call->positionComponent = m_positionComponent;
    call->fileName = m_particleSystemName;
    call->offset = m_offset;
    call->offsetRotation = m_offsetRotation;

    GraphicsManager::get().enqueueCall(call);
}

void
GraphicsParticleComponent::detachedCallback()
{
    struct DestroyGraphicsObjectCall : public DeferredCall
    {
        GraphicsParticleComponentPtr component;

        bool operator()()
        {
            if(!component->ready())
                return false;

            Ogre::SceneManager* sm = GraphicsManager::get().sceneManager();

            sm->destroySceneNode(
                    Ogre::StringConverter::toString(component->localId()));

            sm->destroyEntity(
                    Ogre::StringConverter::toString(component->localId()));

            component->m_sceneNode = 0;
            component->m_particleSystem = 0;

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentDetachedCallback,
                    component->parent(), component));

            return true;
        }
    };

    DestroyGraphicsObjectCall* call = new DestroyGraphicsObjectCall();
    call->component = shared_from_this();

    GraphicsManager::get().enqueueCall(call);
}

}
