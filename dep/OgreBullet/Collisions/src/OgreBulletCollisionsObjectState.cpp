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

#include "OgreBulletCollisions.h"

#include "Utils/OgreBulletConverter.h"
#include "OgreBulletCollisionsObject.h"
#include "OgreBulletCollisionsObjectState.h"

using namespace Ogre;

namespace OgreBulletCollisions
{
    std::map<Object*, btTransform> ObjectState::m_transformationCache;
    SDL_mutex* ObjectState::m_tranformationCacheMutex = SDL_CreateMutex();

    
    // -------------------------------------------------------------------------
    ObjectState::ObjectState(Object *parent):	
        mObject(parent)
    {
        // create an empty entry in the objects transformation cache
        lockTransformationCache();
        m_transformationCache[mObject] = btTransform(btQuaternion(0,0,0,1));
        unlockTransformationCache();
    }
    // -------------------------------------------------------------------------
    ObjectState::~ObjectState()
    {
        lockTransformationCache();
        m_transformationCache.erase(mObject);
        unlockTransformationCache();
    }
    // -------------------------------------------------------------------------
    void ObjectState::getWorldTransform(btTransform& worldTrans) const
    {
        assert (mObject);

        // could use mWorldTrans ?
        worldTrans.setOrigin(OgreBtConverter::to(mObject->getWorldPosition()));
        worldTrans.setRotation(OgreBtConverter::to(mObject->getWorldOrientation()));
    }
    // -------------------------------------------------------------------------
    void ObjectState::setWorldTransform(const btTransform& worldTrans)
    {
        assert (mObject);        

        /// @todo TODO: check this strange behaviour

        // use transformation cache instead of syncing directly
        // mObject->setTransform(worldTrans);
        
        lockTransformationCache();
        m_transformationCache[mObject] = worldTrans;
        unlockTransformationCache();
        
        mWorldTrans = worldTrans;        
    }
}
