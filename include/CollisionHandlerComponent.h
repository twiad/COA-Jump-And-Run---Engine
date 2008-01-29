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


#ifndef COAJNR_COLLISIONHANDLER_INCLUDED
#define COAJNR_COLLISIONHANDLER_INCLUDED

#include "Dependencies.h"

#include "Component.h"

namespace CoAJnR
{

class PhysicsComponent;

/**
 * base for collision reactions.
 *
 * ment to get attached to a physical object.
 */
class CollisionHandlerComponent : 
        public Component, 
        public boost::enable_shared_from_this<CollisionHandlerComponent>

{
    virtual void attachedCallback();
    virtual void detachedCallback();

    boost::shared_ptr<PhysicsComponent> m_physicsComponent;

    mutable boost::mutex m_mutex;

public:
    CollisionHandlerComponent();

    virtual ~CollisionHandlerComponent();
    
    /**
     * will be called on collision event.
     *
     * @param collisionPartner the collided object
     * @param localPointOfCollision point of collision relative to object
     * @param globalPointOfCollision point of collision in world coords
     *
     * @note this method will be called from the thread pool. you must only use
     * thread safe apis from here.
     */
    virtual void handleCollision(
            boost::shared_ptr<PhysicsComponent> collisionPartner,
            Ogre::Vector3 localPointOfCollision,
            Ogre::Vector3 globalPointOfCollision) = 0;

    void handleCollisionInternal(
            boost::shared_ptr<PhysicsComponent> collisionPartner,
            Ogre::Vector3 localPointOfCollision,
            Ogre::Vector3 globalPointOfCollision);


};

typedef boost::shared_ptr<CollisionHandlerComponent> CollisionHandlerComponentPtr;

}

#endif

