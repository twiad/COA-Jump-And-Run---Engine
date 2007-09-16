/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//EntityPage.h
//EntityPage is an extension to PagedGeometry which displays entities as entities (no optimization techniques).
//-------------------------------------------------------------------------------------

#ifndef __EntityPage_H__
#define __EntityPage_H__

#include "PagedGeometry.h"

#include "OgrePrerequisites.h"
#include "OgreResource.h"
#include "OgreStringConverter.h"


class EntityPool;

/**
\brief The EntityPage class renders entities as entities (no optimization).

This is one of the geometry page types included in the StaticGeometry engine. These
page types should be added to a PagedGeometry object with PagedGeometry::addDetailLevel()
so the PagedGeometry will know how you want your geometry displayed.

To use this page type, use:
\code
PagedGeometry::addDetailLevel<EntityPage>(farRange);
\endcode

This page type uses plain Ogre entities to represent the given entities (in other words,
no real optimization is performed). The only reason you would really want to use this
is where you want to use features availible for entities, but not batched geometry, such
as shadows, animation, and per-entity LOD. Plain entities are much slower than the more
optimized methods like BatchPage and ImposterPage, so use it as minimally as possible.
Not only are entities slower rendering, but slower loading (this becomes more obvious
when the camera moves). Usually, EntityPage's should be used at very close view ranges,
while BatchPage's and ImposterPage's are used in the distance.

\warning EntityPage does not support fade transitions.
*/
class EntityPage: public GeometryPage
{
public:
	void init(PagedGeometry *geom);

	EntityPage();
	~EntityPage();

	void addEntity(Ogre::Entity *ent, const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);
	void removeEntities();

	void setVisible(bool visible);
	void setFade(bool enabled, Ogre::Real visibleDist, Ogre::Real invisibleDist)
	{
		#ifndef __EntityPage_CPP__
		#define __STR2__(x) #x
		#define __STR1__(x) __STR2__(x)
		#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : warning: "
		#pragma message(__LOC__"EntityPage does not support fade transitions.")
		#endif
	}

private:
	Ogre::SceneManager *sceneMgr;	//Scene manager
	EntityPool *entPool;	//Pooled entity class

	Ogre::SceneNode *rootNode;	//Scene node containing entities
	std::vector<Ogre::SceneNode *> entityList;	//List of used entities

	static unsigned long GUID;
	static inline Ogre::String getUniqueID(const Ogre::String &prefix)
	{
		return prefix + Ogre::StringConverter::toString(++GUID);
	}
};

//An entity pool allows many entities to be "virtually" created/destroyed extremely fast.
//This not only creates entities, but creates scene nodes and attaches the entities to them.
class EntityPool
{
public:
	static EntityPool *getEntityPool(Ogre::SceneManager *mgr);
	static void deleteAll();
	~EntityPool();

	Ogre::SceneNode *create(Ogre::SceneNode *parent, Ogre::Entity *entity);
	void destroy(Ogre::SceneNode *entityNode);

protected:
	EntityPool(Ogre::SceneManager *mgr);

	static std::map<Ogre::SceneManager *, EntityPool *> selfList;	//List of EntityPool's
	Ogre::SceneManager *sceneMgr;

	//A list of pools (where a pool is a list of scene nodes)
	std::map<Ogre::ResourceHandle, std::deque<Ogre::SceneNode *> *> poolList;

	static unsigned long GUID;
	static inline Ogre::String getUniqueID(const Ogre::String &prefix)
	{
		return prefix + Ogre::StringConverter::toString(++GUID);
	}
};


#endif
