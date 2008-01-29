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


#ifndef COAJNR_GRAPHICSANIMATIONCONTROLLER_INCLUDED
#define COAJNR_GRAPHICSANIMATIONCONTROLLER_INCLUDED

#include "Dependencies.h"
#include "ThreadPool.h"
#include "Component.h"
#include "TimerComponent.h"
#include "GraphicsManager.h"
#include "GraphicsMeshComponent.h"

namespace CoAJnR
{

class GraphicsAnimationController : 
    public TimerComponent
{
    Ogre::AnimationState* m_animationState;

    GraphicsMeshComponentPtr m_meshComponent;
    std::string m_animationName;

public:
    GraphicsAnimationController(std::string p_animationName);

    void attachedCallback();
    void detachedCallback();
    void update(TimeValue t);
    std::vector<std::string> dependencies();
    std::string defaultName() const;
};


}

#endif
