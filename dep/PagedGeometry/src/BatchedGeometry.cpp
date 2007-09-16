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

#include "BatchedGeometry.h"

#include "OgreRoot.h"
#include "OgreCamera.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreSceneNode.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreEntity.h"
#include "OgreSubMesh.h"
#include "OgreMesh.h"
#include "OgreMeshManager.h"
#include "OgreHardwareBufferManager.h"
#include "OgreHardwareBuffer.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
using namespace Ogre;


//-------------------------------------------------------------------------------------

BatchedGeometry::BatchedGeometry(SceneManager *mgr)
 :	withinFarDistance(0),
	minDistanceSquared(0),
	sceneNode(NULL),
	sceneMgr(mgr),
	built(false),
	boundsUndefined(true)

{
	clear();
}

BatchedGeometry::~BatchedGeometry()
{
	clear();
}

void BatchedGeometry::addEntity(Entity *ent, const Vector3 &position, const Quaternion &orientation, const Vector3 &scale)
{
	//For each submesh
	Mesh::SubMeshIterator i = ent->getMesh()->getSubMeshIterator();
	while (i.hasMoreElements()){
		//Get the submesh
		SubMesh *mesh = i.peekNext();

		//Generate a format string that uniquely identifies this material & vertex/index format
		String formatStr = getFormatString(mesh);
		
		//If a batch using an identical format exists...
		SubBatch *batch;
		SubBatchMap::iterator batchIter = subBatchMap.find(formatStr);
		if (batchIter != subBatchMap.end()){
			//Use the batch
			batch = batchIter->second;
		} else {
			//Otherwise create a new batch
			batch = new SubBatch(this, mesh);
			subBatchMap.insert(std::pair<String, SubBatch*>(formatStr, batch));
		}

		//Now add the submesh to the compatible batch
		batch->addSubMesh(mesh, position, orientation, scale);

		//Next item
		i.getNext();
	}

	//Update bounding box
	Matrix4 mat(orientation);
	mat.setScale(scale);
	AxisAlignedBox entBounds = ent->getBoundingBox();
	entBounds.transform(mat);
	
	if (boundsUndefined){
		bounds.setMinimum(entBounds.getMinimum() + position);
		bounds.setMaximum(entBounds.getMaximum() + position);
		boundsUndefined = false;
	} else {
		Vector3 min = bounds.getMinimum();
		Vector3 max = bounds.getMaximum();
		min.makeFloor(entBounds.getMinimum() + position);
		max.makeCeil(entBounds.getMaximum() + position);
		bounds.setMinimum(min);
		bounds.setMaximum(max);
	}
}

BatchedGeometry::SubBatchIterator BatchedGeometry::getSubBatchIterator() const
{
	return BatchedGeometry::SubBatchIterator((SubBatchMap&)subBatchMap);
}

String BatchedGeometry::getFormatString(SubMesh *mesh)
{
	StringUtil::StrStreamType str;

	str << mesh->getMaterialName() << "|";
	str << mesh->indexData->indexBuffer->getType() << "|";

	const VertexDeclaration::VertexElementList &elemList = mesh->vertexData->vertexDeclaration->getElements();
	VertexDeclaration::VertexElementList::const_iterator i;
	for (i = elemList.begin(); i != elemList.end(); ++i)
	{
		const VertexElement &element = *i;
		str << element.getSource() << "|";
		str << element.getSemantic() << "|";
		str << element.getType() << "|";
	}

	return str.str();
}

void BatchedGeometry::build()
{
	///Make sure the batch hasn't already been built
	if (built)
		OGRE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Invalid call to build() - geometry is already batched (call clear() first)", "BatchedGeometry::GeomBatch::build()");

	if (subBatchMap.size() != 0) {
		//Finish bounds information
		center = bounds.getCenter();			//Calculate bounds center
		bounds.setMinimum(bounds.getMinimum() - center);	//Center the bounding box
		bounds.setMaximum(bounds.getMaximum() - center);	//Center the bounding box
		radius = bounds.getMaximum().length();	//Calculate BB radius
		
		//Create scene node
		sceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode(center);

		//Build each batch
		for (SubBatchMap::iterator i = subBatchMap.begin(); i != subBatchMap.end(); ++i){
			i->second->build();
		}

		//Attach the batch to the scene node
		sceneNode->attachObject(this);

		//Debug
		//sceneNode->showBoundingBox(true);

		built = true;
	}
	
}

void BatchedGeometry::clear()
{
	//Remove the batch from the scene
	if (sceneNode){
		sceneNode->removeAllChildren();
		sceneMgr->destroySceneNode(sceneNode->getName());
		sceneNode = NULL;
	}

	//Reset bounds information
	boundsUndefined = true;
	center = Vector3::ZERO;
	radius = 0;

	//Delete each batch
	for (SubBatchMap::iterator i = subBatchMap.begin(); i != subBatchMap.end(); ++i){
		delete i->second;
	}
	subBatchMap.clear();

	built = false;
}

void BatchedGeometry::_updateRenderQueue(RenderQueue *queue)
{
	//If visible...
	if (isVisible()){
		//Ask each batch to add itself to the render queue if appropriate
		for (SubBatchMap::iterator i = subBatchMap.begin(); i != subBatchMap.end(); ++i){
			i->second->addSelfToRenderQueue(queue, getRenderQueueGroup());
		}
	}
}

bool BatchedGeometry::isVisible()
{
	return mVisible && withinFarDistance;
}

void BatchedGeometry::_notifyCurrentCamera(Camera *cam)
{
	if (getRenderingDistance() == 0) {
		withinFarDistance = true;
	} else {
		//Calculate camera distance
		Vector3 camVec = cam->getDerivedPosition() - center;
		Real centerDistanceSquared = camVec.squaredLength();
		minDistanceSquared = std::max(0.0f, centerDistanceSquared - (radius * radius));
		//Note: centerDistanceSquared measures the distance between the camera and the center of the GeomBatch,
		//while minDistanceSquared measures the closest distance between the camera and the closest edge of the
		//geometry's bounding sphere.

		//Determine whether the BatchedGeometry is within the far rendering distance
		withinFarDistance = minDistanceSquared <= Math::Sqr(getRenderingDistance());
	}
}




BatchedGeometry::SubBatch::SubBatch(BatchedGeometry *parent, SubMesh *mesh)
{
	meshType = mesh;
	material = MaterialManager::getSingleton().getByName(mesh->getMaterialName());
	this->parent = parent;
	built = false;

	//Setup vertex/index data structure
	vertexData = mesh->vertexData->clone(false);
	indexData = mesh->indexData->clone(false);

	//Remove blend weights from vertex format
	const VertexElement* blendIndices = vertexData->vertexDeclaration->findElementBySemantic(VES_BLEND_INDICES);
	const VertexElement* blendWeights = vertexData->vertexDeclaration->findElementBySemantic(VES_BLEND_WEIGHTS);
	if (blendIndices && blendWeights)
	{
		//Check for format errors
		assert(blendIndices->getSource() == blendWeights->getSource()
			&& "Blend indices and weights should be in the same buffer");
		assert(blendIndices->getSize() + blendWeights->getSize() == vertexData->vertexBufferBinding->getBuffer(blendIndices->getSource())->getVertexSize()
			&& "Blend indices and blend buffers should have buffer to themselves!");

		//Remove the blend weights
		vertexData->vertexBufferBinding->unsetBinding(blendIndices->getSource());
		vertexData->vertexDeclaration->removeElement(VES_BLEND_INDICES);
		vertexData->vertexDeclaration->removeElement(VES_BLEND_WEIGHTS);
		#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR > 2
		vertexData->closeGapsInBindings();
		#endif
	}

	//Reset vertex/index count
	vertexData->vertexStart = 0;
	vertexData->vertexCount = 0;
	indexData->indexStart = 0;
	indexData->indexCount = 0;
}

BatchedGeometry::SubBatch::~SubBatch()
{
	clear();

	delete vertexData;
	delete indexData;
}

void BatchedGeometry::SubBatch::addSubMesh(SubMesh *mesh, const Vector3 &position, const Quaternion &orientation, const Vector3 &scale)
{
	assert(!built);

	//Add this submesh to the queue
	QueuedMesh newMesh;
	newMesh.mesh = mesh;
	newMesh.position = position;
	newMesh.orientation = orientation;
	newMesh.scale = scale;
	meshQueue.push_back(newMesh);

	//Increment the vertex/index count so the buffers will have room for this mesh
	vertexData->vertexCount += mesh->vertexData->vertexCount;
	indexData->indexCount += mesh->indexData->indexCount;
}

void BatchedGeometry::SubBatch::build()
{
	assert(!built);

	//Misc. setup
	Vector3 batchCenter = parent->center;

	//Allocate the index buffer
	HardwareIndexBuffer::IndexType indexType = meshType->indexData->indexBuffer->getType();
	indexData->indexBuffer = HardwareBufferManager::getSingleton()
		.createIndexBuffer(indexType, indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	//Lock the index buffer
	uint32 *indexBuffer32;
	uint16 *indexBuffer16;
	if (indexType == HardwareIndexBuffer::IT_32BIT)
		indexBuffer32 = static_cast<uint32*>(indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
	else
		indexBuffer16 = static_cast<uint16*>(indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));

	//Allocate & lock the vertex buffers
	std::vector<uchar*> vertexBuffers;
	std::vector<VertexDeclaration::VertexElementList> vertexBufferElements;

	VertexBufferBinding *vertBinding = vertexData->vertexBufferBinding;
	VertexDeclaration *vertDecl = vertexData->vertexDeclaration;

	for (Ogre::ushort i = 0; i < vertBinding->getBufferCount(); ++i)
	{
		HardwareVertexBufferSharedPtr buffer = HardwareBufferManager::getSingleton()
			.createVertexBuffer(vertDecl->getVertexSize(i), vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		vertBinding->setBinding(i, buffer);
		
		vertexBuffers.push_back(static_cast<uchar*>(buffer->lock(HardwareBuffer::HBL_DISCARD)));
		vertexBufferElements.push_back(vertDecl->findElementsBySource(i));
	}

	//For each queued mesh...
	MeshQueueIterator it;
	size_t indexOffset = 0;
	for (it = meshQueue.begin(); it != meshQueue.end(); ++it) {
		const QueuedMesh queuedMesh = (*it);
		const IndexData *sourceIndexData = queuedMesh.mesh->indexData;
		const VertexData *sourceVertexData = queuedMesh.mesh->vertexData;

		//Copy mesh vertex data into the vertex buffer
		VertexBufferBinding *sourceBinds = sourceVertexData->vertexBufferBinding;
		VertexBufferBinding *destBinds = vertexData->vertexBufferBinding;
		for (Ogre::ushort i = 0; i < destBinds->getBufferCount(); ++i)
		{
			//Lock the input buffer
			HardwareVertexBufferSharedPtr sourceBuffer = sourceBinds->getBuffer(i);
			uchar *sourceBase = static_cast<uchar*>(sourceBuffer->lock(HardwareBuffer::HBL_READ_ONLY));

			//Get the locked output buffer
			uchar *destBase = vertexBuffers[i];

			//Copy vertices
			float *sourcePtr, *destPtr;
			for (size_t v = 0; v < sourceVertexData->vertexCount; ++v)
			{
				// Iterate over vertex elements
				VertexDeclaration::VertexElementList &elems = vertexBufferElements[i];
				VertexDeclaration::VertexElementList::iterator ei;
				for (ei = elems.begin(); ei != elems.end(); ++ei)
				{
					VertexElement &elem = *ei;
					elem.baseVertexPointerToElement(sourceBase, &sourcePtr);
					elem.baseVertexPointerToElement(destBase, &destPtr);

					Vector3 tmp;
					switch (elem.getSemantic())
					{
					case VES_POSITION:
						tmp.x = *sourcePtr++;
						tmp.y = *sourcePtr++;
						tmp.z = *sourcePtr++;
						
						//Transform
						tmp = (queuedMesh.orientation * (tmp * queuedMesh.scale)) + queuedMesh.position;
						tmp -= batchCenter;		//Adjust for batch center

						*destPtr++ = tmp.x;
						*destPtr++ = tmp.y;
						*destPtr++ = tmp.z;
						break;

					case VES_NORMAL:
						tmp.x = *sourcePtr++;
						tmp.y = *sourcePtr++;
						tmp.z = *sourcePtr++;

						//Rotate
						tmp = queuedMesh.orientation * tmp;

						*destPtr++ = tmp.x;
						*destPtr++ = tmp.y;
						*destPtr++ = tmp.z;
						break;

					case VES_TANGENT:
					case VES_BINORMAL:
						tmp.x = *sourcePtr++;
						tmp.y = *sourcePtr++;
						tmp.z = *sourcePtr++;

						//Rotate
						tmp = queuedMesh.orientation * tmp;

						*destPtr++ = tmp.x;
						*destPtr++ = tmp.y;
						*destPtr++ = tmp.z;
						break;

					default:
						//Raw copy
						memcpy(destPtr, sourcePtr, VertexElement::getTypeSize(elem.getType()));
						break;
					};
				}

				// Increment both pointers
				destBase += sourceBuffer->getVertexSize();
				sourceBase += sourceBuffer->getVertexSize();

			}

			//Unlock the input buffer
			vertexBuffers[i] = destBase;
			sourceBuffer->unlock();
		}


		//Copy mesh index data into the index buffer
		if (indexType == HardwareIndexBuffer::IT_32BIT) {
			//Lock the input buffer
			uint32 *source = static_cast<uint32*>(sourceIndexData->indexBuffer->lock(
				sourceIndexData->indexStart, sourceIndexData->indexCount, HardwareBuffer::HBL_READ_ONLY
				));
			uint32 *sourceEnd = source + sourceIndexData->indexCount;

			//And copy it to the output buffer
			while (source != sourceEnd) {
				*indexBuffer32++ = static_cast<uint32>(*source++ + indexOffset);
			}
			
			//Unlock the input buffer
			sourceIndexData->indexBuffer->unlock();

			//Increment the index offset
			indexOffset += sourceVertexData->vertexCount;
		} else {
			//Lock the input buffer
			uint16 *source = static_cast<uint16*>(sourceIndexData->indexBuffer->lock(
				sourceIndexData->indexStart, sourceIndexData->indexCount, HardwareBuffer::HBL_READ_ONLY
				));
			uint16 *sourceEnd = source + sourceIndexData->indexCount;

			//And copy it to the output buffer
			while (source != sourceEnd) {
				*indexBuffer16++ = static_cast<uint16>(*source++ + indexOffset);
			}

			//Unlock the input buffer
			sourceIndexData->indexBuffer->unlock();

			//Increment the index offset
			indexOffset += sourceVertexData->vertexCount;
		}
	}

	//Unlock buffers
	indexData->indexBuffer->unlock();
	for (Ogre::ushort i = 0; i < vertBinding->getBufferCount(); ++i)
		vertBinding->getBuffer(i)->unlock();

	//Clear mesh queue
	meshQueue.clear();

	built = true;
}

void BatchedGeometry::SubBatch::clear()
{
	//If built, delete the batch
	if (built){
		//Delete buffers
		indexData->indexBuffer.setNull();
		vertexData->vertexBufferBinding->unsetAllBindings();

		//Reset vertex/index count
		vertexData->vertexStart = 0;
		vertexData->vertexCount = 0;
		indexData->indexStart = 0;
		indexData->indexCount = 0;
	}

	//Clear mesh queue
	meshQueue.clear();

	built = false;
}

void BatchedGeometry::SubBatch::addSelfToRenderQueue(RenderQueue *queue, uint8 group)
{
	if (built){
		//Update material technique based on camera distance
		assert(!material.isNull());
		bestTechnqiue = material->getBestTechnique(material->getLodIndexSquaredDepth(parent->minDistanceSquared));
			
		//Add to render queue
		queue->addRenderable(this, group);
	}
}

void BatchedGeometry::SubBatch::getRenderOperation(RenderOperation& op)
{
	op.operationType = RenderOperation::OT_TRIANGLE_LIST;
	op.srcRenderable = this;
	op.useIndexes = true;
	op.vertexData = vertexData;
	op.indexData = indexData;
}

Real BatchedGeometry::SubBatch::getSquaredViewDepth(const Camera* cam) const
{
	Vector3 camVec = cam->getDerivedPosition() - parent->center;
	return camVec.squaredLength();
}

#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR <= 2
//Dagon-compatible getLights()
const Ogre::LightList& BatchedGeometry::SubBatch::getLights(void) const
{
	return parent->sceneNode->findLights(parent->radius);
}
#else
//Eihort-compatible getLights()
const Ogre::LightList& BatchedGeometry::SubBatch::getLights(void) const
{
	return parent->queryLights();
}
#endif
