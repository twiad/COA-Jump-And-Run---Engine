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


#ifndef COAJNR_COMPONENT_INCLUDED
#define COAJNR_COMPONENT_INCLUDED

#include "Dependencies.h"
#include "IdManager.h"

namespace CoAJnR
{

class ComponentContainer;

/**
 * Adds functionality and behaviour to ComponentContainer objects.
 *
 * Component object may depend on other Component objects attached to the same
 * ComponentContainer.
 *
 * A Component has to implement 3 methodes: 
 * defaultName(), attachedCallback() and detachedCallback()
 */
class Component
{
    friend class ComponentContainer;

    /// whether the Component is attached and initialized at a
    /// ComponentContainer
    bool m_ready;

    /// whether the Component is currently attached to a ComponentContainer.
    bool m_attached;
    
    /// a locally unique object id
    LocalId m_localId;
    
protected:
    /// the parent ComponentContainer this Component belongs to.
    boost::shared_ptr<ComponentContainer> m_parent;
    
    /**
     * returns a list of Component names, this Component depends on.
     */
    virtual std::vector<std::string> dependencies()
    {
        return std::vector<std::string>();
    }
    
    /**
     * callback called by the ComponentContainer, after this Component was
     * attached to it.
     *
     * @note implement your initialization code in here.
     */
    virtual void attachedCallback() = 0;
    
    /**
     * callback called by the ComponentContainer, after this Component was
     * detached from it.
     *
     * @note write your destruction code in here.
     */
    virtual void detachedCallback() = 0;

    /**
     * ComponentContainer calls this function to indicate that this 
     * Component is ready to be used.
     */
    void setReady(bool p_ready = true) { m_ready = p_ready; }
    

    /**
     * ComponentContainer calls this function to indicate that this Component is
     * currently attached to it.
     */
    void setAttached(bool p_attached = true) { m_attached = p_attached; }
    
    /**
     * ComponentContainer calls this to set itself as parent object.
     */
    void setParent(boost::shared_ptr<ComponentContainer> parent)
    {
        m_parent = parent;
    }
    
public:
    /**
     * ctor.
     */
    Component() : 
            m_ready(false), 
            m_attached(false),
            m_localId(IdManager::get().newLocalId()) { }

    /**
     * dtor.
     * 
     * @note do not delete Component objects by hand. boost::shared_ptr will
     * handle that for us.
     */
    virtual ~Component()
    { 
    }

    /**
     * whether the Component can be used.
     */
    bool ready() { return m_ready; }
    
    /**
     * whether this Component is attached to a ComponentContainer.
     */
    bool attached() { return m_attached; }
    
    /**
     * returns a pointer to the parent ComponentContainer, or 0.
     */
    boost::shared_ptr<ComponentContainer> parent() { return m_parent; }

    /**
     * returns the default identifier of this Component type.
     *
     * this name is used by ComponentContainer objects and allows a mapping
     * from name to instance pointer.
     */
    virtual std::string defaultName() const = 0;
    
    /**
     * returns a locally unique identifier for this Component freely usable.
     */
    LocalId localId() const { return m_localId; }
};

/// memory managed shared pointer to a Component
typedef boost::shared_ptr<Component> ComponentPtr;

}

#endif

