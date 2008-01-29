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


#ifndef COAJNR_GRAPHICSMESHCOMPONENT_INCLUDED
#define COAJNR_GRAPHICSMESHCOMPONENT_INCLUDED

#include "PositionComponent.h"
#include "ComponentContainer.h"
#include "GraphicsMeshComponentInterface.h"

namespace CoAJnR
{

class GraphicsManager;

/**
 * ogre mesh with texture
 */
class GraphicsMeshComponent : 
        public GraphicsMeshComponentInterface, 
        public boost::enable_shared_from_this<GraphicsMeshComponent>
{
    Ogre::SceneNode* m_sceneNode;
    
    Ogre::Entity* m_entity;
    
    std::string m_meshName;
    
    PositionComponentPtr m_positionComponent;

    Ogre::Vector3 m_offset;
    Ogre::Quaternion m_offsetRotation;
    
    
protected:
    virtual std::vector<std::string> dependencies();

    virtual void attachedCallback();

    virtual void detachedCallback();
    
public:
    GraphicsMeshComponent(
            const std::string& meshName, 
            const Ogre::Vector3& offset = Ogre::Vector3::ZERO,
            const Ogre::Quaternion& offsetRotation = 
                    Ogre::Quaternion::IDENTITY);
    
    virtual ~GraphicsMeshComponent();

    virtual std::string defaultName() const;

    Ogre::Entity* entity()
    {
        assert(m_entity && "entity must not be null");
        return m_entity;
    }

    Ogre::SceneNode* sceneNode()
    {
        assert(m_sceneNode && 
                "sceneNode must not be null");
        return m_sceneNode;
    }
    
    void setPositionOffset(Ogre::Vector3 position = Ogre::Vector3::ZERO);
    void setRotationOffset(
            Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY);
};

typedef boost::shared_ptr<GraphicsMeshComponent> GraphicsMeshComponentPtr;

}

#endif

