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


#include "DataStorage.h"

namespace CoAJnR
{

DataStorage::DataStorage()
{

}

DataStorage::~DataStorage()
{

}

DataObjectPtr  
DataStorage::object(ObjectId p_id) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    StorageCItrType it = m_data.find(p_id);
    
    assert(it != m_data.end() && "requested not existing ObjectId.");
    
    return (*it).second;
}

ObjectId  
DataStorage::add(DataObjectPtr p_object, ObjectId p_id)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if(!p_id)
        p_id = IdManager::get().newObjectId();
    
    /// @todo TODO: optimize me, no search needed for ids from id manager? 
    /// way around?
    StorageItrType it = m_data.find(p_id);
    
    assert(it == m_data.end() && "tried to add already existing ObjectId.");
    assert(!p_object->m_id && "object already has id set.");
    
    p_object->m_id = p_id;

    m_data[p_id] = p_object;
    
    return p_id;
}

ObjectId
DataStorage::add(DataObject* p_object, ObjectId p_id)
{
    return add(DataObjectPtr(p_object), p_id);
}

DataObjectPtr
DataStorage::remove(ObjectId p_id)
{
    boost::mutex::scoped_lock lock(m_mutex);

    StorageItrType it = m_data.find(p_id);
    DataObjectPtr ptr = (*it).second;
    
    assert(it != m_data.end() && "tried to remove a not existing ObjectId.");
    
    m_data.erase(it);
    
    return ptr;
}

}
