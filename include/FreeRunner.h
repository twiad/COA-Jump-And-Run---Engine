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


#ifndef COAJNR_FREERUNNER_INCLUDED
#define COAJNR_FREERUNNER_INCLUDED

#include "Dependencies.h"

#include "DeferredCall.h"
#include "IdManager.h"
#include "LockfreeDynamicQueue.h"
#include "LockfreeQueue.h"
#include "TimeManager.h"

namespace CoAJnR
{

/**
 * base for all threaded free running managers.
 *
 * inherit this class to create a manager which can get continuously updated
 * in its own thread, and is able to receive DeferredCall objects from 
 * other threads.
 *
 * in general, FreeRunner instances are not meant to get accessed directly by
 * other threads, since FreeRunner objects do not provide any locking mechanism 
 * for synchronization. this is due to performance reasons since locking always 
 * means waiting too. to work on a FreeRunner, construct DeferredCall objects
 * and send them to the FreeRunner via enqueueCallDroppable().
 *
 * FreeRunner objects also have a target update rate, which they try to reach.
 * DeferredCall objects send to a FreeRunner via enqueueCallDroppable() are evaluated 
 * as fast as possible independent from the target update rate.
 */
class FreeRunner : boost::noncopyable 
{
    bool m_run;
    
    /**
     * helper to launch a FreeRunner
     *
     * @note poltergeist anti-pattern
     */
    struct Starter
    {
        /// the FreeRunner to run.
        FreeRunner* m_instance;
        /// ctor.
        Starter(FreeRunner* instance);
        /// the threads main function
        void operator()();
    };
    
    /// the starter which is responsable for this FreeRunner instance.
    Starter* m_starter;
    
    /// queue type for DeferredCall objects queued locally.
    typedef std::list<DeferredCall*> LocalCallQueueType;
    
    /// type of DeferredCall LockfreeQueue.
    typedef rtils::LockfreeQueue<DeferredCall*>* CallQueueArrayDroppableType;

    typedef rtils::LockfreeDynamicQueue<DeferredCall*>* CallQueueArrayType;
    
    /// local call queue.
    LocalCallQueueType m_localCallQueue;
    
    /// the actual call queue array, one queue per possible calling thread
    CallQueueArrayDroppableType m_callQueuesDroppable[COAJNR_MAX_THREAD_COUNT];

    CallQueueArrayType m_callQueues[COAJNR_MAX_THREAD_COUNT];

    /// target amount of time spent for one update in seconds
    TimeValue m_maxFrameTime;
    
    /// time used for the last update in seconds
    TimeValue m_lastFrameTime;

    /// worst frame time in seconds, reset after 5 seconds
    TimeValue m_worstFrameTime; 

    /**
     * processes all the enqueue calls.
     */
    void processCalls();
        
    /**
     * the threads main function
     */
    void run();

protected:
    /**
     * do initialization routines.
     *
     * @note this method is not meant to be thread safe and should only get
     * called by the the thread this FreeRunner is running in
     */
    virtual void init() { }
    
    /**
     * this method is called as often as the maschine allows it
     *
     * @note this method is not meant to be thread safe and should only get
     * called by the the thread this FreeRunner is running in
     */
    virtual void fastUpdate() { }
    
    /**
     * this method is called in m_maxFrameTime intervals
     *
     * @note this method is not meant to be thread safe and should only get
     * called by the the thread this FreeRunner is running in
     */
    virtual bool update(TimeValue elapsed) { return true; }
    
    /**
     * do cleanup routines.
     *     
     * @note this method is not meant to be thread safe and should only get
     * called by the the thread this FreeRunner is running in
     */
    virtual void release() { }

    /**
     * sets the target amount of time spent for one update in seconds
     *
     * @note this method is not thread safe atm.
     *
     * @todo make this thread safe of find another way to call it from outside.
     */
    void setMaxFrameTime(TimeValue maxFrameTime)
    {
        assert(maxFrameTime > 0 && "maxFrameTime must be positive");
        m_maxFrameTime = maxFrameTime;
    }
     
public:
    /**
     * ctor.
     */
    FreeRunner();
    
    /**
     * dtor.
     */
    virtual ~FreeRunner();
    
    /**
     * returns the starter functor which starts this FreeRunner.
     *
     * @note after the constructor is done, the starter pointer does not change
     * anymore... so this method should be thread safe.
     */
    Starter& starter() const
    {
        assert(m_starter);
        return *m_starter;
    }

    /**
     * enqueue a DeferredCall for later exection. returns false if queue full.
     *
     * @note as long as there are no more threads than COAJNR_MAX_THREAD_COUNT
     * this method is thread safe.
     */
    bool enqueueCallDroppable(DeferredCall* call);

    /**
     * enqueues a call which cannot be dropped.
     */
    bool enqueueCall(DeferredCall* call);
    
    /**
     * returns the target loop duration in seconds.
     *
     * @note this method is not thread safe, but in general this value does
     * not change during runtime, so locking is not really needed here.
     */
    TimeValue maxFrameTime() const
    { 
        return m_maxFrameTime; 
    }
    
    /**
     * returns the last loop duration in seconds.
     *
     * @note this call is not thread safe since it is altered continously
     * during FreeRunner update.
     */
    TimeValue lastFrameTime() const
    {
        return m_lastFrameTime;
    }

    /**
     * returns the worst loop duration in seconds.
     *
     * @note this call is not thread safe since it is altered continously
     * during FreeRunner update.
     */
    TimeValue worstFrameTime() const
    {
        return m_worstFrameTime;
    }
    
    /**
     * make this FreeRunner stop after its next loop.
     */
    void stop();
};

}

#endif

