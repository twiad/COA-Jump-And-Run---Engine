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

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsConstraint.h"
#include "Constraints/OgreBulletDynamicsRaycastVehicle.h"

using namespace Ogre;

namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    VehicleRayCaster::VehicleRayCaster(DynamicsWorld *world)
    {
        mBulletVehicleRayCaster = new btDefaultVehicleRaycaster(world->getBulletDynamicsWorld ());
    }
    // -------------------------------------------------------------------------
    VehicleRayCaster::~VehicleRayCaster()
    {
        delete mBulletVehicleRayCaster;
    }   
    // -------------------------------------------------------------------------
    VehicleTuning::VehicleTuning(const Ogre::Real suspensionStiffness,
				const Ogre::Real suspensionCompression,
				const Ogre::Real suspensionDamping,
				const Ogre::Real maxSuspensionTravelCm,
				const Ogre::Real frictionSlip)
    {
        mBulletTuning = new btRaycastVehicle::btVehicleTuning();

        mBulletTuning->m_suspensionStiffness =  suspensionStiffness;
        mBulletTuning->m_suspensionCompression = suspensionCompression;
        mBulletTuning->m_suspensionDamping = suspensionDamping;
        mBulletTuning->m_maxSuspensionTravelCm = maxSuspensionTravelCm;
        mBulletTuning->m_frictionSlip = frictionSlip;
    }
    // -------------------------------------------------------------------------
    VehicleTuning::~VehicleTuning()
    {
    }
    // -------------------------------------------------------------------------
    RaycastVehicle::RaycastVehicle(WheeledRigidBody *body,  
                                VehicleTuning *vt,
                                VehicleRayCaster *caster) :
        TypedConstraint(body),
        mTuning(vt),
        mRayCaster(caster)
    {
        if (mRayCaster == 0)
            mRayCaster = new VehicleRayCaster(mWorld);

        btRaycastVehicle *v = new btRaycastVehicle(
                *(mTuning->getBulletTuning()),
                body->getBulletRigidBody (), 
                mRayCaster->getBulletVehicleRayCaster()
                );
        mConstraint = v;
        mWorld->addVehicle(this);
        body->setVehicle (this);
    }
    // -------------------------------------------------------------------------
    RaycastVehicle::~RaycastVehicle()
    {
    }
    // -------------------------------------------------------------------------
    void RaycastVehicle::setCoordinateSystem(int rightIndex,int upIndex,int forwardIndex)
    {
        getBulletVehicle()->setCoordinateSystem(rightIndex, upIndex, forwardIndex);
    }
    // -------------------------------------------------------------------------
    void RaycastVehicle::addWheel(SceneNode *node,
                                  const Ogre::Vector3 &connectionPoint,
                                  const Ogre::Vector3 &wheelDirection,
                                  const Ogre::Vector3 &wheelAxle,

                                  const Ogre::Real suspensionRestLength,
                                  const Ogre::Real wheelRadius,
                                  const bool isFrontWheel,
                                  
                                  const Ogre::Real wheelFriction,
                                  const Ogre::Real rollInfluence)
    {
        btRaycastVehicle *v = static_cast<btRaycastVehicle *> (mConstraint);

        mWheelsInfo.push_back (
            &v->addWheel(OgreBulletCollisions::OgreBtConverter::to(connectionPoint),
                        OgreBulletCollisions::OgreBtConverter::to(wheelDirection),
                        OgreBulletCollisions::OgreBtConverter::to(wheelAxle),
                        suspensionRestLength,
                        wheelRadius, 
                        *(mTuning->getBulletTuning ()),
                        isFrontWheel));

        {
            const size_t wheelCurrent = mWheelsInfo.size() - 1;
            mWheelsInfo[wheelCurrent]->m_suspensionStiffness = mTuning->getBulletTuning()->m_suspensionStiffness;
            mWheelsInfo[wheelCurrent]->m_wheelsDampingRelaxation = mTuning->getBulletTuning()->m_suspensionDamping;
            mWheelsInfo[wheelCurrent]->m_wheelsDampingCompression = mTuning->getBulletTuning()->m_suspensionCompression;

            mWheelsInfo[wheelCurrent]->m_frictionSlip = wheelFriction;
            mWheelsInfo[wheelCurrent]->m_rollInfluence = rollInfluence;
        }

        // create wheel scene Node
        {
            node->setPosition (connectionPoint);

            mWheelNodes.push_back ( node);
            const size_t wheelCurrent = mWheelsInfo.size() - 1;

            //mWheelsInfo[wheelCurrent]->
        }
    }
    // -------------------------------------------------------------------------
    void RaycastVehicle::setWheelsAttached()
    {
        btRaycastVehicle *v = static_cast<btRaycastVehicle *> (mConstraint);
        for (int i=0; i < v->getNumWheels(); i++)
        {
            btWheelInfo& wheel = v->getWheelInfo(i);

            wheel.m_suspensionStiffness = mTuning->getBulletTuning()->m_suspensionStiffness;
            wheel.m_wheelsDampingRelaxation = mTuning->getBulletTuning()->m_suspensionDamping;
            wheel.m_wheelsDampingCompression = mTuning->getBulletTuning()->m_suspensionCompression;

            //wheel.m_frictionSlip = mBulletTuning->getBulletTuning()->m_wheelFriction;
            //wheel.m_rollInfluence = mBulletTuning->getBulletTuning()->m_rollInfluence;
        }
    }
    // -------------------------------------------------------------------------
    void RaycastVehicle::setTransform()
    {
        //should update wheels as well ?
        for (int i=0; i < getBulletVehicle()->getNumWheels(); i++)
        {
            //synchronize the wheels with the (interpolated) chassis world transform
            getBulletVehicle()->updateWheelTransform(i, true);
            //draw wheels (cylinders)
            const btTransform &w = getBulletVehicle()->getWheelInfo(i).m_worldTransform;

            mWheelNodes[i]->setPosition (w.getOrigin()[0], w.getOrigin()[1], w.getOrigin()[2]);
            mWheelNodes[i]->setOrientation (w.getRotation().getW(), w.getRotation().getX(), w.getRotation().getY(), w.getRotation().getZ());

        }
    }
    // -------------------------------------------------------------------------
    void RaycastVehicle::applyEngineForce (float engineForce, int wheel)
    {
        getBulletVehicle()->applyEngineForce (engineForce, wheel);
    }
    // -------------------------------------------------------------------------
    void RaycastVehicle::setSteeringValue(float steering, int wheel)
    {
        getBulletVehicle()->setSteeringValue (steering, wheel);
    }
    // -------------------------------------------------------------------------
}