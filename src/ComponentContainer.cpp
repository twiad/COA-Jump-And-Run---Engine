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


#include "ComponentContainer.h"
#include "ThreadPool.h"

namespace CoAJnR
{

ComponentContainer::ComponentContainer() : m_detachAllProcess(false)
{

}

ComponentContainer::~ComponentContainer()
{
    std::cout << "componentcontainer dtor" << std::endl;
    
    boost::mutex::scoped_lock lock(m_componentListMutex);

    assert(m_componentList.size() == 0 && "container still has components");
}

ComponentPtr
ComponentContainer::component(const std::string& p_componentIdentifier) const
{
    boost::mutex::scoped_lock lock(m_componentListMutex);

    ComponentListCItrType it = m_componentList.find(p_componentIdentifier);

    assert(it != m_componentList.end() && "component not found");

    return (*it).second;
}

std::string
ComponentContainer::findComponentForRemoval()
{
    if(m_dependencyList.size() == 0)
        return std::string();

    ComponentDependencyListCItrType it;
    for(it = m_dependencyList.begin(); it != m_dependencyList.end(); it++)
        if((*it).second.size() == 0)
            return (*it).first;

    assert(false && "error! circular dependency.");

    return std::string();
}

bool
ComponentContainer::attach(
        ComponentPtr p_component,
        const std::string& p_componentIdentifier)
{
    assert(p_component && "component must not be null");

    if(m_detachAllProcess)
        return false;

    std::string componentIdentifier;
    
    if(p_componentIdentifier.empty())
        componentIdentifier = p_component->defaultName();
    else
        componentIdentifier = p_componentIdentifier;

    {
        boost::mutex::scoped_lock lock(m_componentListMutex);

        assert(m_componentList.find(componentIdentifier) ==
                m_componentList.end() && "double usage of component name");

        m_componentList[componentIdentifier] = p_component;
    }

    std::vector<std::string> dependencies = p_component->dependencies();

    {
        boost::mutex::scoped_lock lock(m_dependencyListMutex);

        std::vector<std::string>::const_iterator it;
        for(it = dependencies.begin(); it != dependencies.end(); it++)
        {
            assert(component(*it) && "component dependency error");

            m_dependencyList[*it].push_back(componentIdentifier);
        }

        m_dependencyList[componentIdentifier];
    }

    p_component->setParent(shared_from_this());
    p_component->setAttached();
    p_component->attachedCallback();

    return true;
}

bool
ComponentContainer::detach(const std::string& p_componentIdentifier)
{
    if(m_detachAllProcess)
        return false;

    boost::mutex::scoped_lock lock1(m_componentListMutex);
    boost::mutex::scoped_lock lock2(m_dependencyListMutex);
    detachInternal(p_componentIdentifier);

    return true;
}

ComponentPtr
ComponentContainer::detachInternal(const std::string& p_componentIdentifier)
{
    ComponentPtr component;

    {
        ComponentListItrType it = m_componentList.find(p_componentIdentifier);
        assert(it != m_componentList.end() && "component to remove not found");

        component = (*it).second;
        m_componentList.erase(it);
        component->setAttached(false);
    }

    {
        ComponentDependencyListItrType componentDeps =
                m_dependencyList.find(p_componentIdentifier);
        assert((*componentDeps).second.size() == 0 &&
                "cannot detach component, because others depend on it");

        m_dependencyList.erase(componentDeps);

        ComponentDependencyListItrType it;
        for(it = m_dependencyList.begin(); it != m_dependencyList.end(); it++)
        {
            std::vector<std::string>::iterator entry = (*it).second.begin();

            for(; entry != (*it).second.end(); entry++)
                if(*entry == p_componentIdentifier)
                {
                    (*it).second.erase(entry);
                    break;
                }
        }
    }

    /// @todo TODO: componentcontainer locked while detach callback in component
    /// creates deadlock when accessing parent() from component detachcb
    // ThreadPool::get().schedule(boost::bind(
    //         &Component::detachedCallback, component));
    component->detachedCallback();

    return component;
}

void
ComponentContainer::componentAttachedCallback(ComponentPtr p_component)
{
    assert(p_component && "component must not be null");
    p_component->setReady();
}

void
ComponentContainer::componentDetachedCallback(ComponentPtr p_component)
{
    assert(p_component && "component must not be null");

	// @todo TODO this must be done much earlier, but leads to stuck calls
    p_component->setReady(false);
    p_component->setParent(ComponentContainerPtr());
    
    /// @todo TODO: this line crashes on character anim comp detach
    if(m_detachAllProcess)
    {
        boost::mutex::scoped_lock lock1(m_componentListMutex);
        boost::mutex::scoped_lock lock2(m_dependencyListMutex);
        
        std::string nextToRemove = findComponentForRemoval();

        if(nextToRemove.empty())
        {
            m_detachAllProcess = false;
            return;
        }

        detachInternal(nextToRemove);
    }
}

void
ComponentContainer::detachAll()
{
    m_detachAllProcess = true;
    boost::mutex::scoped_lock lock1(m_componentListMutex);
    boost::mutex::scoped_lock lock2(m_dependencyListMutex);
    detachInternal(findComponentForRemoval());
}

std::string 
ComponentContainer::groupName() const
{
    boost::mutex::scoped_lock lock(m_groupNameMutex);
    return m_groupName;
}

void 
ComponentContainer::setGroupName(std::string p_groupName)
{
    boost::mutex::scoped_lock lock(m_groupNameMutex);
    m_groupName = p_groupName;
}

}
