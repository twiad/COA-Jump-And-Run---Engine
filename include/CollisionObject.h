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


#ifndef COAJNR_COLLISIONOBJECT_INCLUDED
#define COAJNR_COLLISIONOBJECT_INCLUDED

#include "Dependencies.h"

#include "CollisionHandlerComponent.h"
#include "PhysicsComponent.h"

namespace CoAJnR
{

/**
 * extends btRigidBody to implement CollisionHandlerComponent objects.
 */
class CollisionObject : public btRigidBody
{
    friend class PhysicsManager;

    /// CollisionHandlerComponent list data type
    typedef std::vector<CollisionHandlerComponentPtr> 
            CollisionHandlerComponentListType;
    
    /// CollisionHandlerComponent list iterator type
    typedef CollisionHandlerComponentListType::iterator     
            CollisionHandlerComponentItrType;

    /// CollisionHandlerComponent list const iterator type
    typedef CollisionHandlerComponentListType::const_iterator
            CollisionHandlerComponentCItrType;

    /// list of CollisionHandlerComponent objects.
    CollisionHandlerComponentListType m_handlers;
    
    /// the PhysicsDynamicComponent this object is handling collisions for.
    PhysicsComponentPtr m_component;
    
    /**
     * return a reference to the list of CollisionHandlerComponent objects.
     */
    const CollisionHandlerComponentListType& collisionHandlers() const
    {
        return m_handlers;
    }

public:
    /**
     * ctor
     */
    CollisionObject(
            PhysicsComponentPtr component,
            btScalar mass,
            btMotionState* motionState,
            btCollisionShape* collisionShape,
            const btVector3& localInertia = btVector3(0, 0, 0));

    /**
     * dtor.
     */
    virtual ~CollisionObject();
    
    /**
     * adds a CollisionHandlerComponent to the list of active handlers.
     *
     * @note this method is not thread safe and must only be called in 
     * physics thread, use PhysicsDynamicComponent::attachCollisionHandlerComponent() if
     * you need a thread safe interface.
     */
    void attachCollisionHandlerComponent(CollisionHandlerComponentPtr handler);
    
    /**
     * removes a CollisionHandlerComponent from the list of active handlers.
     *
     * @note this method is not thread safe and must only be called in 
     * physics thread, use PhysicsDynamicComponent::detachCollisionHandlerComponent() if
     * you need a thread safe interface.
     */
    bool detachCollisionHandlerComponent(CollisionHandlerComponentPtr handler);

    /**
     * returns the associated PhysicsDynamicComponent
     */
    PhysicsComponentPtr component() const
    {
        return m_component;
    }
};

}

#endif
