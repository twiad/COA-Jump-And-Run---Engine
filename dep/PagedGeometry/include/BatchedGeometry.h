/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//BatchedGeometry.h
//A "lightweight" version of Ogre::StaticGeometry, which gives you a little more control
//over the batch materials, etc.
//-------------------------------------------------------------------------------------

#ifndef __BatchedGeometry_H__
#define __BatchedGeometry_H__

#include "OgrePrerequisites.h"
#include "OgreMovableObject.h"
#include "OgreSceneNode.h"
#include "OgreMaterialManager.h"


class BatchedGeometry: public Ogre::MovableObject
{
public:
	BatchedGeometry(Ogre::SceneManager *mgr);
	~BatchedGeometry();

	// Dummy declaration of abstract virtual
	//void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false) { }
	
	void addEntity(Ogre::Entity *ent, const Ogre::Vector3 &position, const Ogre::Quaternion &orientation = Ogre::Quaternion::IDENTITY, const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);
	void build();
	void clear();

	void _notifyCurrentCamera(Ogre::Camera *cam);
	void _updateRenderQueue(Ogre::RenderQueue *queue);
	bool isVisible();
	const Ogre::AxisAlignedBox &getBoundingBox(void) const { return bounds; }
	Ogre::Real getBoundingRadius(void) const { return radius; }
	const Ogre::String &getMovableType(void) const { static Ogre::String t = "BatchedGeometry"; return t; }

	class SubBatch: public Ogre::Renderable
	{
	public:
		SubBatch(BatchedGeometry *parent, Ogre::SubMesh *mesh);
		~SubBatch();

		void addSubMesh(Ogre::SubMesh *mesh, const Ogre::Vector3 &position, const Ogre::Quaternion &orientation, const Ogre::Vector3 &scale);
		void build();
		void clear();
		
		void setMaterial(Ogre::MaterialPtr &mat) { material = mat; }
		void setMaterialName(const Ogre::String &mat) { material = Ogre::MaterialManager::getSingleton().getByName(mat); }
		inline Ogre::String getMaterialName() const { return material->getName(); }

		void addSelfToRenderQueue(Ogre::RenderQueue *queue, Ogre::uint8 group);
		void getRenderOperation(Ogre::RenderOperation& op);
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
		const Ogre::LightList& getLights(void) const;

		Ogre::Technique *getTechnique() const { return bestTechnqiue; }
		const Ogre::MaterialPtr &getMaterial(void) const { return material; }
		void getWorldTransforms(Ogre::Matrix4* xform) const { *xform = parent->_getParentNodeFullTransform(); }
		const Ogre::Quaternion& getWorldOrientation(void) const { return parent->sceneNode->_getDerivedOrientation(); }
		const Ogre::Vector3& getWorldPosition(void) const { return parent->sceneNode->_getDerivedPosition(); }
		bool castsShadows(void) const { return parent->getCastShadows(); }

	private:
		//A structure defining the desired position/orientation/scale of a batched mesh. The
		//SubMesh is not specified since that can be determined by which MeshQueue this belongs to.
		struct QueuedMesh
		{
			Ogre::SubMesh *mesh;
			Ogre::Vector3 position;
			Ogre::Quaternion orientation;
			Ogre::Vector3 scale;
		};

		bool built;
		
		Ogre::VertexData *vertexData;
		Ogre::IndexData *indexData;

		Ogre::SubMesh *meshType;
		BatchedGeometry *parent;
		Ogre::MaterialPtr material;
		Ogre::Technique *bestTechnqiue;	//This is recalculated every frame

		typedef std::vector<QueuedMesh>::iterator MeshQueueIterator;
		typedef std::vector<QueuedMesh> MeshQueue;
		MeshQueue meshQueue;	//The list of meshes to be added to this batch
	};

private:
	Ogre::String getFormatString(Ogre::SubMesh *mesh);

	typedef std::map<Ogre::String, SubBatch*> SubBatchMap;	//Stores a list of GeomBatch'es, using a format string (generated with getGeometryFormatString()) as the key value
	SubBatchMap subBatchMap;

	Ogre::Vector3 center;
	Ogre::AxisAlignedBox bounds;
	bool boundsUndefined;
	Ogre::Real radius;

	Ogre::SceneManager *sceneMgr;
	Ogre::SceneNode *sceneNode;

	Ogre::Real minDistanceSquared;
	bool withinFarDistance;

	bool built;

public:
	typedef Ogre::MapIterator<SubBatchMap> SubBatchIterator;
	SubBatchIterator getSubBatchIterator() const;
};

#endif
