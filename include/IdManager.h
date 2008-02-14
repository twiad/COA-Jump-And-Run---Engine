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


#ifndef COAJNR_IDMANAGER_INCLUDED
#define COAJNR_IDMANAGER_INCLUDED

#include "Dependencies.h"

#include "Singleton.h"

namespace CoAJnR
{

/// machine wide unique identifier
typedef unsigned int LocalId;

/// system wide unique object identifier
typedef unsigned int ObjectId;

/// machine wide unique thread identifier
typedef unsigned int ThreadId;

/**
 * generate unique ObjectId and ThreadId numbers and handle thread names.
 */
class IdManager : boost::noncopyable, public rtils::Singleton<IdManager>
{
    /// lock for generating thread safe ObjectId numbers
    mutable boost::mutex m_objectIdMutex;

    /// lock for generating thread safe ThreadId numbers
    mutable boost::mutex m_threadIdMutex;

    /// lock for locally unique id
    mutable boost::mutex m_localIdMutex;
    
    /// thread specific storage for ThreadId numbers
    boost::thread_specific_ptr<ThreadId> m_threadIdStorage;

    /// thread specific storage for thread names
    boost::thread_specific_ptr<std::string> m_threadNameStorage;

    /// lastly generated ObjectId
    ObjectId m_lastLocalId;

    /// lastly generated ObjectId
    ObjectId m_lastObjectId;
    
    /// ThreadId for the next thread
    ThreadId m_nextThreadId;

    /**
     * returns the next ThreadId number.
     *
     * @note this method is not thread safe.
     */
    ThreadId nextThreadId();
    
public:
    /**
     * ctor.
     */
    IdManager();

    /**
     * dtor.
     */
    virtual ~IdManager();

    /**
     * returns a new object id.
     *
     * @note this method is thread safe.
     */
    ObjectId newObjectId();

    /**
     * returns the id of the calling thread
     *
     * @note this method is thread safe.
     */
    ThreadId threadId();
    
    /**
     * returns a locally unique id.
     */
    LocalId newLocalId();
    
    /**
     * sets a name for the current thread.
     *
     * @note this method is thread safe.
     */
    void setThreadName(const std::string& name);
    
    /**
     * returns the name of the current thread, if no name was set,
     * "<unnamed thread>" will be returned.
     *
     * @note this method is thread safe.
     */
    const std::string& threadName();
};

}

#endif

