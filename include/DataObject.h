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


#ifndef COAJNR_DATAOBJECT_INCLUDED
#define COAJNR_DATAOBJECT_INCLUDED

#include "Dependencies.h"

#include "IdManager.h"

/// this macro enables easy locking of DataObject's
#define LOCKME boost::mutex::scoped_lock scopedAutoLock(mutex());

namespace CoAJnR
{

class DataStorage;

/**
 * base for all data objects. 
 *
 * those objects are meant to be stored in DataStorage objects.
 */
class DataObject : boost::noncopyable
{
    /// DataStorage can and must set the Object's id.
    friend class DataStorage;

    /// lock on object level
    mutable boost::mutex m_mutex;

    /// unique system wide identifier of the object
    ObjectId m_id;

protected:
    /**
     * get access to a reference of this objects mutex.
     *
     * there should be no need to use this directly, 
     * use the LOCKME macro instead
     *
     * @note this method is completely thread safe.
     */
    boost::mutex& mutex() const { return m_mutex; }

public:
    /**
     * ctor.
     * the objects id gets initialized to 0 (invalid) here and will be set by
     * a DataStorage object as soon as this objects gets added to it.
     *
     * @note this method is completely thread safe.
     */
    DataObject() 
    { 
        m_id = 0; 
    }
    
    /**
     * dtor.
     */
    virtual ~DataObject() { }
    
    /**
     * get the unique ObjectId of this object.
     * this is 0 (invalid) as long as this object is not added to a DataStorage.
     * 
     * @note this method is thread safe as soon as the object was added to a 
     * DataStorage and the id was set. The id never changes after that.
     */
    ObjectId id() const { return m_id; }
};

/// memory managed shared pointer to data objects
typedef boost::shared_ptr<DataObject> DataObjectPtr;

}

#endif

