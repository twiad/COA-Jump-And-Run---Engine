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


#ifndef COAJNR_COMPONENTCONTAINER_INCLUDED
#define COAJNR_COMPONENTCONTAINER_INCLUDED

#include "Dependencies.h"

#include "Component.h"
#include "DataObject.h"

namespace CoAJnR
{

/**
 * object places in the scenery.
 */
class ComponentContainer : 
        public DataObject,
        public boost::enable_shared_from_this<ComponentContainer>

{
    /// mutex protecting the list of Component instances of this object
    mutable boost::mutex m_componentListMutex;

    mutable boost::mutex m_dependencyListMutex;

    typedef std::map<std::string, ComponentPtr> ComponentListType;
    typedef ComponentListType::iterator ComponentListItrType;
    typedef ComponentListType::const_iterator ComponentListCItrType;
    
    /// the list of Component instances attached to this object
    ComponentListType m_componentList;
    
    /// for every component a list of components which need it.
    typedef std::map<std::string, std::vector<std::string> > 
            ComponentDependencyListType;
    typedef ComponentDependencyListType::iterator 
            ComponentDependencyListItrType;
    typedef ComponentDependencyListType::const_iterator
            ComponentDependencyListCItrType;
            
    ComponentDependencyListType m_dependencyList;
    
    std::string findComponentForRemoval();

    bool m_detachAllProcess;
    
    mutable boost::mutex m_groupNameMutex;
    std::string m_groupName;
    
    ComponentPtr detachInternal(const std::string& componentIdentifier);
    

public:
    /**
     * ctor.
     */
    ComponentContainer();

    /**
     * dtor.
     */
    virtual ~ComponentContainer();

    /**
     * returns a Component of this ComponentContainer, 
     * 0 if Component not present.
     */
    ComponentPtr component(const std::string& componentIdentifier) const;

    template<class T>
    boost::shared_ptr<T> component(const std::string& componentIdentifier) const
    {
        return boost::dynamic_pointer_cast<T>(component(componentIdentifier));
    }

    /**
     * attaches a Component to this ComponentContainer and 
     * calls Component::attached().
     */
    bool attach(
             ComponentPtr component, 
             const std::string& componentIdentifier = std::string());

    /**
     * detaches and destroys a Component and calls Component::detached().
     */
    bool detach(const std::string& componentIdentifier);

    /**
     * detaches and destroys all Components from this ComponentContainer.
     *
     * @note while this is in progress, attach() and detach() will return false.
     */
    void detachAll();

    void componentAttachedCallback(ComponentPtr component);
    void componentDetachedCallback(ComponentPtr component);

    std::string groupName() const;
    void setGroupName(std::string);
};

typedef boost::shared_ptr<ComponentContainer> ComponentContainerPtr;

}

#endif

