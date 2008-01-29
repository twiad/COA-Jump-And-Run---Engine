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

#include "ContentManager.h"
#include "DataStorage.h"

namespace CoAJnR
{

ContentManager::ContentManager()
{
    
}

ContentManager::~ContentManager()
{
    
}

ComponentContainerPtr 
ContentManager::createEmptyContainer()
{
    ComponentContainerPtr obj(new ComponentContainer());
    DataStorage::get().add(obj);
    addToGroup(obj);
    return obj;
}

ComponentContainerPtr 
ContentManager::createStaticObject(
        const std::string& p_meshName,
        const Ogre::Vector3& p_initialPosition,
        const Ogre::Quaternion& p_initialOrientation,
        PhysicsShape p_shape)
{
    ComponentContainerPtr obj = createEmptyContainer();
    
    obj->attach(PositionComponentPtr(
            new PositionComponent(p_initialPosition, p_initialOrientation)));

    obj->attach(GraphicsStaticMeshComponentPtr(
            new GraphicsStaticMeshComponent(p_meshName)));
    
    obj->attach(PhysicsComponentPtr(new PhysicsComponent(p_shape)));

    return obj;
}

ComponentContainerPtr 
ContentManager::createDynamicObject(
        const std::string& p_meshName,
        const Ogre::Vector3& p_initialPosition,
        const Ogre::Quaternion& p_initialOrientation,
        double p_mass,
        PhysicsShape p_shape)
{
    ComponentContainerPtr obj = createEmptyContainer();

    obj->attach(PositionComponentPtr(
            new PositionComponent(p_initialPosition, p_initialOrientation)));

    obj->attach(GraphicsMeshComponentPtr(
            new GraphicsMeshComponent(p_meshName)));
    
    obj->attach(PhysicsComponentPtr(new PhysicsComponent(
            p_shape, 
            p_mass)));
    
    return obj;
}

void 
ContentManager::destroyObject(ComponentContainerPtr p_obj)
{
    assert(p_obj && "obj must not be null");
    
    if(p_obj->groupName().size())
        removeFromGroup(p_obj, p_obj->groupName());
    
    p_obj->detachAll();
    DataStorage::get().remove(p_obj->id());
}

void 
ContentManager::addToGroup(
        ComponentContainerPtr p_obj, 
        std::string p_group)
{
    if(p_obj->groupName().size())
        removeFromGroup(p_obj, p_obj->groupName());

    boost::mutex::scoped_lock lock(m_groupMutex);

    m_groups[p_group].push_back(p_obj);

    p_obj->setGroupName(p_group);

}

void 
ContentManager::removeFromGroup(
        ComponentContainerPtr p_obj, 
        std::string p_group)
{
    boost::mutex::scoped_lock lock(m_groupMutex);
    
    std::vector<ComponentContainerPtr>& group = m_groups[p_group];
    
    std::vector<ComponentContainerPtr>::iterator it;
    for(it = group.begin(); it != group.end(); it++)
        if(*it == p_obj)
        {
            group.erase(it);
            p_obj->setGroupName(std::string());
            break;
        }
}

void 
ContentManager::removeGroup(std::string p_group)
{
    boost::mutex::scoped_lock lock(m_groupMutex);

    std::map<std::string, std::vector<ComponentContainerPtr> >::iterator mIt = 
            m_groups.find(p_group);

    if(mIt == m_groups.end())
        return;

    std::vector<ComponentContainerPtr>& group = (*mIt).second;
    
    std::vector<ComponentContainerPtr>::iterator it;
    for(it = group.begin(); it != group.end(); it++)
    {
        (*it)->detachAll();
    }
    group.clear();
    
    m_groups.erase(mIt);
}

void
ContentManager::removeAllGroups()
{
    boost::mutex::scoped_lock lock(m_groupMutex);

    std::map<std::string, std::vector<ComponentContainerPtr> >::iterator mIt;
    for(mIt = m_groups.begin(); mIt != m_groups.end(); mIt++)
    {
        std::vector<ComponentContainerPtr>& group = (*mIt).second;

        std::vector<ComponentContainerPtr>::iterator it;
        for(it = group.begin(); it != group.end(); it++)
            (*it)->detachAll();

        group.clear();
        
    }

    m_groups.clear();
}

}
