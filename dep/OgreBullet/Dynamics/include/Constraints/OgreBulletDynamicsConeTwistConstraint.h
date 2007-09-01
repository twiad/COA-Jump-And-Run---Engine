/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

Copyright (c) 2007 tuan.kuranes@gmail.com

    file added by rtti (at) elane2k (dot) com

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
#ifndef _OGREBULLETDYNAMICS_ConeTwistConstraint_H
#define _OGREBULLETDYNAMICS_ConeTwistConstraint_H

#include "OgreBulletDynamicsPrerequisites.h"

#include "OgreBulletDynamicsConstraint.h"


namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    // SixDofConstraint  class
    class ConeTwistConstraint : public TypedConstraint
    {
    public:
        ConeTwistConstraint(RigidBody * rbA, RigidBody * rbB, 
            const Ogre::Vector3& FrameInAVector, const Ogre::Quaternion& FrameInAOrientation,
            const Ogre::Vector3& FrameInBVector, const Ogre::Quaternion& FrameInBOrientation);

        virtual ~ConeTwistConstraint();
        
        void setLimit(Ogre::Real a, Ogre::Real b, Ogre::Real c);

    };
}
#endif //_OGREBULLETDYNAMICS_ConeTwistConstraint_H
