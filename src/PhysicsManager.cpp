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


#include "PhysicsManager.h"

#include "CollisionObject.h"
#include "DebugOverlayFrameListener.h"
#include "GraphicsManager.h"
#include "UserThreadPool.h"

namespace CoAJnR
{

PhysicsManager::PhysicsManager() :
    m_accelerationFactor(COAJNR_PHYSICS_ACCELERATION_FACTOR)
{
    m_dispatcher = 0;
    m_broadphase = 0;
    m_solver = 0;
    m_world = 0;
}

PhysicsManager::~PhysicsManager()
{
}

void 
PhysicsManager::init()
{
    IdManager::get().setThreadName("Physics Thread");

    m_dispatcher = new btCollisionDispatcher(&m_defaultCollisionConfiguration);

#ifdef COAJNR_LOADS_OF_OBJECTS
    m_broadphase = new bt32BitAxisSweep3(
            btVector3(-1000, -1000, -1000), 
            btVector3(1000, 1000, 1000));
#else
    m_broadphase = new btAxisSweep3(
            btVector3(-1000, -1000, -1000), 
            btVector3(1000, 1000, 1000));
#endif

    m_solver = new btSequentialImpulseConstraintSolver();
    m_solver->setSolverMode(
            btSequentialImpulseConstraintSolver::SOLVER_RANDMIZE_ORDER);

    m_world = new btDiscreteDynamicsWorld(
            m_dispatcher, m_broadphase, m_solver, 
            &m_defaultCollisionConfiguration);

    m_world->setGravity(btVector3(0.0, -9.81, 0.0));
    
    setMaxFrameTime(COAJNR_PHYSICS_FRAME_TIME);
}

void 
PhysicsManager::reportFrameTime() const
{
    struct TellDebugOverlayAboutPhysicsFpsCall : public DeferredCall
    {
        double lastFPS;
        double worstFPS;
        bool operator()()
        {
            GraphicsManager::get().debugOverlay()->setPhysicsFPS(lastFPS);
            GraphicsManager::get().debugOverlay()->setWorstPhysicsFPS(worstFPS);
        
            return true;
        }
    };
    
    TellDebugOverlayAboutPhysicsFpsCall* call = 
            new TellDebugOverlayAboutPhysicsFpsCall();
    
    call->lastFPS = 1.0f / lastFrameTime();    
    call->worstFPS = 1.0f / worstFrameTime();    
    
    GraphicsManager::get().enqueueCallDroppable(call);    
}


bool 
PhysicsManager::update(double p_elapsed)
{
    {
        boost::mutex::scoped_lock lock(m_accelerationFactorMutex);

        /// @todo TODO: find a good way to handle substepping nicely
        /// find a way which does not slow down more and more on slow machines
        m_world->stepSimulation(p_elapsed * m_accelerationFactor, 1);
    }
    
    publishCollisions();
    reportFrameTime();
    
    return true;
}

void
PhysicsManager::publishCollisions()
{
    CollisionObject* objectOne;
    CollisionObject* objectTwo;
    CollisionObject::CollisionHandlerComponentCItrType it;

    const unsigned int numManifolds = 
            m_world->getDispatcher()->getNumManifolds();

    for(unsigned int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold = 
                m_world->getDispatcher()->getManifoldByIndexInternal(i);

        objectOne = static_cast<CollisionObject*>(
                contactManifold->getBody0());
        objectTwo = static_cast<CollisionObject*>(
                contactManifold->getBody1());
    
        if(!objectOne->collisionHandlers().size() && 
                !objectTwo->collisionHandlers().size())
            continue;

        contactManifold->refreshContactPoints(
                objectOne->getWorldTransform(), objectTwo->getWorldTransform());

        const unsigned int numContacts = contactManifold->getNumContacts();

        for(unsigned int j = 0; j < numContacts; j++)
        {
            const btManifoldPoint& pt = contactManifold->getContactPoint(j);

            it = objectOne->collisionHandlers().begin();
            for(; it != objectOne->collisionHandlers().end(); it++)
                UserThreadPool::get().schedule(
                        boost::bind(
                                &CollisionHandlerComponent::
                                        handleCollisionInternal, 
                                *it, 
                                objectTwo->component(),
                                Ogre::Vector3(
                                        pt.m_localPointA.x(),
                                        pt.m_localPointA.y(),
                                        pt.m_localPointA.z()),
                                Ogre::Vector3(
                                        pt.getPositionWorldOnA().x(),
                                        pt.getPositionWorldOnA().y(),
                                        pt.getPositionWorldOnA().z())));

            it = objectTwo->collisionHandlers().begin();
            for(; it != objectTwo->collisionHandlers().end(); it++)
                UserThreadPool::get().schedule(
                        boost::bind(
                                &CollisionHandlerComponent::
                                        handleCollisionInternal, 
                                *it, 
                                objectOne->component(),
                                Ogre::Vector3(
                                        pt.m_localPointB.x(),
                                        pt.m_localPointB.y(),
                                        pt.m_localPointB.z()),
                                Ogre::Vector3(
                                        pt.getPositionWorldOnB().x(),
                                        pt.getPositionWorldOnB().y(),
                                        pt.getPositionWorldOnB().z())));
        }
    }
}
    
void
PhysicsManager::release()
{
    delete m_world;
    delete m_solver;
    delete m_broadphase;
    delete m_dispatcher;
}

void 
PhysicsManager::setAccelerationFactor(double p_factor)
{
    boost::mutex::scoped_lock lock(m_accelerationFactorMutex);
    m_accelerationFactor = p_factor;
}

double 
PhysicsManager::accelerationFactor()
{
    boost::mutex::scoped_lock lock(m_accelerationFactorMutex);
    return m_accelerationFactor;
}

}

