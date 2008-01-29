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


#include "FreeRunner.h"

#include "TimeManager.h"

namespace CoAJnR
{

FreeRunner::Starter::Starter(FreeRunner* p_instance)
{
    m_instance = p_instance;
}

void
FreeRunner::Starter::operator()()
{
    m_instance->run();
}

FreeRunner::FreeRunner() : m_run(true)
{
    m_starter = new Starter(this);

    m_maxFrameTime = COAJNR_FREERUNNER_FRAME_TIME;
    m_worstFrameTime = 0;

    for(uint i = 0; i < COAJNR_MAX_THREAD_COUNT; i++)
        m_callQueuesDroppable[i] = 
                new rtils::LockfreeQueue<DeferredCall*>(COAJNR_CALL_QUEUE_SIZE);

    for(uint i = 0; i < COAJNR_MAX_THREAD_COUNT; i++)
        m_callQueues[i] = new rtils::LockfreeDynamicQueue<DeferredCall*>();
}

FreeRunner::~FreeRunner()
{
    if(m_starter)
    {
        delete m_starter;
        m_starter = 0;
    }

    DeferredCall* call;

    for(uint i = 0; i < COAJNR_MAX_THREAD_COUNT; i++)
        if(m_callQueuesDroppable[i])
        {
            while(m_callQueuesDroppable[i]->pop(call))
            {
                delete call;
                call = 0;
            }
            delete m_callQueuesDroppable[i];
            m_callQueuesDroppable[i] = 0;
        }

    for(uint i = 0; i < COAJNR_MAX_THREAD_COUNT; i++)
        if(m_callQueues[i])
        {
            while(m_callQueues[i]->pop(call))
            {
                delete call;
                call = 0;
            }
            delete m_callQueues[i];
            m_callQueues[i] = 0;
        }

    while(m_localCallQueue.size())
    {
        delete m_localCallQueue.front();
        m_localCallQueue.pop_front();
    }
}

void
FreeRunner::processCalls()
{
    DeferredCall* call;

    size_t localCallQueueSize = m_localCallQueue.size();

    if(localCallQueueSize)
        for(size_t i = 0; i < localCallQueueSize; i++)
        {
            call = m_localCallQueue.front();
            m_localCallQueue.pop_front();

            call->m_callCount++;

            if((*call)())
                delete call;
            else
            {
                if(call->m_callCount < COAJNR_CALL_MAX_EXECUTION_COUNT)
                    m_localCallQueue.push_back(call);
                else
                    delete call;
            }
        }

    for(uint i = 0; i < COAJNR_MAX_THREAD_COUNT; i++)
        if(m_callQueuesDroppable[i] != 0)
            while(m_callQueuesDroppable[i]->pop(call))
            {
                call->m_callCount++;

                if((*call)())
                    delete call;
                else
                {
                    if(call->m_callCount < COAJNR_CALL_MAX_EXECUTION_COUNT)
                        m_localCallQueue.push_back(call);
                    else
                        delete call;
                }
            }

    for(uint i = 0; i < COAJNR_MAX_THREAD_COUNT; i++)
        if(m_callQueues[i] != 0)
            while(m_callQueues[i]->pop(call))
            {
                call->m_callCount++;

                if((*call)())
                    delete call;
                else
                {
                    if(call->m_callCount < COAJNR_CALL_MAX_EXECUTION_COUNT)
                        m_localCallQueue.push_back(call);
                    else
                        delete call;
                }
            }

}

void
FreeRunner::run()
{
    init();

    TimeStamp lastWorstTimeUpdate = 0;
    TimeStamp timeStamp = 0;
    TimeValue frameTime = 0;
    TimeValue updateTime = 0;

    TimeStamp lastUpdateTimeStamp = TimeManager::get().timeStamp();


    while(m_run)
    {
        timeStamp = TimeManager::get().timeStamp();

        processCalls();

        fastUpdate();

        if(lastUpdateTimeStamp + m_maxFrameTime < timeStamp)
        {
            frameTime = timeStamp - lastUpdateTimeStamp;
            lastUpdateTimeStamp = timeStamp;
            m_lastFrameTime = frameTime;

            if(!update(frameTime))
                break;

            updateTime = TimeManager::get().timeStamp() - timeStamp;

            if(m_worstFrameTime < frameTime ||
                timeStamp - lastWorstTimeUpdate > 5)
            {
                lastWorstTimeUpdate = timeStamp;
                m_worstFrameTime = frameTime;
            }
        }
        else TimeManager::get().wait(0.0001);
    }

    release();
}

bool
FreeRunner::enqueueCallDroppable(DeferredCall* p_call)
{
    ThreadId thread = IdManager::get().threadId();

    assert(thread < COAJNR_MAX_THREAD_COUNT && "increase max thread count");

    return m_callQueuesDroppable[thread]->push(p_call);
}

bool
FreeRunner::enqueueCall(DeferredCall* p_call)
{
     ThreadId thread = IdManager::get().threadId();
     
     assert(thread < COAJNR_MAX_THREAD_COUNT && "increase max thread count");
     
     return m_callQueues[thread]->push(p_call);
}

void
FreeRunner::stop()
{
    m_run = false;
}

}

