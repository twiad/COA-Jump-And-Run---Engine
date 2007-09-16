/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//BatchPage.h
//BatchPage is an extension to PagedGeometry which displays entities as static geometry.
//-------------------------------------------------------------------------------------

#ifndef __BatchPage_H__
#define __BatchPage_H__

#include "PagedGeometry.h"
#include "BatchedGeometry.h"

#include "OgrePrerequisites.h"
#include "OgreStringConverter.h"


/**
\brief The BatchPage class renders entities as StaticGeometry.

This is one of the geometry page types included in the StaticGeometry engine. These
page types should be added to a PagedGeometry object with PagedGeometry::addDetailLevel()
so the PagedGeometry will know how you want your geometry displayed.

To use this page type, use:
\code
PagedGeometry::addDetailLevel<BatchPage>(farRange);
\endcode

This page type uses batched geometry (Ogre::StaticGeometry) to represent the entities.
Batched geometry is generally much faster than plain entities, since video card state
changes and transform calculations can be minimized. Batched geometry can be anywhere
from 2 to 20 times faster than plain entities.
*/
class BatchPage: public GeometryPage
{
public:
	void init(PagedGeometry *geom);
	~BatchPage();
	
	void addEntity(Ogre::Entity *ent, const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);
	void build();
	void removeEntities();

	void setVisible(bool visible);
	void setFade(bool enabled, Ogre::Real visibleDist, Ogre::Real invisibleDist);

	void addEntityToBoundingBox() {}
	void clearBoundingBox() {}
	const Ogre::AxisAlignedBox &getBoundingBox() { return batch->getBoundingBox(); }

private:
	Ogre::MaterialPtr getFadeMaterial(Ogre::MaterialPtr mat, Ogre::Real visibleDist, Ogre::Real invisibleDist);

	bool fadeEnabled;
	std::vector<Ogre::MaterialPtr> unfadedMaterials;

	typedef std::map<Ogre::String, Ogre::MaterialPtr> FadedMaterialMap;
	static FadedMaterialMap fadedMaterialMap;
	static unsigned long refCount;

	Ogre::SceneManager *sceneMgr;
	BatchedGeometry *batch;

	static unsigned long GUID;
	static inline Ogre::String getUniqueID(const Ogre::String &prefix)
	{
		return prefix + Ogre::StringConverter::toString(++GUID);
	}
};


#endif
