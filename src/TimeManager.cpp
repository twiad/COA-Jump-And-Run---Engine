 /*****************************************************************************
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


#include "TimeManager.h"
#include "TimerComponent.h"
#include "ThreadPool.h"

namespace CoAJnR
{

TimeManager::TimeManager() : m_threadPool(ThreadPool::get())
{
    m_initialTimeStamp = 0;
    m_initialTimeStamp = timeStamp();
}

TimeManager::~TimeManager()
{

}

TimeStamp
TimeManager::timeStamp() const
{
#if defined(__MAC__) || defined(__LINUX__)
    boost::xtime timeStamp;
    boost::xtime_get(&timeStamp, boost::TIME_UTC);
    return (timeStamp.sec - m_initialTimeStamp) + 
            (timeStamp.nsec / 1000000000.0f);
#endif
#if defined(__WIN32__) || defined(__MINGW__) 
	return (timeGetTime() / 1000.0f) - m_initialTimeStamp;
#endif
}

TimeValue
TimeManager::wait(TimeValue p_duration) const
{
    if(!p_duration)
        p_duration = 0.00001;

    TimeStamp waitStart = timeStamp();

    boost::xtime wakeupTime;
    boost::xtime_get(&wakeupTime, boost::TIME_UTC);

    uint timeToWaitSec = (uint)p_duration;
    uint timeToWaitNSec = (uint)((p_duration - timeToWaitSec) * 1000000000);

    if(timeToWaitNSec + wakeupTime.nsec > 1000000000)
    {
        wakeupTime.sec += timeToWaitSec + 1;
        wakeupTime.nsec += timeToWaitNSec - 1000000000;
    }
    else
    {
        wakeupTime.sec += timeToWaitSec;
        wakeupTime.nsec += timeToWaitNSec;
    }

    boost::thread::sleep(wakeupTime);

    return timeStamp() - waitStart;
}

void
TimeManager::update()
{
    TimeStamp timeStamp = this->timeStamp();
    
    std::vector<boost::shared_ptr<TimerComponent> >::iterator it;
    for(it = m_timerComponents.begin(); it != m_timerComponents.end(); it++)
        if(!(*it)->updateInProgress() && 
                ((*it)->lastUpdate() + (*it)->interval()) <= timeStamp)
        {
        	(*it)->setUpdateInProgress();
            m_threadPool.schedule(boost::bind(
                    &TimerComponent::updateInternal, *it));
        }
}

void 
TimeManager::registerTimerComponent(
        boost::shared_ptr<TimerComponent> p_component)
{
    m_timerComponents.push_back(p_component);
}

void 
TimeManager::unregisterTimerComponent(
        boost::shared_ptr<TimerComponent> p_component)
{
    std::vector<boost::shared_ptr<TimerComponent> >::iterator it;
    for(it = m_timerComponents.begin(); it != m_timerComponents.end(); it++)
        if(*it == p_component)
        {
            m_timerComponents.erase(it);
            break;
        }
}

}
