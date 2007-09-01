/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

Copyright (c) 2007 tuan.kuranes@gmail.com



This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#include "OgreBulletDynamics.h"

#include "OgreBulletCollisionsShape.h"
#include "OgreBulletCollisionsObjectState.h"

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsObjectState.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsConstraint.h"
#include "OgreBulletCollisionInfo.h"
#include "OgreBulletCollisionHandler.h"

#include "Constraints/OgreBulletDynamicsRaycastVehicle.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

namespace OgreBulletDynamics
{

    DynamicsWorld::DynamicsWorld(Ogre::SceneManager *mgr, 
                const Ogre::AxisAlignedBox &bounds,  
                const Ogre::Vector3 &gravity) :
        CollisionsWorld(mgr, bounds, false),
            mDebugDrawer(0)
    {
        //btSequentialImpulseConstraintSolver
        //btSequentialImpulseConstraintSolver3
        mConstraintsolver = new btSequentialImpulseConstraintSolver();

        //mWorld = new btSimpleDynamicsWorld();
        mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mConstraintsolver);


        static_cast <btSimpleDynamicsWorld *> (mWorld)->setGravity(btVector3(gravity.x,gravity.y,gravity.z));

        mCollisionInfosMutex = SDL_CreateMutex();
        assert(mCollisionInfosMutex);
    }
    // -------------------------------------------------------------------------
    DynamicsWorld::~DynamicsWorld()
    {
        delete mConstraintsolver;
        SDL_DestroyMutex(mCollisionInfosMutex);
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::addRigidBody (RigidBody *rb)
    {
        mObjects.push_back (static_cast <Object *> (rb));
        static_cast <btSimpleDynamicsWorld *> (mWorld)->addRigidBody(rb->getBulletRigidBody());      
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::stepSimulation(const Ogre::Real elapsedTime)
    {
        // Reset Debug Lines
        if (mDebugDrawer) 
            mDebugDrawer->clear ();

        // step the world
        static_cast <btSimpleDynamicsWorld *> (mWorld)->stepSimulation(elapsedTime, 50);


        CollisionHandler* handlerA = 0;
        CollisionHandler* handlerB = 0;

        const unsigned int  numManifolds = mWorld->getDispatcher()->getNumManifolds();
        for (unsigned int i=0;i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);

            btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
            btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

            if(obA)
                handlerA = ((Object*)obA->getUserPointer())->getCollisionHandler();
            else
                handlerA = 0;

            if(obB)
                handlerB = ((Object*)obB->getUserPointer())->getCollisionHandler();
            else
                handlerB = 0;
            
            //if(!handlerA && !handlerB)
                //continue;
            
            contactManifold->refreshContactPoints(obA->getWorldTransform(),obB->getWorldTransform());
            
            const unsigned int numContacts = contactManifold->getNumContacts();
            std::cout << numContacts << std::endl;
            for (unsigned int j = 0;j < numContacts; j++)
            {
                btManifoldPoint& pt = contactManifold->getContactPoint(j);
                        
                if(handlerA)
                {                    
                    CollisionInfo info(
                        (Object*)obA->getUserPointer(), 
                        (Object*)obB->getUserPointer(), 
                        pt.m_localPointA, 
                        pt.getPositionWorldOnA());
                    // handlerA->handleCollision(info);

                    SDL_LockMutex(mCollisionInfosMutex);
                    mCollisionInfos.push_front(info);
                    SDL_UnlockMutex(mCollisionInfosMutex);
                }
            
                if(handlerB)
                {
                    CollisionInfo info(
                        (Object*)obB->getUserPointer(), 
                        (Object*)obA->getUserPointer(), 
                        pt.m_localPointB, 
                        pt.getPositionWorldOnB());
                    // handlerB->handleCollision(info);

                    SDL_LockMutex(mCollisionInfosMutex);
                    mCollisionInfos.push_front(info);
                    SDL_UnlockMutex(mCollisionInfosMutex);
                }
            
            }
        }


        if (mDebugDrawer) 
        {
            // draw lines that step Simulation sent.
            mDebugDrawer->draw();

            const bool drawFeaturesText = (mDebugDrawer->getDebugMode () & btIDebugDraw::DBG_DrawFeaturesText) != 0;
            if (drawFeaturesText)
            {
                // on all bodies we have
                // we get all shapes and draw more information
                //depending on mDebugDrawer mode.
                std::deque<Object*>::iterator it = mObjects.begin();
                while (it != mObjects.end())
                {
                    //(*it)->drawFeaturesText();
                    ++it;
                }
            }
        }
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::synchronizeToOgre()
    {
        OgreBulletCollisions::ObjectState::lockTransformationCache();

        // sync all object back to ogre
        std::map<Object*, btTransform>::const_iterator it = 
                OgreBulletCollisions::ObjectState::transformationCache().begin();

        while(it != OgreBulletCollisions::ObjectState::transformationCache().end())
        {
            (*it).first->setTransform((*it).second);
            it++;
        }
        
        OgreBulletCollisions::ObjectState::unlockTransformationCache();
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::publishCollisions()
    {
        SDL_LockMutex(mCollisionInfosMutex);

        // std::cout << "publishing " << mCollisionInfos.size() 
        //         << " collision events" << std::endl;

        while(mCollisionInfos.size())
        {
            if(mCollisionInfos.front().getObject()->getCollisionHandler())
                mCollisionInfos.front().getObject()->getCollisionHandler()->
                        handleCollision(mCollisionInfos.front());

            mCollisionInfos.pop_front();
        }
        SDL_UnlockMutex(mCollisionInfosMutex);
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::removeConstraint(TypedConstraint *constraint)
    {
        getBulletDynamicsWorld()->removeConstraint(constraint->getBulletTypedConstraint());
        std::deque <TypedConstraint*>::iterator it = mConstraints.begin();
        while (it != mConstraints.end())
        {
            if ((*it) == constraint)
            {
                mConstraints.erase (it);
                break;
            }
            ++it;
        }
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::addConstraint(TypedConstraint *constraint)
    {
        getBulletDynamicsWorld()->addConstraint(constraint->getBulletTypedConstraint());
        mConstraints.push_back(constraint);
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::addVehicle(RaycastVehicle *v)
    {
        getBulletDynamicsWorld()->addVehicle(v->getBulletVehicle ());
        mConstraints.push_back(static_cast <TypedConstraint *> (v));

        //mVehicles.push_back(v);
    }
    // -------------------------------------------------------------------------
    bool DynamicsWorld::isConstraintRegistered(TypedConstraint *constraint) const
    {
        std::deque <TypedConstraint*>::const_iterator it = mConstraints.begin();
        while (it != mConstraints.end())
        {
            if ((*it) == constraint)
                return true;
            ++it;
        }
        return false;
    }
}

