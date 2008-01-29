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

#include "PhysicsComponent.h"
#include "PhysicsConstraintsComponent.h"
#include "CollisionObject.h"
#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "GraphicsMeshComponent.h"
#include "MeshToShapeConverter.h"
#include "PhysicsManager.h"
#include "LogicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

PhysicsConstraintsComponent::PhysicsConstraintsComponent()
{

};

PhysicsConstraintsComponent::~PhysicsConstraintsComponent()
{
}

std::string 
PhysicsConstraintsComponent::defaultName() const
{
    return std::string("PhysicsConstraintMain");
}

std::vector<std::string>
PhysicsConstraintsComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("PhysicsMain");

    return dependencies;
}

void
PhysicsConstraintsComponent::attachedCallback()
{
    assert(parent() && "parent must not be null");
    assert(parent()->id() && "component container has invalid ObjectId");

    m_physicsComponent = boost::dynamic_pointer_cast<PhysicsComponent>(
            parent()->component("PhysicsMain"));

    assert(m_physicsComponent && "failed to get dependent component");

    struct CatchCollisionObjectCall : public DeferredCall
    {
        PhysicsComponentPtr physicsComponent;
        PhysicsConstraintsComponentPtr physicsConstraintsComponent;

        bool operator()()
        {
            if(!physicsComponent->ready())
                return false;

            physicsConstraintsComponent->m_collisionObject =
                    physicsComponent->collisionObject();

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentAttachedCallback,
                    physicsConstraintsComponent->parent(),
                    physicsConstraintsComponent));

            return true;
        }
    };

    CatchCollisionObjectCall* call = new CatchCollisionObjectCall;
    call->physicsConstraintsComponent = shared_from_this();
    call->physicsComponent = m_physicsComponent;

    PhysicsManager::get().enqueueCall(call);
}

void
PhysicsConstraintsComponent::detachedCallback()
{
    struct RemoveActiveConstraintsCall : public DeferredCall
    {
        PhysicsConstraintsComponentPtr physicsConstraintsComponent;
        bool operator()()
        {
            // if not ready, it may get ready.
            if(!physicsConstraintsComponent->ready())
                return false;

            while(physicsConstraintsComponent->m_constrainedComponents.size())
                physicsConstraintsComponent->removePhysicsConstraintInternal(
                        (*physicsConstraintsComponent->
                                m_constrainedComponents.begin()).first);

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentDetachedCallback,
                    physicsConstraintsComponent->parent(),
                    physicsConstraintsComponent));

            return true;
        }
    };

    RemoveActiveConstraintsCall* call = new RemoveActiveConstraintsCall;
    call->physicsConstraintsComponent = shared_from_this();

    PhysicsManager::get().enqueueCall(call);
}

bool
PhysicsConstraintsComponent::componentAdded(
        PhysicsConstraintsComponentPtr p_physicalConstraintComponent,
        btTypedConstraint* p_constraint)
{
    assert(m_constrainedComponents.find(p_physicalConstraintComponent) == 
            m_constrainedComponents.end() && "constraint already present.");
    
    m_constrainedComponents[p_physicalConstraintComponent] = p_constraint;

    return true;
}

bool
PhysicsConstraintsComponent::componentRemoved(
        PhysicsConstraintsComponentPtr p_physicalConstraintComponent)
{
    std::map<PhysicsConstraintsComponentPtr, btTypedConstraint*>::iterator it;

    it = m_constrainedComponents.find(p_physicalConstraintComponent);

    assert(it != m_constrainedComponents.end() && "linked component not found");

    btTypedConstraint* constraint = (*it).second;

    if(constraint)
    {
        PhysicsManager::get().world()->removeConstraint(constraint);
        delete constraint;
    }

    m_constrainedComponents.erase(it);

    return true;
};

bool
PhysicsConstraintsComponent::addPhysicsConstraint(
        PhysicsConstraintsComponentPtr p_physicalConstraintComponent,
        PhysicsConstraint p_constraintType,
        Ogre::Vector3 p_anchorPoint)
{
    assert(attached() && "not attached");
    assert(p_physicalConstraintComponent->attached() && "partner not attached");
    
    struct AddContraintCall : public DeferredCall
    {
        PhysicsConstraintsComponentPtr constraintPartner;
        PhysicsConstraintsComponentPtr constraintInitializer;
        btVector3 anchorPoint;
        PhysicsConstraint constraintType;

        bool operator()()
        {
            if(!constraintInitializer->ready() || !constraintPartner->ready())
                return false;

            btTypedConstraint* constraint;

            switch(constraintType)
            {
            default:
            case PC_Point2Point:
            {
                btTransform initializerTrans;
                btTransform partnerTrans;

                initializerTrans =
                    constraintInitializer->
                        collisionObject()->getWorldTransform();

                partnerTrans =
                    constraintPartner->
                        collisionObject()->getWorldTransform();

                anchorPoint += initializerTrans.getOrigin();

                btVector3 initializerAnchor =
                        initializerTrans.getBasis().inverse() *
                        (anchorPoint - initializerTrans.getOrigin());

                btVector3 partnerAnchor =
                        partnerTrans.getBasis().inverse() *
                        (anchorPoint - partnerTrans.getOrigin());

                constraint =
                    new btPoint2PointConstraint(
                            *constraintInitializer->collisionObject(),
                            *constraintPartner->collisionObject(),
                            initializerAnchor,
                            partnerAnchor);
            }
            break;
            }

            PhysicsManager::get().world()->addConstraint(constraint);

            constraintInitializer->componentAdded(
                    constraintPartner, constraint);
            constraintPartner->componentAdded(
                    constraintInitializer, 0);

            return true;
        }
    };

    AddContraintCall* call = new AddContraintCall;
    call->constraintPartner = p_physicalConstraintComponent;
    call->constraintInitializer = shared_from_this();
    call->constraintType = p_constraintType;
    call->anchorPoint = btVector3(
            p_anchorPoint.x,
            p_anchorPoint.y,
            p_anchorPoint.z);

    PhysicsManager::get().enqueueCall(call);

    return true;
}

bool
PhysicsConstraintsComponent::removePhysicsConstraint(
        PhysicsConstraintsComponentPtr p_physicalConstraintComponent)
{
    assert(attached() && "not attached");
    assert(p_physicalConstraintComponent->attached() && "partner not attached");


    struct RemoveConstraintCall : public DeferredCall
    {
        PhysicsConstraintsComponentPtr constraintInitializer;
        PhysicsConstraintsComponentPtr constraintPartner;

        bool operator()()
        {
            if(!constraintInitializer->ready())
                return false;

            constraintInitializer->
                    removePhysicsConstraintInternal(constraintPartner);

            return true;
        }
    };

    RemoveConstraintCall* call = new RemoveConstraintCall;
    call->constraintInitializer = shared_from_this();
    call->constraintPartner = p_physicalConstraintComponent;

    PhysicsManager::get().enqueueCall(call);
    
    return true;
}

bool
PhysicsConstraintsComponent::removePhysicsConstraintInternal(
        PhysicsConstraintsComponentPtr p_physicalConstraintComponent)
{
    assert(p_physicalConstraintComponent &&
            "physicalConstraintComponent must not be null");

    componentRemoved(p_physicalConstraintComponent);
    p_physicalConstraintComponent->componentRemoved(shared_from_this());

    return true;
}


}