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


#ifndef COAJNR_PHYSICALCONSTRAINSCOMPONENT_COMPONENT_INCLUDED
#define COAJNR_PHYSICALCONSTRAINSCOMPONENT_COMPONENT_INCLUDED

#include "Component.h"
#include "CollisionObject.h"

namespace CoAJnR
{

/**
 * describes how PhysicsConstraintsComponent instances are connected.
 */
enum PhysicsConstraint
{
    PC_Point2Point = 0
};

/**
 * enables objects to get physically attached.
 */
class PhysicsConstraintsComponent : 
        public Component,
        public boost::enable_shared_from_this<PhysicsConstraintsComponent>
{
    /// the PhysicsComponent instance this component depends on.
    PhysicsComponentPtr m_physicsComponent;

    /// the CollisionObject of the PhysicsComponent, needed to attach constraint
    CollisionObject* m_collisionObject;

    /// list, mapping from constraint partner Component to constraint instance
    std::map<boost::shared_ptr<PhysicsConstraintsComponent>, btTypedConstraint*>
            m_constrainedComponents;
    
    /**
     * return a list filled with names of components we need.
     *
     * @note this method is thread safe and can get called from any thread.
     */
    std::vector<std::string> dependencies();

    /**
     * callback for being attached to a ComponentContainer.
     *
     * this queues a call in PhysicsManager, which then writes back the 
     * CollisionObject instance pointer into m_collisionObject. When this is
     * done, the component is marked as ready()
     *
     * @note this method is thread safe and can get called from any thread.
     */
    void attachedCallback();

    /**
     * callback for being detached from a ComponentContainer.
     *
     * this queues a call in PhysicsManager, which first waits until all pending
     * operations like the attached() callback or addPhysicsConstraint() are
     * done. once finished, the component will set to not ready and all 
     * available constrains will be destroyed 
     * via removePhysicsConstraintInternal(). after this last action, the 
     * component can be deleted. 
     *
     * @note this method is thread safe and can get called from any thread.
     */
    void detachedCallback();
    
    /**
     * updates the local list of attached constraints.
     *
     * @note callback method only used by PhysicsManager, not thread safe.
     */
    bool componentAdded(
            boost::shared_ptr<PhysicsConstraintsComponent>
                    physicsConstrainsComponent, 
            btTypedConstraint* constraint);

    /**
     * updates the local list of attached constraints.
     *
     * @note callback method only used by PhysicsManager, not thread safe.
     */
    bool componentRemoved(
            boost::shared_ptr<PhysicsConstraintsComponent>
                    physicsConstrainsComponent);

    /**
     * this removes the constraint from the physics world
     * 
     * deletes the instance. afterwards, componentRemoved() will be called on 
     * this and partner Component.
     *
     * @note method only used by PhysicsManager, not thread safe.
     */
    bool removePhysicsConstraintInternal(
            boost::shared_ptr<PhysicsConstraintsComponent> 
                    physicsConstrainsComponent);


public:
    /**
     * ctor.
     */
    PhysicsConstraintsComponent();
    
    /**
     * dtor.
     */
    ~PhysicsConstraintsComponent();

    virtual std::string defaultName() const;

    /**
     * adds a new constraint between this and the given constraintPartner.
     *
     * this queues a call in PhysicsManager which creates the constraint as
     * soon as both PhysicsConstraintsComponent are ready().
     *
     * @param constraintPartner the other PhysicsConstraintsComponent.
     * @param constraintType the PhysicsConstraint type.
     * @param the constraint anchor position relative to this.
     *
     * @note this method is thread safe and can get called from any thread.
     */
    bool addPhysicsConstraint(
            boost::shared_ptr<PhysicsConstraintsComponent> constraintPartner, 
            PhysicsConstraint constraintType,
            Ogre::Vector3 anchorPoint = Ogre::Vector3::ZERO);

    /**
     * removes a constraint to a certain partner PhysicsConstraintsComponent.
     *
     * this queues a call in PhysicsManager which first waits for all pending
     * operations and the ready() flag. then removePhysicsConstraintInternal()
     * will be called.
     *
     * @note this method is thread safe and can get called from any thread.
     */
    bool removePhysicsConstraint(
            boost::shared_ptr<PhysicsConstraintsComponent> constraintPartner);
    
    /// @todo TODO: world constraints
    // bool addPhysicsConstraint(Ogre::Vector3);
    // bool removePhysicsConstraint(Ogre::Vector3);

    /**
     * returns the bullet CollisionObject from the PhysicsComponent 
     * this depends on
     */
    CollisionObject* collisionObject() 
    {
        return m_collisionObject;
    };
};

typedef boost::shared_ptr<PhysicsConstraintsComponent> 
        PhysicsConstraintsComponentPtr;

}

#endif
