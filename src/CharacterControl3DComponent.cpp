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


#include "CharacterControl3DComponent.h"
#include "DeferredCall.h"
#include "CollisionObject.h"
#include "ThreadPool.h"
#include "PhysicsManager.h"

namespace CoAJnR
{

CharacterControl3DComponent::CharacterControl3DComponent()
{
    
}

CharacterControl3DComponent::~CharacterControl3DComponent()
{
    
}

std::vector<std::string> 
CharacterControl3DComponent::dependencies()
{
    std::vector<std::string> deps;
    deps.push_back(std::string("Position"));
    deps.push_back(std::string("PhysicsMain"));
    return deps;
}

std::string
CharacterControl3DComponent::defaultName() const
{
    return std::string("CharacterControl3D");
}

void
CharacterControl3DComponent::attachedCallback()
{
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));
    
    m_physicsComponent = 
        boost::dynamic_pointer_cast<PhysicsComponent>(
                parent()->component("PhysicsMain"));


    m_positionComponent->setManualRotationUpdate(true);
    m_positionComponent->setOrientation(Ogre::Quaternion::IDENTITY);


    DEFERRED_CALL_VARS(DisableAngularFactorCall)
        PhysicsComponentPtr physicsComponent;
        CharacterControl3DComponentPtr component;
    
    DEFERRED_CALL_CODE()
        if(!physicsComponent->ready())
            return false;

        physicsComponent->collisionObject()->
                setActivationState(DISABLE_DEACTIVATION); 

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));
    
        return true;
    
    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(DisableAngularFactorCall)
        call->component = shared_from_this();
        call->physicsComponent = m_physicsComponent;
    
    DEFERRED_CALL_ENQUEUE(PhysicsManager)
}

void
CharacterControl3DComponent::detachedCallback()
{
    m_positionComponent->setManualRotationUpdate(false);

    DEFERRED_CALL_VARS(ReenableAngularFactorCall)
        PhysicsComponentPtr physicsComponent;
        CharacterControl3DComponentPtr component;
    
    DEFERRED_CALL_CODE()
        if(!physicsComponent->ready())
            return false;

        physicsComponent->collisionObject()->
                setActivationState(WANTS_DEACTIVATION); 

        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentDetachedCallback,
                component->parent(), component));
    
        return true;
    
    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(ReenableAngularFactorCall)
        call->component = shared_from_this();
        call->physicsComponent = m_physicsComponent;
    
    DEFERRED_CALL_ENQUEUE(PhysicsManager)
}

void 
CharacterControl3DComponent::forward(double p_amount)
{
    m_physicsComponent->applyImpulse(
            m_positionComponent->orientation() * 
            Ogre::Vector3(0,0,-p_amount * 10.0f));
}

void 
CharacterControl3DComponent::backward(double p_amount)
{
    forward(-p_amount);
}

void 
CharacterControl3DComponent::turnLeft(double p_amount)
{
    m_positionComponent->setOrientation(
            m_positionComponent->orientation() * 
            Ogre::Quaternion(
                    Ogre::Radian(p_amount * 2.0f),
                    Ogre::Vector3(0,1,0)));
}

void 
CharacterControl3DComponent::turnRight(double p_amount)
{
    turnLeft(-p_amount);
}

void 
CharacterControl3DComponent::jump(double p_amount)
{
    DEFERRED_CALL_VARS(JumpCall)
        PhysicsComponentPtr component;
        Ogre::Vector3 origin;
    
    DEFERRED_CALL_CODE()
        if(!component->ready())
            return false;
        
        btCollisionWorld::ClosestRayResultCallback rayResult(
                btVector3(0,0,0), btVector3(0,0,0));
        
        PhysicsManager::get().world()->rayTest(
                btVector3(origin.x, origin.y - 0, origin.z),
                btVector3(origin.x, origin.y - 1.3f, origin.z),
                rayResult);

        if(rayResult.HasHit())
        {
            std::cout << "applying jump force" << std::endl;
            component->applyForce(Ogre::Vector3(0,60,0));
        }
        return true;
        
    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(JumpCall)
        call->component = m_physicsComponent;
        call->origin = m_positionComponent->position();
    
    DEFERRED_CALL_ENQUEUE(PhysicsManager)
}


}
