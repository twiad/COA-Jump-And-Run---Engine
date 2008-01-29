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


#ifndef COAJNR_DATASTORAGE_INCLUDED
#define COAJNR_DATASTORAGE_INCLUDED

#include "DataObject.h"

namespace CoAJnR
{

/**
 * pool for DataObject.
 *
 * DataStorage objects can get filled with objects. When adding an object, the
 * pool will try to assign a unique identifier (ObjectId) to the added object
 * and return it to get access to the object later. For more infos, see add().
 * Several DataStorage instances can have objects with the same ObjectID values. 
 * This enables eg. a GraphicsDataStorage and a PhysicsDataStorage to store
 * different (independent) information to objects which belong together. 
 *
 * @note instances of this class are not thread safe, use LockedDataStorage 
 * if you need a thread safe DataStorage.
 */
class DataStorage : boost::noncopyable, public rtils::Singleton<DataStorage>
{
    /// how the data is stored in this class
    typedef std::map<ObjectId, DataObjectPtr> StorageType;

    /// iterator for the StorageType
    typedef StorageType::iterator StorageItrType;

    /// const iterator for the StorageType
    typedef StorageType::const_iterator StorageCItrType;

    /// the actual data storage
    StorageType m_data;

    /// mutex for locking storage access
    mutable boost::mutex m_mutex;

public:
    /**
     * ctor.
     */
    DataStorage();
    
    /**
     * ctor.
     */
    virtual ~DataStorage();

    /**
     * returns a shared pointer to the DataObject matching the given identifier.
     * 
     * this method will assert if the id was not found.
     *
     * @note this method is not thread safe, use LockedDataStorage if you need
     * a thread safe DataStorage.
     */
    virtual DataObjectPtr object(ObjectId id) const;
    
    /**
     * returns a shared pointer of type T to the 
     * DataObject matching the given identifier,
     * 
     * returns 0 if the object cannot be casted to the given type. 
     * this method will assert if the id was not found.
     *
     * @note this method is not thread safe, use LockedDataStorage if you need
     * a thread safe DataStorage.
     */
    template<class T>
    boost::shared_ptr<T> object(ObjectId id) const
    {
        return boost::dynamic_pointer_cast<T>(object(id));
    }
    
    /**
     * adds a given DataObject to the pool.
     *
     * if no ObjectId is provided,
     * a new one will be generated using the IdManager, if an ObjectId is 
     * given to this method as a parameter, the
     * pool will try to use the given one instead of generating a new one.
     * 
     * this method will assert if the given id is already used.
     *
     * @note this method is not thread safe, use LockedDataStorage if you need
     * a thread safe DataStorage.
     */
    virtual ObjectId add(DataObjectPtr object, ObjectId id = 0);
    
    virtual ObjectId add(DataObject* object, ObjectId id = 0);
    
    /**
     * removes the DataObject with the given identifier from the data pool.
     * 
     * if the DataStorage was the last one holding a reference to the 
     * DataObject, the object will be deleted due to shared pointers.
     *
     * this method will assert if the given id is not available.
     *
     * @note this method is not thread safe, use LockedDataStorage if you need
     * a thread safe DataStorage.
     */
    virtual DataObjectPtr remove(ObjectId id);
};

}

#endif
