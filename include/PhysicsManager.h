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


#ifndef COAJNR_PHYSICSMANAGER_INCLUDED
#define COAJNR_PHYSICSMANAGER_INCLUDED

#include "Dependencies.h"

#include "CollisionHandlerComponent.h"
#include "FreeRunner.h"

namespace CoAJnR
{
    
/**
 * this class is responsable for bullet configuration and runtime
 */
class PhysicsManager : public FreeRunner, 
                       public rtils::Singleton<PhysicsManager>
{
    mutable boost::mutex m_accelerationFactorMutex;
    
    /// the default configuration for the dispatcher
    btDefaultCollisionConfiguration m_defaultCollisionConfiguration;

    /// collision dispatcher
    btCollisionDispatcher* m_dispatcher;

    /// constraint solver
    btSequentialImpulseConstraintSolver* m_solver;

    /// the dynamics world
    btDiscreteDynamicsWorld* m_world;

    /// broadphase
    btAxisSweep3* m_broadphase;

    double m_accelerationFactor;

    /// sends collision events via thread pool to all 
    // CollisionObject s with CollisionHandlerComponent objects attached
    void publishCollisions();

    /**
     * sends a frame time report to the graphics threads and writes it into the
     * debug overlay.
     */
    void reportFrameTime() const;

protected:
    /**
     * brings up the bullet subsystem.
     */
    void init();
    
    /**
     * updates the physics manager (steps the world)
     */
    bool update(double elapsed);
    
    /**
     *
     */
    void release();
    
public:
    /**
     * ctor.
     */
    PhysicsManager();
    
    /**
     * dtor.
     */
    virtual ~PhysicsManager();
    
    /**
     * returns the dynamic world
     */
    btDiscreteDynamicsWorld* world()
    {
        assert(m_world);
        return m_world;
    }
    
    void setAccelerationFactor(double factor);

    double accelerationFactor();
};

}

#endif

