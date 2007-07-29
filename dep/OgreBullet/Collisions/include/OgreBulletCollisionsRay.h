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

#ifndef _OGREBULLETCOLLISIONS_CollisionRay_H
#define _OGREBULLETCOLLISIONS_CollisionRay_H

#include "OgreBulletCollisionsPrerequisites.h"


namespace OgreBulletCollisions
{
    // -------------------------------------------------------------------------
    // basic CollisionRay
    class CollisionRayResultCallback 
    {
    public:
        CollisionRayResultCallback(const Ogre::Ray &ray, CollisionsWorld *world, bool init = true);
	    virtual ~CollisionRayResultCallback();

        btCollisionWorld::RayResultCallback *getBulletRay() const {return mRayResultCallback;}

        bool doesCollide () const;

        inline const Ogre::Ray &getRay() const;

    protected:

        btCollisionWorld::RayResultCallback   *mRayResultCallback;
        CollisionsWorld                       *mWorld;
        Ogre::Ray                              mRay;

    };
    // -------------------------------------------------------------------------
    // basic CollisionRay inline methods
    inline const Ogre::Ray &CollisionRayResultCallback ::getRay() const 
    {
        return mRay;
    }
    // -------------------------------------------------------------------------
    //  CollisionClosestRay
    class CollisionClosestRayResultCallback : public CollisionRayResultCallback
    {
    public:
        CollisionClosestRayResultCallback(const Ogre::Ray &ray, CollisionsWorld *world);
        virtual ~CollisionClosestRayResultCallback(){};

        Object *getCollidedObject() const;
        Ogre::Vector3 getCollisionPoint() const;

        inline btCollisionWorld::ClosestRayResultCallback *getBulletClosestRayResultCallback() const 
        {
            return static_cast <btCollisionWorld::ClosestRayResultCallback * > (mRayResultCallback);
        }
    };
}
#endif //_OGREBULLETCOLLISIONS_CollisionRay_H

