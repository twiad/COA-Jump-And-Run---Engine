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


#ifndef COAJNR_EDENBODYPARTCOMPONENT_INCLUDED
#define COAJNR_EDENBODYPARTCOMPONENT_INCLUDED

#include "Dependencies.h"

#include "Component.h"
#include "PositionComponent.h"

#include "bodypart.h"

namespace CoAJnR
{

using namespace EDen;

class EdenBodypartComponent :
        public Component,
        public boost::enable_shared_from_this<EdenBodypartComponent>
{

protected:
    virtual std::vector<std::string> dependencies();
    virtual void attachedCallback();
    virtual void detachedCallback();
    
    Bodypart* bp;
 
public:

    EdenBodypartComponent(Bodypart* bp);

    virtual ~EdenBodypartComponent();

    virtual std::string defaultName() const;

    Bodypart* getBodypart();
};

typedef boost::shared_ptr<EdenBodypartComponent> EdenBodypartComponentPtr;

}

#endif

