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

#ifndef _OGREBULLETCOLLISIONS_ObjectState_H
#define _OGREBULLETCOLLISIONS_ObjectState_H

#include "OgreBulletCollisionsPrerequisites.h"


namespace OgreBulletCollisions
{
    class ObjectState : public btMotionState
    {        
    public:
	    ObjectState(Object *parent);
	    ~ObjectState();
    	
	    virtual void getWorldTransform(btTransform& worldTrans ) const;
        virtual void setWorldTransform(const btTransform& worldTrans);	

        static bool lockTransformationCache()
        {
            return SDL_LockMutex(m_tranformationCacheMutex);
        }

        static bool unlockTransformationCache()
        {
            return SDL_UnlockMutex(m_tranformationCacheMutex);            
        }

        static const std::map<Object*, btTransform>& transformationCache()
        {
            return m_transformationCache;
        }

    private:
        Object      *mObject;
        btTransform  mWorldTrans;
        
        static SDL_mutex* m_tranformationCacheMutex;
        static std::map<Object*, btTransform> m_transformationCache;
    };
}
#endif //_OGREBULLETCOLLISIONS_ObjectState_H

