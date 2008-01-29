/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2007  Adrian Jäkel                       *
 *                     Copyright (C) 2007  Franz Köhler                       *
 *                     Copyright (C) 2007  Robert Timm                        *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/


#include "CollisionHandlerComponentTest.h"

namespace CoAJnR
{

void 
CollisionHandlerComponentTest::handleCollision(
        PhysicsComponentPtr collisionPartner,
        Ogre::Vector3 localPointOfCollision,
        Ogre::Vector3 globalPointOfCollision)
{
    std::cout << "someone hit" << std::endl;
    std::cout << "global: " << globalPointOfCollision << std::endl;
    std::cout << "local: " << localPointOfCollision << std::endl;
}

std::string 
CollisionHandlerComponentTest::defaultName() const
{
    return std::string("TestCollisionHander"); 
}

}
