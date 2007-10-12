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

#ifndef _OGREBULLETCOLLISIONS_CollisionObject_H
#define _OGREBULLETCOLLISIONS_CollisionObject_H

#include "OgreBulletCollisionsPrerequisites.h"

#include "OgreBulletCollisionsWorld.h"
#include "OgreBulletCollisionHandler.h"

namespace OgreBulletCollisions
{
    // -------------------------------------------------------------------------
    /*!
	 * \brief
	 * 
	 * Object is the Basic Bullet Collision representation of an Physical thing in
     * a scene. It does need a Shape Object to know its "Geometrics" Bounds.
	 * 
	 * \remarks
	 * Objects doesn't need to be represented by a Visible 3D mesh.
	 * 
	 * \see
	 * Ogre::MovableObject | Ogre::UserDefinedObject | OgreBulletDynamics::RigidBody
	 */
	class Object : public Ogre::MovableObject, public Ogre::UserDefinedObject
    {
    public:



        Object(const Ogre::String &name,  CollisionsWorld *world, bool init);


        virtual ~Object();

        // override Movables
        virtual const Ogre::String& getMovableType() const; 
        virtual void _notifyAttached(Ogre::Node* parent,bool isTagPoint = false);
        //virtual const Ogre::String& getName(void) const {return mName};
        virtual void _notifyCurrentCamera(Ogre::Camera* camera);
        virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const;
        virtual Ogre::Real getBoundingRadius(void) const;
        virtual void _updateRenderQueue(Ogre::RenderQueue* queue);


        inline const Ogre::Vector3 &getWorldPosition() const {return mRootNode->_getDerivedPosition();};
        inline const Ogre::Quaternion &getWorldOrientation() const {return mRootNode->_getDerivedOrientation();};

        inline void setPosition(const Ogre::Vector3 &p) { if(mRootNode) mRootNode->setPosition (p); };
        inline void setOrientation(const Ogre::Quaternion &q)  { if(mRootNode) mRootNode->setOrientation (q); };

        inline void setPosition(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z) { if(mRootNode) mRootNode->setPosition (x,y,z); };
        inline void setOrientation(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z, const Ogre::Real w)  { if(mRootNode) mRootNode->setOrientation (x,y,z,w); };

        virtual void setPosition(const btVector3 &pos);
        virtual void setOrientation(const btQuaternion &quat);
        virtual void setTransform(const btVector3 &pos, const btQuaternion &quat);
        virtual void setTransform(const btTransform& worldTrans);

        inline btCollisionObject*  getBulletObject() const { return mObject;};
        inline btCollisionWorld*  getBulletCollisionWorld() const { return mWorld->getBulletCollisionWorld ();};
        inline CollisionsWorld*  getCollisionWorld() const { return mWorld;};
        
        inline CollisionShape *getShape() const{ return mShape;};
        inline DebugCollisionShape* getDebugShape() const{ return mDebugShape;};

        inline void setCollisionHandler(CollisionHandler* handler) { mCollisionHandler = handler; }
        inline CollisionHandler* getCollisionHandler() { return mCollisionHandler; }

        void setShape(CollisionShape *shape, 
            const Ogre::Vector3 &pos, 
            const Ogre::Quaternion &quat);
        void showDebugShape(bool show);

        void setSyncRotations(bool value){ mSyncRotations = value; }
        bool getSyncRotations() { return mSyncRotations; }
        
        Ogre::SceneNode* sceneNode() { return mRootNode; }

    protected:
        
        bool                    mSyncRotations;

        Ogre::SceneNode*        mRootNode;
        Ogre::SceneNode*        mShapeNode;
        Ogre::SceneNode*        mDebugNode;

        ObjectState   *         mState;
        CollisionsWorld*        mWorld;

        btCollisionObject*      mObject;

        Ogre::AxisAlignedBox    mBounds;

        CollisionShape*         mShape;
        DebugCollisionShape *   mDebugShape;
        
        CollisionHandler*       mCollisionHandler;


    public:
        static const Ogre::String mMovableType;
    };
    // -------------------------------------------------------------------------
}
#endif //_OGREBULLETCOLLISIONS_CollisionObject_H

