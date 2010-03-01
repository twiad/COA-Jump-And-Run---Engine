/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2010  Franz Köhler                       *
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

#include "EdenOrganismComponent.h"

#include "PhysicsComponent.h"
#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "GraphicsMeshComponent.h"
#include "LogicsManager.h"
#include "ThreadPool.h"

#include "organism.h"

namespace CoAJnR
{

using namespace EDen;

EdenOrganismComponent::EdenOrganismComponent(Organism* pOrganism)
{
  org = pOrganism;
}

EdenOrganismComponent::~EdenOrganismComponent()
{
}

std::string 
EdenOrganismComponent::defaultName() const
{
    return std::string("EDenOrganism");
}


std::vector<std::string> 
EdenOrganismComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("Position");

    return dependencies;
}

void
EdenOrganismComponent::attachedCallback()
{
    assert(parent() && "parent must not be null");
    assert(parent()->id() && "component container has invalid ObjectId");

    mPositionComponent = 
    boost::dynamic_pointer_cast<PositionComponent>(parent()->component("Position"));
}

void
EdenOrganismComponent::detachedCallback()
{
  
}

} // namespace
