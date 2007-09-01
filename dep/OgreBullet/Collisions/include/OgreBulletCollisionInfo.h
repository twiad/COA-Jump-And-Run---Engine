/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

Copyright (c) 2007 tuan.kuranes@gmail.com

file added by rtti@elane2k.com

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

#ifndef _OGREBULLETCOLLISIONS_CollisionInfo_H
#define _OGREBULLETCOLLISIONS_CollisionInfo_H

#include "OgreBulletCollisionsPrerequisites.h"


namespace OgreBulletCollisions
{

    class CollisionInfo 
    {
        Object* m_object;
        Object* m_partner;
        btVector3 m_localPosition;
        btVector3 m_worldPosition;
        
    public:
        CollisionInfo(Object* object, Object* partner, btVector3 localPosition, btVector3 worldPosition)
        {
            m_partner = partner;
            m_localPosition = localPosition;
            m_worldPosition = worldPosition;
        }
        
        virtual ~CollisionInfo() { };
        
        Object* getObject() { return m_object; }
        Object* getPartner() { return m_partner; }
        btVector3 getLocalPosition() { return m_localPosition; }
        btVector3 getWorldPosition() { return m_worldPosition; }
    };
}
#endif //_OGREBULLETCOLLISIONS_CollisionInfo_H

