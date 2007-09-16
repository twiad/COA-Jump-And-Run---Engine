/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

#ifndef __TreeLoader2D_H__
#define __TreeLoader2D_H__

#include "PagedGeometry.h"

#include "OgrePrerequisites.h"


class TreeIterator3D;
class TreeIterator2D;

/** \brief A PageLoader-derived object you can use with PagedGeometry to easily place trees on your terrain. 

\note TreeLoader2D is derived from PageLoader - this implementation provides you with an easy way
to add trees to your scene. Remember that if the included PageLoader's aren't enough, you can easily
create your own to load geometry any way you want. You could even modify existing an PageLoader
to suit your needs.

Using a TreeLoader is simple - simply create an instance, attach it to your PagedGeometry object
with PagedGeometry::setPageLoader(), and add your trees.

To add trees, just call TreeLoader2D::addTree(), supplying the appropriate parameters. You may notice that
TreeLoader2D restricts trees to uniform scale, x/z position, and yaw rotation. This is done to conserve
memory; TreeLoader2D packs trees into memory as effeciently as possible, taking only 6 bytes per tree. This
means 1 million trees only takes 5.72 MB of RAM (additionally, adding 1 million trees takes less than a
second in a release build).

\note By default, TreeLoader2D doesn't know what shape your terrain is, so all trees will be placed
at 0 height. To inform TreeLoader2D of the shape of your terrain, you must specify a height function
that returns the height (y coordinate) of your terrain at the given x and z coordinates. See
the TreeLoader2D::setHeightFunction() documentation for more information.

\warning If you attempt to use Ogre's scene queries to get the terrain height,
keep in mind that calculating the height of Ogre's built-in terrain this way can
be VERY slow if not done properly, and may cause stuttering due to long paging delays.

If using only x/z positions are too limiting, or you are unable to implement a fast enough height function,
refer to TreeLoader3D. TreeLoader3D allows you to provide full 3D x/y/z coordinates, although 40% more memory
is required per tree.
*/
class TreeLoader2D: public PageLoader
{
public:
	/** \brief Creates a new TreeLoader object.
	\param geom The PagedGeometry object that this GrassLoader will be assigned to.
	\param bounds The rectangular boundary in which all trees will be placed. */
	TreeLoader2D(PagedGeometry *geom, const Ogre::TRect<Ogre::Real> &bounds);
	~TreeLoader2D();
	
	/** \brief Adds an entity to the scene with the specified location, rotation, and scale.
	\param entity The entity to be added to the scene.
	\param position The 2-dimensional x/z position of the tree
	\param yaw The desired rotation around the Y axis in degrees
	\param scale The desired scale of the entity

	Trees are restricted to x/z positions, only yaw rotation, and only uniform scale.
	This conserves memory by avoiding storage of useless data (for example, storing the
	height of each tree when this can be easily calculated from the terrain's shape).

	The y position of each tree is calculated with a height function, which you must
	specify with setHeightFunction() (otherwise all trees will be placed at 0 height). */
	void addTree(Ogre::Entity *entity, const Ogre::Vector2 &position, Ogre::Degree yaw = Ogre::Degree(0), Ogre::Real scale = 1.0f);

	/** \brief Deletes trees within a certain radius of the given coordinates.
	\param x The x coordinate of the tree(s) to delete
	\param z The z coordinate of the tree(s) to delete
	\param radius The radius from the coordinates where trees will be deleted (optional)
	\param type The type of tree to delete (optional)

	\note If the "type" parameter is set to an entity, only trees created with that entity
	will be deleted. */
	void deleteTrees(Ogre::Real x, Ogre::Real z, float radius, Ogre::Entity *type = NULL);

	/** \brief Sets the height function used to calculate tree Y coordinates
	\param heightFunction A pointer to a height function

	Unless you want all your trees placed at 0 height, you need to specify a height function
	so TreeLoader2D will be able to calculate the Y coordinate. The height function given
	to setHeightFunction() should use the following prototype (although you can name the
	function anything you want):

	\code
	Real getHeightAt(Real x, Real z);
	\endcode

	After you've defined a height function, using setHeightFunction is easy:
	
	\code
	pageLoader2D->setHeightFunction(&getHeightAt);
	\endcode
	*/
	void setHeightFunction(Ogre::Real (*heightFunction)(Ogre::Real x, Ogre::Real z)) { this->heightFunction = heightFunction; }

	/** \brief Gets an iterator which can be used to access all added trees.
	
	The returned TreeIterator can be used to iterate through every tree that was added
	to this TreeLoader fairly efficiently.

	\see The TreeIterator class documentation for more info.
	*/
	TreeIterator2D getTrees();


	void loadPage(PageInfo &page);

private:
	friend class TreeIterator2D;

	struct TreeDef
	{
		Ogre::uint16 xPos, zPos;
		Ogre::uint8 scale, rotation;
	};

	//Information about the 2D grid of pages
	int pageGridX, pageGridZ;
	Ogre::Real pageSize;
	Ogre::TRect<Ogre::Real> gridBounds, actualBounds;

	//Height data
	Ogre::Real (*heightFunction)(Ogre::Real x, Ogre::Real z);	//Pointer to height function

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
	friend class TreeIterator3D;
	friend class TreeIterator2D;
	Ogre::Vector3 position;
	Ogre::Degree yaw;
	Ogre::Real scale;
	Ogre::Entity *entity;
};

#endif


class TreeIterator2D
{
public:
	TreeIterator2D(TreeLoader2D *trees);

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

	TreeLoader2D *trees;
	TreeLoader2D::PageGridListIterator currentGrid;
	int currentX, currentZ;
	std::vector<TreeLoader2D::TreeDef> *currentTreeList;
	std::vector<TreeLoader2D::TreeDef>::iterator currentTree;

	TreeRef currentTreeDat, prevTreeDat;
	bool hasMore;
};

#endif
