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

/******************************************************************************
   This file is based on the OgreBulletMeshToShapeConverter from OgreBullet
    
   (Object-oriented Graphics Rendering Engine Bullet Wrapper)
   
   For the latest info, see 
   http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10
    
   written by tuan.kuranes@gmail.com    
\******************************************************************************/

#include "MeshToShapeConverter.h"

using namespace Ogre;
using namespace CoAJnR;

void MeshToShapeConverter::addVertexData(const VertexData *vertex_data,
                                   const VertexData *blended_data,
                                   const Mesh::IndexMap *indexMap)
{
    if (!vertex_data) 
        return;

    const VertexData *data = (blended_data) ? blended_data: vertex_data;

    const uint prev_size = mVertexCount;
    mVertexCount += (uint)data->vertexCount;

    Ogre::Vector3* tmp_vert = new Ogre::Vector3[mVertexCount];
    if (mVertexBuffer)
    {
        memcpy(tmp_vert,mVertexBuffer,sizeof(Vector3) * prev_size);
        delete[] mVertexBuffer;
    }
    mVertexBuffer = tmp_vert;

    // Get the positional buffer element
    {   
        const Ogre::VertexElement* posElem = data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);            
        Ogre::HardwareVertexBufferSharedPtr vbuf = data->vertexBufferBinding->getBuffer(posElem->getSource());
        const uint vSize = (uint)vbuf->getVertexSize();

        unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        float* pReal;
        Ogre::Vector3 * curVertices = &mVertexBuffer[prev_size];
        const uint vertexCount = (uint)data->vertexCount;
        for(uint j = 0; j < vertexCount; ++j)
        {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            vertex += vSize;

            curVertices->x = (*pReal++);
            curVertices->y = (*pReal++);
            curVertices->z = (*pReal++);

            *curVertices = mTransform * (*curVertices);
            
            curVertices++;
        }
        vbuf->unlock();
    }

    // Get the bone index element
    if (mEntity && mEntity->hasSkeleton())
    {

        Ogre::MeshPtr mesh = mEntity->getMesh ();

        const Ogre::VertexElement* bneElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_BLEND_INDICES);
        assert (bneElem);
        {
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(bneElem->getSource());
            const uint vSize = (uint)vbuf->getVertexSize();
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            unsigned char* pBone;

            if (!mBoneIndex)
                mBoneIndex = new BoneIndex();   
            BoneIndex::iterator i;

            Ogre::Vector3 * curVertices = &mVertexBuffer[prev_size];
            
            const uint vertexCount = (uint)vertex_data->vertexCount;
            for(uint j = 0; j < vertexCount; ++j)
            {
                bneElem->baseVertexPointerToElement(vertex, &pBone);
                vertex += vSize;
               
                const unsigned char currBone = (indexMap) ? (*indexMap)[*pBone] : *pBone;
                i = mBoneIndex->find (currBone);
                Vector3Vector* l = 0;
                if (i == mBoneIndex->end())
                {
                    l = new Vector3Vector;
                    mBoneIndex->insert(BoneKeyIndex(currBone, l));
                }                       
                else 
                {
                    l = i->second;
                }

                l->push_back(*curVertices);

                curVertices++;
            }
            vbuf->unlock();
        }
    }
}

void MeshToShapeConverter::addIndexData(IndexData *data, const uint offset)
{
    const uint prev_size = mIndexCount;
    mIndexCount += (uint)data->indexCount;

    uint* tmp_ind = new uint[mIndexCount];
    if (mIndexBuffer)
    {
        memcpy (tmp_ind, mIndexBuffer, sizeof(uint) * prev_size);
        delete[] mIndexBuffer;
    }
    mIndexBuffer = tmp_ind;

    const uint numTris = (uint) data->indexCount / 3;
    HardwareIndexBufferSharedPtr ibuf = data->indexBuffer;  
    const bool use32bitindexes = (ibuf->getType() == HardwareIndexBuffer::IT_32BIT);
    uint index_offset = prev_size;

    if (use32bitindexes) 
    {
        const uint* pInt = static_cast<uint*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
        for(uint k = 0; k < numTris; ++k)
        {
            mIndexBuffer[index_offset ++] = offset + *pInt++;
            mIndexBuffer[index_offset ++] = offset + *pInt++;
            mIndexBuffer[index_offset ++] = offset + *pInt++;
        }
        ibuf->unlock();
    }
    else 
    {
        const unsigned short* pShort = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
        for(uint k = 0; k < numTris; ++k)
        {
            mIndexBuffer[index_offset ++] = offset + static_cast<uint> (*pShort++);
            mIndexBuffer[index_offset ++] = offset + static_cast<uint> (*pShort++);
            mIndexBuffer[index_offset ++] = offset + static_cast<uint> (*pShort++);
        }
        ibuf->unlock();
    }

}

void MeshToShapeConverter::addEntity(Entity *entity,const Matrix4 &transform)
{
    // Each entity added need to reset size and radius
    // next time getRadius and getSize are asked, they're computed.
    mBounds  = Ogre::Vector3(-1,-1,-1);
    mBoundRadius = -1;

    mEntity = entity;
    mNode = (SceneNode*)(mEntity->getParentNode());
    mTransform = transform;

    const bool isSkeletonAnimated = mEntity->hasSkeleton();
    //const bool isHWanimated = isSkeletonAnimated && entity->isHardwareAnimationEnabled();
    if (isSkeletonAnimated)
    {
        mEntity->addSoftwareAnimationRequest(false);
        mEntity->_updateAnimation();
    }


    if (mEntity->getMesh()->sharedVertexData)
    {
        if (!isSkeletonAnimated)
            addVertexData (mEntity->getMesh()->sharedVertexData);
        else
            addVertexData (mEntity->getMesh()->sharedVertexData, 
            mEntity->_getSkelAnimVertexData(),
            &mEntity->getMesh()->sharedBlendIndexToBoneIndexMap); 
    }

    for(uint i = 0;i < mEntity->getNumSubEntities();++i)
    {
        SubMesh *sub_mesh = mEntity->getSubEntity(i)->getSubMesh();

        if (!sub_mesh->useSharedVertices)
        {
            addIndexData(sub_mesh->indexData,mVertexCount);

            if (!isSkeletonAnimated)
                addVertexData (sub_mesh->vertexData);
            else
                addVertexData (sub_mesh->vertexData, 
                mEntity->getSubEntity(i)->_getSkelAnimVertexData(),
                &sub_mesh->blendIndexToBoneIndexMap); 
        }
        else 
        {
            addIndexData (sub_mesh->indexData);
        }

    }

    if (isSkeletonAnimated) 
        mEntity->removeSoftwareAnimationRequest(false);
}

void MeshToShapeConverter::addMesh(const MeshPtr &mesh, const Matrix4 &transform)
{
    // Each entity added need to reset size and radius
    // next time getRadius and getSize are asked, they're computed.
    mBounds  = Ogre::Vector3(-1,-1,-1);
    mBoundRadius = -1;

    //_entity = entity;
    //_node = (SceneNode*)(_entity->getParentNode());
    mTransform = transform;

    if (mesh->hasSkeleton ())
        Ogre::LogManager::getSingleton().logMessage("MeshToShapeConverter::addMesh : Mesh " + mesh->getName () + " as skeleton but added to trimesh non animated");
//      OGRE_EXCEPT(1, "Mesh " + mesh->getName () + " as skeleton but added to trimesh non animated", 
//                  "MeshToShapeConverter::addMesh");

    if (mesh->sharedVertexData)
    {
        addVertexData (mesh->sharedVertexData);
    }

    for(uint i = 0;i < mesh->getNumSubMeshes();++i)
    {
        SubMesh *sub_mesh = mesh->getSubMesh(i);

        if (!sub_mesh->useSharedVertices)
        {
            addIndexData(sub_mesh->indexData, mVertexCount);
            addVertexData (sub_mesh->vertexData);
        }
        else 
        {
            addIndexData (sub_mesh->indexData);
        }

    }
}

MeshToShapeConverter::MeshToShapeConverter(Entity *entity,const Matrix4 &transform) :
    mVertexBuffer (0),
    mIndexBuffer (0),
    mVertexCount (0),
    mIndexCount (0),
    mBoneIndex (0),
    mBoundRadius (-1),
    mBounds (Vector3(-1,-1,-1))
{
    addEntity(entity, transform);   
}

MeshToShapeConverter::MeshToShapeConverter(Renderable *rend, const Matrix4 &transform) : 
    mEntity (0), 
    mTransform (transform),
    mVertexBuffer (0), 
    mIndexBuffer (0), 
    mVertexCount (0), 
    mIndexCount (0), 
    mBoneIndex (0), 
    mBoundRadius (-1), 
    mBounds (Vector3(-1,-1,-1))
{ 
   RenderOperation op; 
   rend->getRenderOperation(op); 
   addVertexData(op.vertexData); 
   if(op.useIndexes) 
      addIndexData(op.indexData); 
}

MeshToShapeConverter::MeshToShapeConverter() :
        mVertexBuffer (0),
        mIndexBuffer (0),
        mVertexCount (0),
        mIndexCount (0),
        mBoneIndex (0),
        mBoundRadius (-1),
        mBounds (Vector3(-1,-1,-1))
{
    
}

Real MeshToShapeConverter::getRadius()
{
    if (mBoundRadius == (-1))
    {
        getSize();
        mBoundRadius = (std::max(mBounds.x,std::max(mBounds.y,mBounds.z)) * 0.5);
    }
    return mBoundRadius;
}

Vector3 MeshToShapeConverter::getSize()
{
    const uint vCount = getVertexCount();
    if (mBounds == Ogre::Vector3(-1,-1,-1) && vCount > 0)
    {

        const Ogre::Vector3 * const v = getVertices();
        
        Ogre::Vector3 vmin(v[0]);
        Ogre::Vector3 vmax(v[0]);

        for(uint j = 1; j < vCount; j++)
        {
            vmin.x = std::min(vmin.x, v[j].x);
            vmin.y = std::min(vmin.y, v[j].y);
            vmin.z = std::min(vmin.z, v[j].z);

            vmax.x = std::max(vmax.x, v[j].x);
            vmax.y = std::max(vmax.y, v[j].y);
            vmax.z = std::max(vmax.z, v[j].z);
        }

        mBounds.x = (vmax.x - vmin.x)/2.0;
        mBounds.y = (vmax.y - vmin.y)/2.0;
        mBounds.z = (vmax.z - vmin.z)/2.0;
    }

    return mBounds;
}

const Ogre::Vector3* MeshToShapeConverter::getVertices()
{
    return mVertexBuffer;
}

uint MeshToShapeConverter::getVertexCount()
{
    return mVertexCount;
}

const uint* MeshToShapeConverter::getIndices()
{
    return mIndexBuffer;
}

uint MeshToShapeConverter::getIndexCount()
{
    return mIndexCount;
}

void
MeshToShapeConverter::getSphereRadius(Ogre::Real& p_radius)
{
   p_radius = getRadius();
   assert((p_radius > 0.0) && 
       ("Sphere radius must be greater than zero"));
}

//CylinderCollisionShape* MeshToShapeConverter::createCylinder(Ogre::Vector3 axis)
//{
//    const Ogre::Vector3 sz = getSize();
//    assert((sz.x > 0.0) && (sz.y > 0.0) && (sz.z > 0.0) && 
//        ("Size of Cylinder must be greater than zero on all axes"));
//
//    CylinderCollisionShape* shape = new CylinderCollisionShape(sz, axis);
//    return shape;
//}

void MeshToShapeConverter::getBox(Ogre::Vector3& p_boundingBox)
{
    p_boundingBox = getSize();

    assert(
            (p_boundingBox.x > 0.0) && 
            (p_boundingBox.y > 0.0) && 
            (p_boundingBox.z > 0.0) && 
            ("Size of box must be greater than zero on all axes"));
}

void 
MeshToShapeConverter::getIndexArray(uint*& p_indices, uint& p_numIndices)
{
    assert(mVertexCount && (mIndexCount >= 6) && 
        ("Mesh must have some vertices and at least 6 indices (2 triangles)"));

    p_indices = new uint[mIndexCount];

    for(uint i = 0; i < mIndexCount; i++)
        p_indices[i] = mIndexBuffer[i];
    
    p_numIndices = mIndexCount;
}

void 
MeshToShapeConverter::getVertexArray(
        Ogre::Real*& p_vertices, 
        uint& p_numVertices)
{
    assert(mVertexCount && (mIndexCount >= 6) && 
        ("Mesh must have some vertices and at least 6 indices (2 triangles)"));

    p_vertices = new Ogre::Real[mVertexCount * 3];

    for(uint i = 0; i < mVertexCount; i++)
    {
        p_vertices[(i * 3) + 0] = mVertexBuffer[i].x;
        p_vertices[(i * 3) + 1] = mVertexBuffer[i].y;
        p_vertices[(i * 3) + 2] = mVertexBuffer[i].z;
    }
    
    p_numVertices = mVertexCount;
}

bool MeshToShapeConverter::addBoneVertices(
        unsigned char bone, 
        uint& vertex_count, 
        Ogre::Vector3*& vertices)
{
    BoneIndex::iterator i = mBoneIndex->find(bone);
    
    if (i == mBoneIndex->end()) 
        return false;

    if (i->second->empty()) 
        return false;

    vertex_count = (uint) i->second->size() + 1;

    vertices = new Ogre::Vector3[vertex_count];
    vertices[0] = mEntity->_getParentNodeFullTransform() * 
            mEntity->getSkeleton()->getBone(bone)->_getDerivedPosition();

    uint o = 1;
    for(Vector3Vector::iterator j = i->second->begin();
        j != i->second->end(); ++j,++o) 
    {
        vertices[o] = (*j);
    }       
    return true;
}

MeshToShapeConverter::~MeshToShapeConverter()
{
    delete[] mVertexBuffer;
    delete[] mIndexBuffer;

    if (mBoneIndex)
    {
        for(BoneIndex::iterator i = mBoneIndex->begin();
            i != mBoneIndex->end();
            ++i)
        {
            delete i->second;
        }
        delete mBoneIndex;
    }
}
