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


#include "IdManager.h"

namespace CoAJnR
{

IdManager::IdManager()
{
    {
        boost::mutex::scoped_lock lock(m_objectIdMutex);
        m_lastObjectId = 0;
    }

    {
        boost::mutex::scoped_lock lock(m_threadIdMutex);
        m_nextThreadId = 0;
    }

    {
        boost::mutex::scoped_lock lock(m_localIdMutex);
        m_lastLocalId = 0;
    }
}

IdManager::~IdManager()
{

}

ThreadId
IdManager::nextThreadId()
{
    return m_nextThreadId++;
}

ObjectId
IdManager::newObjectId()
{
    boost::mutex::scoped_lock lock(m_objectIdMutex);
    return ++m_lastObjectId;
}

ObjectId
IdManager::newLocalId()
{
    boost::mutex::scoped_lock lock(m_localIdMutex);
    return ++m_lastLocalId;
}

ThreadId
IdManager::threadId()
{
    if(!m_threadIdStorage.get())
    {
        boost::mutex::scoped_lock lock(m_threadIdMutex);
        m_threadIdStorage.reset(new ThreadId(nextThreadId()));
    }
    
    return *m_threadIdStorage;
}

   
void 
IdManager::setThreadName(const std::string& p_name)
{
    if(!m_threadNameStorage.get())
        m_threadNameStorage.reset(new std::string);
    
    *m_threadNameStorage = p_name;
}
    
const std::string& 
IdManager::threadName()
{
    if(!m_threadNameStorage.get())
        m_threadNameStorage.reset(new std::string("<unnamed thread>"));
    
    return *m_threadNameStorage;
}


}

