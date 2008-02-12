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

#include "TimerComponent.h"
#include "TimeManager.h"
#include "DeferredCall.h"
#include "LogicsManager.h"
#include "ComponentContainer.h"
#include "ThreadPool.h"

namespace CoAJnR
{

TimerComponent::TimerComponent(TimeValue p_interval) :
	m_updateInProgress(false),    
	m_interval(p_interval)
        
{
    
}

TimerComponent::~TimerComponent()
{
    
}

void
TimerComponent::attachedCallback()
{
    m_lastUpdate = TimeManager::get().timeStamp();

    struct RegisterTimerComponent : public DeferredCall
    {
        TimerComponentPtr timerComponent;
        
        bool operator()()
        {
            TimeManager::get().registerTimerComponent(timerComponent);
            
            if(typeid(timerComponent) == typeid(TimerComponent))
                ThreadPool::get().schedule(boost::bind(
                        &ComponentContainer::componentAttachedCallback,
                        timerComponent->parent(), 
                        timerComponent));
            
            return true;
        }
    };

    RegisterTimerComponent* call = new RegisterTimerComponent;
    call->timerComponent = shared_from_this();
    
    LogicsManager::get().enqueueCall(call);
}

void
TimerComponent::detachedCallback()
{
    struct UnregisterTimerComponent : public DeferredCall
    {
        TimerComponentPtr timerComponent;
        
        bool operator()()
        {
            TimeManager::get().unregisterTimerComponent(timerComponent);

            ThreadPool::get().schedule(boost::bind(
                    &ComponentContainer::componentDetachedCallback,
                    timerComponent->parent(), 
                    timerComponent));

            return true;
        }
    };

    UnregisterTimerComponent* call = new UnregisterTimerComponent;
    call->timerComponent = shared_from_this();
    
    LogicsManager::get().enqueueCall(call);
}

void
TimerComponent::updateInternal()
{
    boost::mutex::scoped_lock lock(m_mutex);
    
    TimeStamp ts = TimeManager::get().timeStamp();
    TimeValue timeSinceLastUpdate = ts - m_lastUpdate;
    m_lastUpdate = ts;
    
    update(timeSinceLastUpdate);
    setUpdateInProgress(false);
}
    
}
