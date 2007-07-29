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
#include "Constraints/OgreBulletDynamicsHingeConstraint.h"

using namespace Ogre;

namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    HingeConstraint::HingeConstraint(RigidBody * rbA, RigidBody * rbB, const Vector3& pivotInA,
        const Vector3& pivotInB, const Vector3& axisInA, const Vector3& axisInB):
        TypedConstraint(rbA, rbB)
    {
        btVector3 vec[4];
        vec[0] = OgreBulletCollisions::OgreBtConverter::to(pivotInA);
        vec[1] = OgreBulletCollisions::OgreBtConverter::to(pivotInB);
        vec[2] = OgreBulletCollisions::OgreBtConverter::to(axisInA);
        vec[3] = OgreBulletCollisions::OgreBtConverter::to(axisInB);
        
        mConstraint = new btHingeConstraint(
            *rbA->getBulletRigidBody (),
            *rbB->getBulletRigidBody (), 
            vec[0], vec[1], vec[2], vec[3]);
    }
    // -------------------------------------------------------------------------
        HingeConstraint::HingeConstraint(RigidBody * rbA, 
            const Vector3& pivotInA, 
            const Vector3&  axisInA):
    TypedConstraint(rbA)
    {
        btVector3 vec[2];
        vec[0] = OgreBulletCollisions::OgreBtConverter::to(pivotInA);
        vec[1] = OgreBulletCollisions::OgreBtConverter::to(axisInA);

        mConstraint = new btHingeConstraint(*rbA->getBulletRigidBody (),
            vec[0], vec[1]);
    }
    // -------------------------------------------------------------------------
    HingeConstraint::~HingeConstraint()
    {
    }
}
