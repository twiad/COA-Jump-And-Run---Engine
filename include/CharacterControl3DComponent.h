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


#ifndef COAJNR_CHARACTERCONTROL3DCOMPONENT_INCLUDED
#define COAJNR_CHARACTERCONTROL3DCOMPONENT_INCLUDED

#include "Dependencies.h"

#include "Component.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "GraphicsMeshComponent.h"


namespace CoAJnR
{

/**
 * Enables character-like movement behaviour.
 *
 * This Component enables a ComponentContainer which has a PhysicsComponent to
 * move like a typical character, including walking, turning and jumping.
 */
class CharacterControl3DComponent :
        public Component,
        public boost::enable_shared_from_this<
                CharacterControl3DComponent>
{
    PositionComponentPtr m_positionComponent;
    PhysicsComponentPtr m_physicsComponent;

protected:
    virtual void attachedCallback();
    virtual void detachedCallback();

    std::vector<std::string> dependencies();

public:
    CharacterControl3DComponent();
    
    virtual ~CharacterControl3DComponent();

    std::string defaultName() const;

    void forward(double amount = 1.0f);
    void backward(double amount = 1.0f);
    void turnLeft(double amount = 1.0f);
    void turnRight(double amount = 1.0f);
    void jump(double amount = 1.0f);
};

typedef boost::shared_ptr<CharacterControl3DComponent>
        CharacterControl3DComponentPtr;

}

#endif

