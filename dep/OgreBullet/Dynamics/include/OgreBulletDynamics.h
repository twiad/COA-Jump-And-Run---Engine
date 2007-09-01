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

#ifndef _OGREBULLETDYNAMICS_H
#define _OGREBULLETDYNAMICS_H

#include "OgreBulletDynamicsPrerequisites.h"

#ifdef _PRECOMP

    #include "OgreBulletDynamicsRigidBody.h"
    #include "OgreBulletDynamicsObjectState.h"

    #include "OgreBulletDynamicsConstraint.h"
    #include "Constraints/OgreBulletDynamics6DofConstraint.h"
    #include "Constraints/OgreBulletDynamicsHingeConstraint.h"
    #include "Constraints/OgreBulletDynamicsPoint2pointConstraint.h"
    #include "Constraints/OgreBulletDynamicsConeTwistConstraint.h"

    #include "Constraints/OgreBulletDynamicsRaycastVehicle.h"

    #include "OgreBulletDynamicsWorld.h"

#endif //_PRECOMP
 
#endif //_OGREBULLETDYNAMICS_H
