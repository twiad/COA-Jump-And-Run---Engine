/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

#include "TreeLoader2D.h"
#include "PagedGeometry.h"

#include "OgreRoot.h"
#include "OgreException.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
using namespace Ogre;


TreeLoader2D::TreeLoader2D(PagedGeometry *geom, const TRect<Real> &bounds)
{
	//Calcualte grid size
	TreeLoader2D::geom = geom;
	pageSize = geom->getPageSize();

	//Reset height function
	heightFunction = NULL;

	//Make sure the bounds are aligned with PagedGeometry's grid, so the TreeLoader's grid tiles will have a 1:1 relationship
	actualBounds = bounds;
	gridBounds = bounds;
	gridBounds.left = pageSize * Math::Floor((gridBounds.left - geom->getBounds().left) / pageSize) + geom->getBounds().left;
	gridBounds.top = pageSize * Math::Floor((gridBounds.top - geom->getBounds().top) / pageSize) + geom->getBounds().top;
	gridBounds.right = pageSize * Math::Ceil((gridBounds.right - geom->getBounds().left) / pageSize) + geom->getBounds().left;
	gridBounds.bottom = pageSize * Math::Ceil((gridBounds.bottom - geom->getBounds().top) / pageSize) + geom->getBounds().top;

	//Calculate page grid size
	pageGridX = (gridBounds.width() / pageSize) + 1;
	pageGridZ = (gridBounds.height() / pageSize) + 1;
}

TreeLoader2D::~TreeLoader2D()
{
	//Delete all page grids
	PageGridListIterator i;
	for (i = pageGridList.begin(); i != pageGridList.end(); ++i){
		delete[] i->second;
	}
	pageGridList.clear();
}

void TreeLoader2D::addTree(Entity *entity, const Vector2 &position, Degree yaw, Real scale)
{
	Real x = position.x;
	Real z = position.y;

	//Check that the tree is within bounds (DEBUG)
	#ifdef _DEBUG
	if (x < actualBounds.left || x > actualBounds.right || z < actualBounds.top || z > actualBounds.bottom){
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
				"Tree position is out of bounds",
				"TreeLoader::addTree()");
	}
	#endif

	//Find the appropriate page grid for the entity
	PageGridListIterator i;
	i = pageGridList.find(entity);

	std::vector<TreeDef> *pageGrid;
	if (i != pageGridList.end()){
		//If it exists, get the page grid
		pageGrid = i->second;
	} else {
		//If it does not exist, create a new page grid
		pageGrid = new std::vector<TreeDef>[pageGridX * pageGridZ];

		//Register the new page grid in the pageGridList for later retrieval
		pageGridList.insert(PageGridListValue(entity, pageGrid));
	}

	//Calculate the gridbounds-relative position of the tree
	float xrel = x - gridBounds.left;
	float zrel = z - gridBounds.top;

	//Get the appropriate grid element based on the new tree's position
	int pageX = Math::Floor(xrel / pageSize);
	int pageZ = Math::Floor(zrel / pageSize);
	std::vector<TreeDef> &treeList = _getGridPage(pageGrid, pageX, pageZ);

	//Create the new tree
	TreeDef tree;
	tree.xPos = 65535 * (xrel - (pageX * pageSize)) / pageSize;
	tree.zPos = 65535 * (zrel - (pageZ * pageSize)) / pageSize;
	tree.rotation = 255 * (yaw.valueDegrees() / 360.0f);
	tree.scale = 255 * (scale / 2.0f);

	//Add it to the tree list
	treeList.push_back(tree);

	//Rebuild geometry if necessary
	geom->reloadGeometryPage(Vector3(x, 0, z));
}

void TreeLoader2D::deleteTrees(Real x, Real z, Real radius, Entity *type)
{
	//Determine the grid blocks which might contain the requested trees
	int minPageX = Math::Floor(((x-radius) - gridBounds.left) / pageSize);
	int minPageZ = Math::Floor(((z-radius) - gridBounds.top) / pageSize);
	int maxPageX = Math::Floor(((x+radius) - gridBounds.left) / pageSize);
	int maxPageZ = Math::Floor(((z+radius) - gridBounds.top) / pageSize);
	Real radiusSq = radius * radius;

	PageGridListIterator it, end;
	if (type == NULL){
		//Scan all entity types
		it = pageGridList.begin();
		end = pageGridList.end();
	} else {
		//Only scan entities of the given type
		it = pageGridList.find(type);
		assert(it != pageGridList.end());
		end = ++it;
	}

	//Scan all the grid blocks
	while (it != end){
		std::vector<TreeDef> *pageGrid = it->second;

		for (int tileZ = minPageZ; tileZ <= maxPageZ; ++tileZ){
			for (int tileX = minPageX; tileX <= maxPageX; ++tileX){
				bool modified = false;

				//Scan all trees in grid block
				std::vector<TreeDef> &treeList = _getGridPage(pageGrid, tileX, tileZ);
				std::vector<TreeDef>::iterator i;
				for (i = treeList.begin(); i != treeList.end(); ){
					//Get tree distance
					float distX = (gridBounds.left + (tileX * pageSize) + ((Real)i->xPos / 65535) * pageSize) - x;
					float distZ = (gridBounds.top + (tileZ * pageSize) + ((Real)i->zPos / 65535) * pageSize) - z;
					float distSq = distX * distX + distZ * distZ;

					if (distSq <= radiusSq){
						//If it's within the radius, delete it
						*i = treeList.back();
						treeList.pop_back();
						modified = true;
					}
					else
						++i;
				}

				//Rebuild geometry if necessary
				if (modified){
					Vector3 pos(gridBounds.left + ((0.5f + tileX) * pageSize), 0, gridBounds.top + ((0.5f + tileZ) * pageSize));
					geom->reloadGeometryPage(pos);
				}
			}
		}

		++it;
	}
}


void TreeLoader2D::loadPage(PageInfo &page)
{
	//Calculate x/z indexes for the grid array
	page.xIndex -= Math::Floor(gridBounds.left / pageSize);
	page.zIndex -= Math::Floor(gridBounds.top / pageSize);

	//Check if the requested page is in bounds
	if (page.xIndex < 0 || page.zIndex < 0 || page.xIndex >= pageGridX || page.zIndex >= pageGridZ)
		return;

	//For each tree type...
	PageGridListIterator i;
	for (i = pageGridList.begin(); i != pageGridList.end(); ++i){
		//Get the appropriate tree list for the specified page
		std::vector<TreeDef> *pageGrid = i->second;
		std::vector<TreeDef> &treeList = _getGridPage(pageGrid, page.xIndex, page.zIndex);
		Entity *entity = i->first;

		//Load the listed trees into the page
		std::vector<TreeDef>::iterator o;
		for (o = treeList.begin(); o != treeList.end(); ++o){
			//Get position
			Vector3 pos;
			pos.x = page.bounds.left + ((Real)o->xPos / 65535) * pageSize;
			pos.z = page.bounds.top + ((Real)o->zPos / 65535) * pageSize;

			//Calculate terrain height at pos.x / pos.z to get pos.y
			if (heightFunction != NULL)
				pos.y = heightFunction(pos.x, pos.z);
			else
				pos.y = 0.0f;
			
			//Get rotation
			Degree angle((Real)o->rotation * (360.0f / 255));
			Quaternion rot(angle, Vector3::UNIT_Y);

			//Get scale
			Vector3 scale;
			scale.y = (Real)o->scale * (2.0f / 255);
			scale.x = scale.y;
			scale.z = scale.y;

			addEntity(entity, pos, rot, scale);
		}
	}
}

TreeIterator2D TreeLoader2D::getTrees()
{
	return TreeIterator2D(this);
}



TreeIterator2D::TreeIterator2D(TreeLoader2D *trees)
{
	TreeIterator2D::trees = trees;

	//Setup iterators
	currentGrid = trees->pageGridList.begin();
	currentX = 0; currentZ = 0;
	currentTreeList = &trees->_getGridPage(currentGrid->second, currentX, currentZ);
	currentTree = currentTreeList->begin();
	hasMore = true;

	//If there's not tree in the first page, keep looking
	if (currentTree == currentTreeList->end())
		moveNext();

	//Read the first tree's data
	_readTree();

	//Read one more tree, so peekNext() will properly return the first item, while the system
	//actually is looking ahead one item (this way it can detect the end of the list before
	//it's too late)
	if (hasMore)
		moveNext();
}

TreeRef TreeIterator2D::getNext()
{
	TreeRef tree = peekNext();
	moveNext();
	return tree;
}

void TreeIterator2D::moveNext()
{
	//Out of bounds check
	if (!hasMore)
		OGRE_EXCEPT(1, "Cannot read past end of TreeIterator list", "TreeIterator::moveNext()");

	//Preserve the last tree
	prevTreeDat = currentTreeDat;

	//Incriment iterators to the next tree
	++currentTree;
	while (currentTree == currentTreeList->end()){
		if (++currentX >= trees->pageGridX){
			currentX = 0;
			if (++currentZ >= trees->pageGridZ){
				++currentGrid;
				if (currentGrid == trees->pageGridList.end()){
					//No more trees left
					hasMore = false;
					return;
				}
				currentX = 0; currentZ = 0;
			}
		}

		currentTreeList = &trees->_getGridPage(currentGrid->second, currentX, currentZ);
		currentTree = currentTreeList->begin();
	}

	//Read the current tree data
	_readTree();
}

void TreeIterator2D::_readTree()
{
	TreeLoader2D::TreeDef treeDef = *currentTree;

	//Get position
	Real boundsLeft = trees->gridBounds.left + trees->pageSize * currentX;
	Real boundsTop = trees->gridBounds.top + trees->pageSize * currentZ;
	currentTreeDat.position.x = boundsLeft + ((Real)treeDef.xPos / 65535) * trees->pageSize;
	currentTreeDat.position.z = boundsTop + ((Real)treeDef.zPos / 65535) * trees->pageSize;

	//Calculate terrain height at x / z to get y
	if (trees->heightFunction != NULL)
		currentTreeDat.position.y = trees->heightFunction(currentTreeDat.position.x, currentTreeDat.position.z);
	else
		currentTreeDat.position.y = 0.0f;
	
	//Get rotation
	currentTreeDat.yaw = Degree((Real)treeDef.rotation * (360.0f / 255));

	//Get scale
	currentTreeDat.scale = (Real)treeDef.scale * (2.0f / 255);

	//Get entity
	currentTreeDat.entity = currentGrid->first;
}
