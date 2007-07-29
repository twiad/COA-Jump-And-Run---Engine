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
#include "OgreBulletCollisionsObject.h"
#include "OgreBulletCollisionsWorld.h"
#include "OgreBulletCollisionsObjectState.h"

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsObjectState.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

namespace OgreBulletDynamics
{

    // -------------------------------------------------------------------------
    RigidBody::RigidBody(const String &name, DynamicsWorld *world)
        :	
        Object(name, world, false)
    {
    }
    // -------------------------------------------------------------------------
    RigidBody::~RigidBody()
    {
    }  
    // -------------------------------------------------------------------------
    void RigidBody::setShape(Ogre::SceneNode *node,  
        OgreBulletCollisions::CollisionShape *shape,                          
        const float      bodyRestitution,
        const float      bodyFriction,
        const float      bodyMass,
        const Vector3 &pos, 
        const Quaternion &quat)
    {

        mState = new ObjectState(this);

        mRootNode = node;
        mShapeNode = mRootNode->createChildSceneNode(mName + "Node");
        mShapeNode->attachObject(this);

        node->setPosition (pos);
        node->setOrientation (quat);

        mShape = shape;
        showDebugShape(mWorld->getShowDebugShapes());

        btVector3 localInertiaTensor = btVector3(0,0,0);
        mShape->getBulletShape ()->calculateLocalInertia(bodyMass, localInertiaTensor);


        btRigidBody *body = new btRigidBody(bodyMass, mState, mShape->getBulletShape (), localInertiaTensor);
        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        mObject = body;
        getDynamicsWorld()->addRigidBody(this);
    }

    // -------------------------------------------------------------------------
    void RigidBody::setStaticShape(Ogre::SceneNode *node, 
        OgreBulletCollisions::CollisionShape *shape,
        const float      bodyRestitution,
        const float      bodyFriction,
        const Vector3 &pos, 
        const Quaternion &quat)
    {

        mRootNode = node;

        mShapeNode = mRootNode->createChildSceneNode(mName + "Node");
        mShapeNode->attachObject(this);

        node->setPosition (pos);
        node->setOrientation (quat);

        mShape = shape;
        showDebugShape(mWorld->getShowDebugShapes());

        btRigidBody *body = new btRigidBody(0.0, mState, mShape->getBulletShape ());

        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        body->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
        body->getWorldTransform().setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));

        mObject = body;
        getDynamicsWorld()->addRigidBody(this);
    }
    // -------------------------------------------------------------------------
    void RigidBody::setStaticShape(OgreBulletCollisions::CollisionShape *shape,
        const float      bodyRestitution,
        const float      bodyFriction,
        const Vector3 &pos, 
        const Quaternion &quat)
    {

        mShape = shape;
        btRigidBody *body = new btRigidBody(0.0, mState, mShape->getBulletShape ());

        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        body->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
        body->getWorldTransform().setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));

        mObject = body;
        getDynamicsWorld()->addRigidBody(this);
    }
    // -------------------------------------------------------------------------
    void RigidBody::setLinearVelocity( const Ogre::Vector3 &vel )
    {
        getBulletRigidBody()->setLinearVelocity (btVector3(vel.x, vel.y, vel.z));
    }
    // -------------------------------------------------------------------------
    void RigidBody::setLinearVelocity( const Ogre::Real x, const Ogre::Real y, const Ogre::Real z )
    {
        getBulletRigidBody()->setLinearVelocity (btVector3(x, y, z));
    }
    // -------------------------------------------------------------------------
    void RigidBody::applyImpulse( const Ogre::Vector3 &impulse, const Ogre::Vector3 &position )
    {
        getBulletRigidBody()->applyImpulse (OgreBtConverter::to(impulse), OgreBtConverter::to(position));
    }
    // -------------------------------------------------------------------------
    Ogre::Vector3 RigidBody::getCenterOfMassPivot( const Ogre::Vector3 &pivotPosition ) const
    {
        const btVector3 centerOfMassPivot(getCenterOfMassTransform().inverse()* OgreBtConverter::to(pivotPosition));
        return BtOgreConverter::to(centerOfMassPivot);
    }
    // -------------------------------------------------------------------------
    void RigidBody::setDeactivationTime( const float ftime )
    {
        getBulletRigidBody()->setDeactivationTime( ftime );
    }
    // -------------------------------------------------------------------------
    void RigidBody::setDamping( const Ogre::Real linearDamping, const Ogre::Real angularDamping )
    {
        getBulletRigidBody()->setDamping( linearDamping,  angularDamping);
    }
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setPosition(const btVector3 &pos)
    { 
        //should update wheels as well ?
        mRootNode->setPosition(pos[0], pos[1], pos[2]);
    };
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setOrientation(const btQuaternion &quat)
    { 
        mRootNode->setOrientation(quat.getW(),quat.getX(), quat.getY(), quat.getZ());
    };
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setTransform(const btVector3 &pos, const btQuaternion &quat)
    {
        mRootNode->setPosition(pos[0], pos[1], pos[2]);
        mRootNode->setOrientation(quat.getW(),quat.getX(), quat.getY(), quat.getZ());

        mVehicle->setTransform();
    }
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setTransform(const btTransform& worldTrans)
    { 
        mRootNode->setPosition(worldTrans.getOrigin()[0], worldTrans.getOrigin()[1],worldTrans.getOrigin()[2]);
        mRootNode->setOrientation(worldTrans.getRotation().getW(),worldTrans.getRotation().getX(), worldTrans.getRotation().getY(), worldTrans.getRotation().getZ());

        mVehicle->setTransform();
    }
}

