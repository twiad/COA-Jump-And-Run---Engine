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


#ifndef COAJNR_TIMEMANAGER_INCLUDED
#define COAJNR_TIMEMANAGER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class TimerComponent;


/// a value describing a point of time (seconds since application startup)
typedef double TimeStamp;

/// a value describing an amount of time (in seconds) like a difference
typedef double TimeValue;

/**
 * general time management.
 */
class TimeManager : boost::noncopyable, public rtils::Singleton<TimeManager>
{
    /// time stamp of TimeManager initialization.
    TimeStamp m_initialTimeStamp;

    std::vector<boost::shared_ptr<TimerComponent> > m_timerComponents;

public:
    /**
     * ctor.
     */
    TimeManager();

    /**
     * dtor.
     */
    virtual ~TimeManager();
    
    /**
     * returns a time stamp, 
     *
     * this is the time since application startup in seconds.
     */
    TimeStamp timeStamp() const;
    
    /**
     * waits for a given duration.
     *
     * the precision of this method may vary. The real amount of time 
     * waited will be returned.
     */
    TimeValue wait(TimeValue duration) const;
    
    /**
     * update all registered timers.
     */
    void update();
    
    /**
     * not locked, only call from logics thread
     */
    void registerTimerComponent(boost::shared_ptr<TimerComponent> component);
    
    /**
     * not locked, only call from logics thread
     */
    void unregisterTimerComponent(boost::shared_ptr<TimerComponent> component);
};

}

#endif
