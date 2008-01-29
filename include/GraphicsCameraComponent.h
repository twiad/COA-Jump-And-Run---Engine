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


#ifndef COAJNR_GRAPHICSGraphicsCameraComponent_INCLUDED
#define COAJNR_GRAPHICSGraphicsCameraComponentGraphicsCameraComponent_INCLUDED

#include "Component.h"
#include "ComponentContainer.h"
#include "PositionComponent.h"

namespace CoAJnR
{

class GraphicsManager;

/**
 * ogre camera.
 */
class GraphicsCameraComponent : 
        public Component, 
        public boost::enable_shared_from_this<GraphicsCameraComponent>
{
    Ogre::SceneNode* m_sceneNode;

    Ogre::Camera* m_camera;
    
    PositionComponentPtr m_positionComponent;

    Ogre::Vector3 m_offset;
    
    Ogre::Quaternion m_offsetRotation;
    
    bool m_autoEnable;
    
protected:
    virtual std::vector<std::string> dependencies();

    virtual void attachedCallback();

    virtual void detachedCallback();
    
public:
    GraphicsCameraComponent(
            bool autoEnable = true,
            const Ogre::Vector3& offset = Ogre::Vector3::ZERO,
            const Ogre::Quaternion& offsetRotation = 
                    Ogre::Quaternion::IDENTITY);
    
    
    virtual ~GraphicsCameraComponent();

    virtual std::string defaultName() const;

    Ogre::SceneNode* sceneNode()
    {
        assert(m_sceneNode && "sceneNode must not be null");
        return m_sceneNode;
    }

    Ogre::Camera* camera()
    {
        assert(m_camera && "camera must not be null");
        return m_camera;
    }
    
    void enable();
};

typedef boost::shared_ptr<GraphicsCameraComponent> 
        GraphicsCameraComponentPtr;

}

#endif
