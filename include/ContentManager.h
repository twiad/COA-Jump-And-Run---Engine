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


#ifndef COAJNR_ContentManager_INCLUDED
#define COAJNR_ContentManager_INCLUDED

#include "Dependencies.h"

#include "ComponentContainer.h"
#include "PositionComponent.h"
#include "GraphicsMeshComponent.h"
#include "GraphicsStaticMeshComponent.h"
#include "PhysicsComponent.h"

namespace CoAJnR
{
/**
 * Responsable for object creation, destruction and group management.
 */
class ContentManager : public rtils::Singleton<ContentManager>
{
    mutable boost::mutex m_groupMutex;
    
    std::map<std::string, std::vector<ComponentContainerPtr> > m_groups;

    std::string m_currentGroup;
    
public:
    ContentManager();
    
    virtual ~ContentManager();

    ComponentContainerPtr createEmptyContainer();
    
    ComponentContainerPtr createStaticObject(
            const std::string& meshName,
            const Ogre::Vector3& initialPosition = 
                    Ogre::Vector3::ZERO,
            const Ogre::Quaternion& initialOrientation = 
                    Ogre::Quaternion::IDENTITY,
            PhysicsShape shape = PS_Trimesh);
    
    ComponentContainerPtr createDynamicObject(
            const std::string& meshName,
            const Ogre::Vector3& initialPosition = 
                    Ogre::Vector3::ZERO,
            const Ogre::Quaternion& initialOrientation = 
                    Ogre::Quaternion::IDENTITY,
            double mass = 1.0f,
            PhysicsShape shape = PS_Convex);

    void destroyObject(ComponentContainerPtr obj);
    
    void addToGroup(
            ComponentContainerPtr obj, 
            std::string group = "Default");

    void removeFromGroup(
            ComponentContainerPtr obj, 
            std::string group = "Default");

    void removeGroup(std::string group = "Default");

    void removeAllGroups();
};

}

#endif

