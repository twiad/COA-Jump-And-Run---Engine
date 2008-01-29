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


#ifndef COAJNR_DEFERREDCALL_INCLUDED
#define COAJNR_DEFERREDCALL_INCLUDED

namespace CoAJnR
{

/**
 * base for all functors which are meant for later execution.
 *
 * by overwriting the operator()() in a derived call, 
 * code for later execution can be defined.
 *
 * an instance of the newly created class or struct can be send to a 
 * FreeRunner by calling FreeRunner::enqueueCallDroppable(). The FreeRunner will in turn
 * store the object in a list (FreeRunner::enqueueCallDroppable() is thread safe and 
 * can be called from any point in the application) and execute it in its next
 * update run (FreeRunner::processCalls()). this moves the code defined in 
 * operator()() into the thread of the FreeRunner the call was enqueued at.
 */
class DeferredCall : boost::noncopyable
{
    friend class FreeRunner;
    
    uint m_callCount;
    
public:
    /**
     * ctor.
     */
    DeferredCall() { m_callCount = 0; }
    
    /**
     * dtor.
     */
    virtual ~DeferredCall() { };
    
    /**
     * contains the actual code.
     *
     * will be executed in the target thread by FreeRunner::processCalls().
     */
    virtual bool operator()() = 0;    
    
};

/// give a call name and define its member names behind this macro
#define DEFERRED_CALL_VARS(callName) \
        struct callName : public DeferredCall {

/// after, the DEFERRED_CALL_VARS makro, put this followed by your call code
#define DEFERRED_CALL_CODE() \
        bool operator()() {

/// finalize your call code, now you can create, configure and enqueue the call
#define DEFERRED_CALL_END() \
        }};

#define DEFERRED_CALL_CREATE(callName) \
        callName* call = new callName;
        
#define DEFERRED_CALL_ENQUEUE(manager) \
        manager::get().enqueueCall(call);

#define DEFERRED_CALL_ENQUEUE_DROPABLE(manager) \
        manager::get().enqueueCallDropable(call);

}

#endif

