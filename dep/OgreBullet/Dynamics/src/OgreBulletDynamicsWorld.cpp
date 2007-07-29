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

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsObjectState.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsConstraint.h"

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

    }
    // -------------------------------------------------------------------------
    DynamicsWorld::~DynamicsWorld()
    {
        delete mConstraintsolver;
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

        static_cast <btSimpleDynamicsWorld *> (mWorld)->stepSimulation(elapsedTime);

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

