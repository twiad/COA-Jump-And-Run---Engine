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


#ifndef COAJNR_POSITIONCOMPONENT_INCLUDED
#define COAJNR_POSITIONCOMPONENT_INCLUDED

#include "Component.h"
#include "ComponentContainer.h"

namespace CoAJnR
{

/**
 * represents a position in 3d space, including linear velocity.
 */
class PositionComponent : 
        public Component, 
        public boost::enable_shared_from_this<PositionComponent>
{
    Ogre::Vector3 m_position;
    Ogre::Quaternion m_orientation;

    Ogre::SceneNode* m_sceneNode;

    Ogre::Vector3 m_velocity;

    bool m_manualRotationUpdate;

    mutable boost::mutex m_mutex;
    
protected:
    virtual void attachedCallback();

    virtual void detachedCallback();
    
public:
    PositionComponent(
            const Ogre::Vector3 position = Ogre::Vector3::ZERO, 
            const Ogre::Quaternion orientation = Ogre::Quaternion::IDENTITY,
            bool manualRotationUpdate = false);
    
    virtual ~PositionComponent();

    virtual std::string defaultName() const;

    Ogre::SceneNode* sceneNode()
    {
        assert(m_sceneNode && "sceneNode must not be null");
        return m_sceneNode;
    }

    Ogre::Vector3 position() const;
    Ogre::Quaternion orientation() const;
    Ogre::Vector3 velocity() const;

    void setPosition(const Ogre::Vector3& position);
    void setOrientation(const Ogre::Quaternion& orientation);

    void dataChangedCallback(
            const Ogre::Vector3& position, 
            const Ogre::Quaternion& orientation,
            const Ogre::Vector3& velocity,
            bool force = false);
            
    void setManualRotationUpdate(bool flag = true);
};

typedef boost::shared_ptr<PositionComponent> PositionComponentPtr;

}

#endif
