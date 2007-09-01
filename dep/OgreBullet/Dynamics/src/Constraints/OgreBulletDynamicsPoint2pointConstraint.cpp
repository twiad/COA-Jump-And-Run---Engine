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
#include "Constraints/OgreBulletDynamicsPoint2pointConstraint.h"

using namespace Ogre;

namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    PointToPointConstraint::PointToPointConstraint(RigidBody *body, const Vector3 &pivot) :
        TypedConstraint(body)
    {
        mConstraint = new btPoint2PointConstraint(*(body->getBulletRigidBody ()), 
            OgreBulletCollisions::OgreBtConverter::to(pivot));
    }
    // -------------------------------------------------------------------------
    PointToPointConstraint::PointToPointConstraint(RigidBody *body1, RigidBody *body2, 
            const Vector3 &pivot1, const Vector3 &pivot2) :
        TypedConstraint(body1, body2)
    {
        mConstraint = new btPoint2PointConstraint(*(body1->getBulletRigidBody ()),
            *(body2->getBulletRigidBody ()),
            OgreBulletCollisions::OgreBtConverter::to(pivot1),
            OgreBulletCollisions::OgreBtConverter::to(pivot2));
    }
    // -------------------------------------------------------------------------
    PointToPointConstraint::~PointToPointConstraint()
    {
    }
    // -------------------------------------------------------------------------
    void	PointToPointConstraint::setPivotA(const Ogre::Vector3& pivotA)
    {
        getBulletPoint2PointConstraint()->setPivotA(
            OgreBulletCollisions::OgreBtConverter::to(pivotA));
    }
    // -------------------------------------------------------------------------
    void	PointToPointConstraint::setPivotB(const Ogre::Vector3& pivotB)
    {
        getBulletPoint2PointConstraint()->setPivotB(
            OgreBulletCollisions::OgreBtConverter::to(pivotB));
    }
    // -------------------------------------------------------------------------
    void PointToPointConstraint::setTau (float tau) 
    {
        getBulletPoint2PointConstraint()->m_setting.m_tau = tau;
    };
    // -------------------------------------------------------------------------
    void PointToPointConstraint::setDamping (float damping) 
    {
        getBulletPoint2PointConstraint()->m_setting.m_damping = damping;
    };
    // -------------------------------------------------------------------------
    float PointToPointConstraint::getTau () const 
    {
        return getBulletPoint2PointConstraint()->m_setting.m_tau;
    };
    // -------------------------------------------------------------------------
    float PointToPointConstraint::getDamping () const 
    {
        return getBulletPoint2PointConstraint()->m_setting.m_damping;
    };
}
