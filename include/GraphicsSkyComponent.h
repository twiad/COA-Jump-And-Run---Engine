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


#ifndef COAJNR_GraphicsSkyComponent_INCLUDED
#define COAJNR_GraphicsSkyComponent_INCLUDED

#include "PositionComponent.h"
#include "ComponentContainer.h"

namespace CoAJnR
{

class GraphicsManager;

enum SkyType
{
    ST_Box = 0,
    ST_Dome,
    // ST_Plane
};

/**
 * ogre skybox and skydome.
 */
class GraphicsSkyComponent : 
        public Component, 
        public boost::enable_shared_from_this<GraphicsSkyComponent>
{
    
    std::string m_name;
    
    SkyType m_type;
    
protected:
    virtual void attachedCallback();

    virtual void detachedCallback();
    
public:
    GraphicsSkyComponent(const std::string& name, SkyType sky = ST_Box);
    
    virtual ~GraphicsSkyComponent();

    virtual std::string defaultName() const;
};

typedef boost::shared_ptr<GraphicsSkyComponent> GraphicsSkyComponentPtr;

}

#endif

