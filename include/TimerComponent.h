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


#ifndef COAJNR_SCENERY_INCLUDED
#define COAJNR_SCENERY_INCLUDED

#include "Dependencies.h"
#include "Component.h"
#include "TimeManager.h"


namespace CoAJnR
{

/**
 * periodically updated component
 */
class TimerComponent : 
        public Component,
        public boost::enable_shared_from_this<TimerComponent>
{

    mutable boost::mutex m_mutex;

    TimeStamp m_lastUpdate;
    bool m_updateInProgress;

protected:
    TimeValue m_interval;

public:
    TimerComponent(TimeValue interval);
    virtual ~TimerComponent();
    
    virtual void update(TimeValue timeSinceLastUpdate) = 0;
    
    void updateInternal();
    
    TimeStamp lastUpdate()
    {
        return m_lastUpdate;
    }
    
    TimeValue interval()
    {
        return m_interval;
    }
    
    bool updateInProgress()
    {
    	return m_updateInProgress;
    }
    
    void setUpdateInProgress(bool p_updateInProgress = true) 
    {
    	m_updateInProgress = p_updateInProgress;
    }
    virtual void attachedCallback();
    virtual void detachedCallback();
};

typedef boost::shared_ptr<TimerComponent> TimerComponentPtr;

}

#endif
