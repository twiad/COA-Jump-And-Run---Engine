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


#ifndef COAJNR_GRAPHICSLIGHTCOMPONENT_INCLUDED
#define COAJNR_GRAPHICSLIGHTCOMPONENT_INCLUDED

#include "PositionComponent.h"
#include "ComponentContainer.h"

namespace CoAJnR
{

class GraphicsManager;

enum LightType
{
    LT_Point = 0,
    LT_Directional,
    LT_Spot
};

/**
 * ogre light.
 */
class GraphicsLightComponent : 
        public Component, 
        public boost::enable_shared_from_this<GraphicsLightComponent>
{
    Ogre::SceneNode* m_sceneNode;
    
    Ogre::Light* m_light;
    
    PositionComponentPtr m_positionComponent;

    LightType m_type;

    Ogre::Vector3 m_offset;
    Ogre::Quaternion m_offsetRotation;

    Ogre::Radian m_outerAngle;
    Ogre::Radian m_innerAngle;
    double m_fallOff;
    
protected:
    virtual std::vector<std::string> dependencies();

    virtual void attachedCallback();

    virtual void detachedCallback();
    
public:
    GraphicsLightComponent(
            LightType type,
            const Ogre::Vector3& offset = Ogre::Vector3::ZERO,
            const Ogre::Quaternion& offsetRotation = 
                    Ogre::Quaternion::IDENTITY,
            Ogre::Radian outerAngle = Ogre::Degree(70),
            Ogre::Radian innerAngle = Ogre::Degree(50),
            double fallOff = 1.0f);
    
    virtual ~GraphicsLightComponent();

    virtual std::string defaultName() const;

    Ogre::Light* light()
    {
        assert(m_light && "light must not be null");
        return m_light;
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

typedef boost::shared_ptr<GraphicsLightComponent> GraphicsLightComponentPtr;

}

#endif

