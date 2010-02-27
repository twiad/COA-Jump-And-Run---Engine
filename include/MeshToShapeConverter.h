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


#ifndef COAJNR_MESHTOSHAPECONVERTER_H
#define COAJNR_MESHTOSHAPECONVERTER_H

#include "Dependencies.h"

namespace CoAJnR
{

/**
 * converts ogre meshes into bullet collision shapes
 */
class MeshToShapeConverter
{
    typedef std::vector<Ogre::Vector3> Vector3Vector;
    typedef std::map<unsigned char, Vector3Vector* > BoneIndex;
    typedef std::pair<unsigned short, Vector3Vector* > BoneKeyIndex;

    Ogre::Entity* mEntity;
    Ogre::SceneNode* mNode;
    Ogre::Matrix4 mTransform;
    Ogre::Vector3* mVertexBuffer;
    uint* mIndexBuffer;
    uint mVertexCount;
    uint mIndexCount;
    BoneIndex* mBoneIndex;

    Ogre::Real mBoundRadius;
    Ogre::Vector3 mBounds;

    void addVertexData(
            const Ogre::VertexData *vertex_data, 
            const Ogre::VertexData *blended_data = 0, 
            const Ogre::Mesh::IndexMap *indexMap = 0);

    void addIndexData(Ogre::IndexData *data, const uint offset = 0);

    bool addBoneVertices(
            unsigned char bone,
            uint& vertex_count, 
            Ogre::Vector3* &vertices);

public:
    /**
     * create a mesh to shape converter with a given entity.
     */
    MeshToShapeConverter(
            Ogre::Entity* entity,
            const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);

    /**
     * create a mesh to shape converter with a given renderable.
     */
    MeshToShapeConverter(
            Ogre::Renderable* rend, 
            const Ogre::Matrix4 &transform);
    
    /**
     * create an empty mesh to shape converter.
     */
    MeshToShapeConverter();

    /**
     * dtor.
     */
    ~MeshToShapeConverter();

    /**
     * adds an entity to the mesh to shape converter.
     */
    void addEntity(
            Ogre::Entity* entity,
            const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
    
    /**
     * adds a mesh to the mesh to shape converter.
     */
    void addMesh(
            const Ogre::MeshPtr &mesh, 
            const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);

    /**
     * returns the radius of the bounding sphere of attached objects.
     */
    Ogre::Real      getRadius();

    /**
     * returns the bounding box of the attached objects.
     */
    Ogre::Vector3   getSize();

    /**
     * returns a vector describing the axis aligned bounding box 
     * of the given entity 
     */
    void getBox(Ogre::Vector3& boundingBox);

    /**
     * writes the vertex array and vertex count into parameter values.
     */
    void getVertexArray(Ogre::Real*& vertices, uint& numVertices);

    /**
     * writes the index array and index count into parameter values.
     */
    void getIndexArray(uint*& indices, uint& numIndices);

    /**
     *
     */
    //CylinderCollisionShape* createCylinder(Ogre::Vector3 axis);

    /**
     *
     */
    void getSphereRadius(Ogre::Real& radius);

    /**
     *
     */
    const Ogre::Vector3* getVertices();

    /**
     *
     */
    uint getVertexCount();

    /**
     *
     */
    const uint* getIndices();

    /**
     *
     */
    uint getIndexCount();

};

}

#endif
