/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//EntityPage.cpp
//EntityPage is an extension to PagedGeometry which displays entities as entities (no optimization techniques).
//-------------------------------------------------------------------------------------

#define __EntityPage_CPP__
#include "EntityPage.h"

#include "OgreRoot.h"
#include "OgreCamera.h"
#include "OgreVector3.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
using namespace Ogre;


//-------------------------------------------------------------------------------------

unsigned long EntityPage::GUID = 0;


void EntityPage::init(PagedGeometry *geom)
{
	sceneMgr = geom->getSceneManager();
	entPool = EntityPool::getEntityPool(sceneMgr);
	rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
}

EntityPage::EntityPage()
	: sceneMgr(NULL)
{}

EntityPage::~EntityPage()
{
	EntityPool::deleteAll();
	sceneMgr->destroySceneNode(rootNode->getName());
}


void EntityPage::addEntity(Entity *ent, const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
{
	//Create new entity
	SceneNode *node = entPool->create(rootNode, ent);
	entityList.push_back(node);

	//Position the entity
	node->setPosition(position);
	node->setOrientation(rotation);
	node->setScale(scale);
}

void EntityPage::removeEntities()
{
	//Destroy all entities
	std::vector<SceneNode *>::iterator iter;
	for (iter = entityList.begin(); iter != entityList.end(); iter++){
		entPool->destroy(*iter);
	}
	entityList.clear();
}


void EntityPage::setVisible(bool visible)
{
	rootNode->setVisible(visible);
}



//-------------------------------------------------------------------------------------

unsigned long EntityPool::GUID = 0;
std::map<SceneManager *, EntityPool *> EntityPool::selfList;


EntityPool *EntityPool::getEntityPool(SceneManager *mgr)
{
	//Search for an existing EntityPool for this scene manager
	std::map<SceneManager *, EntityPool *>::iterator iter;
	iter = selfList.find(mgr);

	//If found..
	if (iter != selfList.end()){
		//Return it
		return iter->second;
	} else {
		//Otherwise, return a new EntityPool
		return (new EntityPool(mgr));
	}
}

void EntityPool::deleteAll()
{
	while (selfList.size() > 0){
		EntityPool *pool = selfList.begin()->second;
		delete pool;
	}
}

EntityPool::EntityPool(SceneManager *mgr)
{
	sceneMgr = mgr;

	//Add self to selfList
	typedef std::pair<SceneManager *, EntityPool *> ListItem;
	selfList.insert(ListItem(sceneMgr, this));
}

EntityPool::~EntityPool()
{
	//Delete all entities
	if (sceneMgr != 0)
	{
		std::map<ResourceHandle, std::deque<SceneNode *> *>::iterator iter;
		for (iter = poolList.begin(); iter != poolList.end(); iter++)
		{
			std::deque<SceneNode *> *nodeList = iter->second;
			std::deque<SceneNode *>::iterator ent_iter;
			for (ent_iter = nodeList->begin(); ent_iter != nodeList->end(); ent_iter++)
			{
				Ogre::Entity *ent = static_cast<Entity*>((*ent_iter)->getAttachedObject(0));
				(*ent_iter)->detachAllObjects();
				// destroy attached entity
				sceneMgr->destroyEntity(ent);
				// delete scene node too
				sceneMgr->destroySceneNode((*ent_iter)->getName());
			}
			delete nodeList;
		}
	}

	//Remove self from selfList
	selfList.erase(sceneMgr);
}

SceneNode *EntityPool::create(SceneNode *parent, Entity *entity)
{
	//Use an existing pool if possible, or create a new one if necessary
	std::map<ResourceHandle, std::deque<SceneNode *> *>::iterator iter;
	iter = poolList.find(entity->getMesh()->getHandle());

	std::deque<SceneNode *> *nodeList;
	if (iter != poolList.end()){
		//Get the appropriate pool (entityList) for this entity
		nodeList = iter->second;
	} else {
		//Otherwise, create a new pool (entityList) for this entity
		nodeList = new std::deque<SceneNode *>;

		typedef std::pair<ResourceHandle, std::deque<SceneNode *> *> PoolItem;
		poolList.insert(PoolItem(entity->getMesh()->getHandle(), nodeList));
	}

	//Now, in the pool, use an existing entity if possible, or create a new one if necessary
	SceneNode *node;
	if (!nodeList->empty()){
		node = nodeList->front();	//Get entity from pool
		parent->addChild(node);		//Insert it into the scene
		nodeList->pop_front();	//Remove entity from pool list
	} else {
		//Pool some entities (in case some are needer later on)
		Entity *ent;
		for (int i = 0; i < 10; i++){
			ent = entity->clone(getUniqueID("PooledEntity"));
			node = sceneMgr->createSceneNode();
			node->attachObject(ent);
			nodeList->push_back(node);
		}

		//Create new entity (to be used now)
		ent = entity->clone(getUniqueID("PooledEntity"));
		node = parent->createChildSceneNode();
		node->attachObject(ent);
	}

	//Return the new entity
	return node;
}

void EntityPool::destroy(SceneNode *entityNode)
{
	//Find the pool where the entity belongs
	Entity *ent = (Entity*)(entityNode->getAttachedObject(0));
	std::map<ResourceHandle, std::deque<SceneNode *> *>::iterator iter;
	iter = poolList.find(ent->getMesh()->getHandle());

	std::deque<SceneNode *> *nodeList;
	if (iter != poolList.end()){
		//Get the appropriate pool (entityList) for this entity
		nodeList = iter->second;
	} else {
		//Otherwise, create a new pool (entityList) for this entity
		nodeList = new std::deque<SceneNode *>;

		typedef std::pair<ResourceHandle, std::deque<SceneNode *> *> PoolItem;
		poolList.insert(PoolItem(ent->getMesh()->getHandle(), nodeList));
	}

	//Remove the entity from the scene
	entityNode->getParentSceneNode()->removeChild(entityNode->getName());

	//Now insert the entity back into the pool
	nodeList->push_back(entityNode);
}
