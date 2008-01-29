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


#include "CollisionObject.h"
#include "PhysicsComponent.h"

namespace CoAJnR
{

CollisionObject::CollisionObject(PhysicsComponentPtr p_component, 
                                 btScalar p_mass, 
                                 btMotionState* p_motionState, 
                                 btCollisionShape* p_collisionShape, 
                                 const btVector3& p_localInertia) 
    : btRigidBody(p_mass, p_motionState, p_collisionShape, p_localInertia)
{
    m_component = p_component;
}

CollisionObject::~CollisionObject()
{
    m_handlers.clear();
}

void 
CollisionObject::attachCollisionHandlerComponent(
        CollisionHandlerComponentPtr p_handler)
{
    assert(p_handler && "handler must not be null");
    m_handlers.push_back(p_handler);
}

bool 
CollisionObject::detachCollisionHandlerComponent(
        CollisionHandlerComponentPtr p_handler)
{
    assert(p_handler && "handler must not be null");
    CollisionHandlerComponentItrType it;
    for(it = m_handlers.begin(); it != m_handlers.end(); it++)
        if(*it == p_handler)
        {
            m_handlers.erase(it);
            return true;
        }
    
    return false;
}

}
