/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

#ifndef __TreeLoader3D_H__
#define __TreeLoader3D_H__

#include "PagedGeometry.h"

#include "OgrePrerequisites.h"


class TreeIterator3D;
class TreeIterator2D;

/** \brief A PageLoader-derived object you can use with PagedGeometry to easily place trees on your terrain. 

\note TreeLoader3D is derived from PageLoader - this implementation provides you with an easy way
to add trees to your scene. Remember that if the included PageLoader's aren't enough, you can easily
create your own to load geometry any way you want. You could even modify existing an PageLoader
to suit your needs.

Using a TreeLoader is simple - simply create an instance, attach it to your PagedGeometry object
with PagedGeometry::setPageLoader(), and add your trees.

To add trees, just call TreeLoader3D::addTree(), supplying the appropriate parameters. You may notice that
TreeLoader3D restricts trees to uniform scale and yaw rotation. This is done to conserve memory; TreeLoader3D
packs trees into memory as effeciently as possible, taking only 10 bytes per tree. This means 1 million trees
takes 9.53 MB of RAM (additionally, adding 1 million trees takes less than a second in a release build).

\note Using TreeLoader2D uses 40% less memory than TreeLoader3D.
*/
class TreeLoader3D: public PageLoader
{
public:
	/** \brief Creates a new TreeLoader3D object.
	\param geom The PagedGeometry object that this GrassLoader will be assigned to.
	\param bounds The rectangular boundary in which all trees will be placed. */
	TreeLoader3D(PagedGeometry *geom, const Ogre::TRect<Ogre::Real> &bounds);
	~TreeLoader3D();

	/** \brief Adds an entity to the scene with the specified location, rotation, and scale.
	\param entity The entity to be added to the scene.
	\param position The desired position of the tree
	\param yaw The desired rotation around the Y axis in degrees
	\param scale The desired scale of the entity

	While TreeLoader3D allows you to provide full 3-dimensional x/y/z coordinates,
	you are restricted to only yaw rotation, and only uniform scale. */
	void addTree(Ogre::Entity *entity, const Ogre::Vector3 &position, Ogre::Degree yaw = Ogre::Degree(0), Ogre::Real scale = 1.0f);

	/** \brief Deletes trees within a certain radius of the given coordinates.
	\param x The x coordinate of the tree(s) to delete
	\param z The z coordinate of the tree(s) to delete
	\param radius The radius from the coordinates where trees will be deleted (optional)
	\param type The type of tree to delete (optional)

	\note If the "type" parameter is set to an entity, only trees created with that entity
	will be deleted. */
	void deleteTrees(Ogre::Real x, Ogre::Real z, float radius, Ogre::Entity *type = NULL);

	/** \brief Gets an iterator which can be used to access all added trees.

	The returned TreeIterator3D can be used to iterate through every tree that was added
	to this TreeLoader3D fairly efficiently.

	\see The TreeIterator class documentation for more info.
	*/
	TreeIterator3D getTrees();


	void loadPage(PageInfo &page);

private:
	friend class TreeIterator3D;

	struct TreeDef
	{
		float yPos;
		Ogre::uint16 xPos, zPos;
		Ogre::uint8 scale, rotation;
	};

	//Information about the 2D grid of pages
	int pageGridX, pageGridZ;
	Ogre::Real pageSize;
	Ogre::TRect<Ogre::Real> gridBounds, actualBounds;

	//Misc.
	PagedGeometry *geom;

	//A std::map of 2D grid arrays. Each array is the same size (pageGridX x pageGridZ), and
	//contains a std::vector list of trees. Each std::map entry corresponds to a single tree
	//type, and every tree defined in the std::map entry's grid should be of that tree type.
	std::map<Ogre::Entity*, std::vector<TreeDef>*> pageGridList;
	typedef std::map<Ogre::Entity*, std::vector<TreeDef>*>::iterator PageGridListIterator;
	typedef std::pair<Ogre::Entity*, std::vector<TreeDef>*> PageGridListValue;

	inline std::vector<TreeDef> &_getGridPage(std::vector<TreeDef> *grid, int x, int z)
	{
		return grid[z * pageGridX + x];
	}

	inline void _setGridPage(std::vector<TreeDef> *grid, int x, int z, const std::vector<TreeDef> &page)
	{
		grid[z * pageGridX + x] = page;
	}
};



//The TreeRef class is used by both TreeLoader2D and TreeLoader3D.
//This #ifndef makes sure TreeRef isn't declared twice in case both treeloaders are used.
#ifndef TreeRef_Declared
#define TreeRef_Declared

class TreeRef
{
public:
	/** Returns the tree's position */
	inline Ogre::Vector3 &getPosition() { return position; }

	/** Returns the tree's yaw as a degree value */
	inline Ogre::Degree &getYaw() { return yaw; }

	/** Returns the tree's uniform scale value */
	inline Ogre::Real &getScale() { return scale; }

	/** Returns the tree's orientation as a Quaternion */
	inline Ogre::Quaternion getOrientation() { return Ogre::Quaternion(yaw, Ogre::Vector3::UNIT_Y); }

	/** Returns the entity used to create the tree */
	inline Ogre::Entity *getEntity() { return entity; }

private:
	friend class TreeIterator2D;
	friend class TreeIterator3D;
	Ogre::Vector3 position;
	Ogre::Degree yaw;
	Ogre::Real scale;
	Ogre::Entity *entity;
};

#endif


class TreeIterator3D
{
public:
	TreeIterator3D(TreeLoader3D *trees);

	/** Returns true if there are more trees available to be read */
	inline bool hasMoreElements() const { return hasMore; }

	/** Returns the next tree, and advances to the next */
	TreeRef getNext();

	/** Returns the next tree, without advancing to the next */
	inline TreeRef &peekNext() { return prevTreeDat; }

	/** Returns a pointer to the next tree, without advancing to the next */
	inline TreeRef *peekNextPtr() { return &prevTreeDat; }

	/** Moves the iterator on to the next tree */
	void moveNext();

private:
	void _readTree();

	TreeLoader3D *trees;
	TreeLoader3D::PageGridListIterator currentGrid;
	int currentX, currentZ;
	std::vector<TreeLoader3D::TreeDef> *currentTreeList;
	std::vector<TreeLoader3D::TreeDef>::iterator currentTree;

	TreeRef currentTreeDat, prevTreeDat;
	bool hasMore;
};

#endif
