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

#ifndef _OGREBULLETDYNAMICS_DynamicWorld_H
#define _OGREBULLETDYNAMICS_DynamicWorld_H

#include "OgreBulletDynamicsPrerequisites.h"

#include "OgreBulletCollisionsWorld.h"
#include "Debug/OgreBulletCollisionsDebugDrawer.h"


namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    // basic rigid body class
    class DynamicsWorld : public OgreBulletCollisions::CollisionsWorld
    {
    public:
        DynamicsWorld(Ogre::SceneManager *mgr, 
            const Ogre::AxisAlignedBox &bounds,  
            const Ogre::Vector3 &gravity);

	    ~DynamicsWorld();

        void stepSimulation(const Ogre::Real elapsedTime);

        void addRigidBody (RigidBody *rb);

        void setDebugDrawer(OgreBulletCollisions::DebugDrawer *debugdrawer)
        {
            mDebugDrawer = debugdrawer;
            (static_cast <btDiscreteDynamicsWorld *> (mWorld))->setDebugDrawer(mDebugDrawer);
        };

        OgreBulletCollisions::DebugDrawer *getDebugDrawer(){return mDebugDrawer;};

        inline btDynamicsWorld * getBulletDynamicsWorld() const {return static_cast<btDynamicsWorld *> (mWorld);};

        void removeConstraint(TypedConstraint *constraint);
        void addConstraint(TypedConstraint *constraint);
        bool isConstraintRegistered(TypedConstraint *constraint) const;

        void addVehicle(RaycastVehicle *v);

    private:
        OgreBulletCollisions::DebugDrawer                 *mDebugDrawer;

        btConstraintSolver                                *mConstraintsolver;

        std::deque <TypedConstraint *>                    mConstraints;
    };
}
#endif //_OGREBULLETDYNAMICS_DynamicWorld_H

