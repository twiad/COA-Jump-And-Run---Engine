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


#ifndef COAJNR_PHYSICSDYNAMICCOMPONENT_INCLUDED
#define COAJNR_PHYSICSDYNAMICCOMPONENT_INCLUDED

#include "Dependencies.h"

#include "Component.h"
#include "CollisionHandlerComponent.h"
#include "PositionComponent.h"
#include "GraphicsMeshComponentInterface.h"

namespace CoAJnR
{

class CollisionObject;

enum PhysicsShape
{
    PS_Convex = 0,
    PS_Box,
    PS_Sphere,
    PS_Trimesh
};

/**
 * represents static and dynamic physical collision objects.
 */
class PhysicsComponent :
        public Component,
        public boost::enable_shared_from_this<PhysicsComponent>
{
    friend class PhysicsManager;

    Ogre::Vector3 m_manualBounds;

    PhysicsShape m_shape;

    CollisionObject* m_collisionObject;

    PositionComponentPtr m_positionComponent;

    GraphicsMeshComponentInterfacePtr m_graphicsMeshComponent;
    
    mutable boost::mutex m_massMutex;
    
    double m_mass;
    
    bool m_dynamic;

protected:
    virtual std::vector<std::string> dependencies();
    virtual void attachedCallback();
    virtual void detachedCallback();

public:

    PhysicsComponent(
            PhysicsShape shape, 
            double mass = 0.0f);

    PhysicsComponent(
            Ogre::Vector3 manualBounds,
            PhysicsShape shape,
            double mass = 0.0f);

    virtual ~PhysicsComponent();

    GraphicsMeshComponentInterfacePtr graphicsMeshComponent() const
    {
        // assert(m_graphicsMeshComponent.get() && 
        //         "graphicsMeshComponent must not be null");
        return m_graphicsMeshComponent;
    }

    double mass() const
    {
        boost::mutex::scoped_lock lock(m_massMutex);
        
        return m_mass;
    }

    /**
     * @note does not change after creation, so no lock needed.
     */
    PhysicsShape physicsShape() const
    {
        return m_shape;
    }

    CollisionObject* collisionObject() const
    {
        return m_collisionObject;
    }

    void attachCollisionHandlerComponent(
            CollisionHandlerComponentPtr collisionHandler);

    void detachCollisionHandlerComponent(
            CollisionHandlerComponentPtr collisionHandler);

    /// @todo TODO: EVIIIIIILL !!!!! make it available through pos componont
    void setPosition(Ogre::Vector3 position);
    void setOrientation(Ogre::Quaternion orientation);
    
    void applyForce(
            Ogre::Vector3 force, 
            Ogre::Vector3 relativePosition = Ogre::Vector3::ZERO);
    
    void applyTorque(Ogre::Vector3 torque);
    
    void applyImpulse(
            Ogre::Vector3 impulse, 
            Ogre::Vector3 relativePosition = Ogre::Vector3::ZERO);
    
    void applyTorqueImpulse(Ogre::Vector3 torqueImpulse);

    void setLinearVelocity(Ogre::Vector3 velocity = Ogre::Vector3::ZERO);
    
    void setAngularVelocity(Ogre::Vector3 velocity = Ogre::Vector3::ZERO);

    /**
     * after setting this, 
     * the object updates its position from State every frame.
     * must be set before calling setPosition()
     * 
     */
    void setKinematic(bool kinematic = true);

    virtual std::string defaultName() const;

    /**
     * the motion state of the object, this is the point where bullet stores
     * its motion updates.
     */
    class State : public btMotionState
    {
        /// the current transformation of this object
        btTransform m_transform;

        /// the object this state belongs to
        boost::shared_ptr<PhysicsComponent> m_owner;
        
        PositionComponentPtr m_ownerPosition;

    public:
        /**
         * ctor.
         */
        State(  boost::shared_ptr<PhysicsComponent> owner,
                PositionComponentPtr ownerPosition,
                btTransform startTrans = btTransform::getIdentity());

        /**
         * dtor.
         */
        virtual ~State();

        /**
         * returns the transformation of the object this state belongs to.
         */
        virtual void getWorldTransform(btTransform& transform) const;

        /**
         * sets the transformation of the object this state belongs to.
         */
        virtual void setWorldTransform(const btTransform& transform);	

        void sync();
    };

};

typedef boost::shared_ptr<PhysicsComponent> PhysicsComponentPtr;

}

#endif

