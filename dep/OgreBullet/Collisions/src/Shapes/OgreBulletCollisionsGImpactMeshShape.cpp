// /***************************************************************************
// 
// This source file is part of OGREBULLET
// (Object-oriented Graphics Rendering Engine Bullet Wrapper)
// For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10
// 
// Copyright (c) 2007 tuan.kuranes@gmail.com
// 
//     file added by rtti(at)elane2k(dot)com
// 
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place - Suite 330, Boston, MA 02111-1307, USA, or go to
// http://www.gnu.org/copyleft/lesser.txt.
// -----------------------------------------------------------------------------
// */
// 
// #include "OgreBulletCollisions.h"
// 
// #include "Shapes/OgreBulletCollisionsGImpactMeshShape.h"
// #include "Debug/OgreBulletCollisionsDebugLines.h"
// #include "Utils/OgreBulletConverter.h"
// 
// using namespace Ogre;
// using namespace OgreBulletCollisions;
// 
// const int NUM_TRIANGLES = 12;
// const int NUM_VERTICES = 8;
// const int NUM_INDICES  = NUM_TRIANGLES * 3;
// 
// 
// btScalar gVertices[NUM_VERTICES * 3] = {
// btScalar(0.5 ), btScalar(0.5 ), btScalar (-0.5), 
// btScalar(0.5 ), btScalar(-0.5), btScalar (-0.5), 
// btScalar(-0.5), btScalar(-0.5), btScalar (-0.5), 
// btScalar(-0.5), btScalar(0.5 ), btScalar (-0.5), 
// btScalar(0.5 ), btScalar(0.5 ), btScalar (0.5 ),
// btScalar(-0.5), btScalar(0.5 ), btScalar (0.5 ),
// btScalar(-0.5), btScalar(-0.5), btScalar (0.5 ),
// btScalar(0.5 ), btScalar(-0.5), btScalar (0.5 )};
// 
// unsigned int gIndices[NUM_TRIANGLES][3] = {
// {0,1,2},
// {0,2,3},
// {4,5,7},
// {5,6,7},
// {8,9,11},
// {9,10,11},
// {12,13,15},
// {13,14,15},
// {16,17,18},
// {16,18,19},
// {20,21,23},
// {21,22,23}};
// 
// // 0.5 0.5 -0.5 
// // 0.5 -0.5 -0.5 
// // -0.5 -0.5 -0.5 
// // -0.5 0.5 -0.5 
// // 0.5 0.5 0.5 
// // -0.5 0.5 0.5 
// // -0.5 -0.5 0.5 
// // 0.5 -0.5 0.5
// 
// namespace OgreBulletCollisions
// {
//     // -------------------------------------------------------------------------
//     GImpactMeshCollisionShape::GImpactMeshCollisionShape(
//         Vector3* vertices, unsigned int vertexCount, unsigned int *indices, unsigned int indexCount) :   
//         CollisionShape()// ,
//     {
// 
//         // std::cout << "we have vcount: " << vertexCount / 3 << std::endl;
//         // std::cout << "we have icount: " << indexCount / 3 << std::endl;
// 
//         vertexCount = NUM_VERTICES;
//         indexCount = NUM_INDICES;
// 
//     // create vertex buffer
//     // mVbuffer = (btScalar*)malloc(sizeof(btScalar) * vertexCount);
// 
//         mVbuffer = gVertices;
//         indices = (unsigned int*)gIndices;
// 
//     for(int i = 0; i < (int)vertexCount / 3; i++)
//     {
//         std::cout << (btScalar)vertices[i].x << " " << (btScalar)vertices[i].y << " " << (btScalar)vertices[i].z << " " << std::endl;
//         
//         mVbuffer[(i * 3) + 0] = (btScalar)vertices[i].x;
//         mVbuffer[(i * 3) + 1] = (btScalar)vertices[i].y;
//         mVbuffer[(i * 3) + 2] = (btScalar)vertices[i].z;
//     }
// 
//     for(int i = 0; i < indexCount; i++)
//         std::cout << indices[i] << std::endl;
// 
// 
//     /// @todo TODO: signed unsigned wooooooooooo o.O
// 
//  // create trimesh
//  btTriangleIndexVertexArray* indexVertexArrays = 
//          new btTriangleIndexVertexArray(indexCount / 3,
//                  (int*)indices,
//                  3*sizeof(int),
//                  vertexCount / 3, mVbuffer, sizeof(btScalar) * 3);
// 
//  mShape = new btGImpactMeshShape(indexVertexArrays);
//     // trimesh->setLocalScaling(btVector3(4.f,4.f,4.f));
//  ((btGImpactMeshShape*)mShape)->updateBound();
//  
//     // //register algorithm
//     // 
//     // btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(m_dynamicsWorld ->getDispatcher());
//     // btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
// 
// 
//     }
//     // -------------------------------------------------------------------------
//     GImpactMeshCollisionShape::~GImpactMeshCollisionShape()
//     {
//         /// @todo TODO: cleanup
//         free(mVbuffer);
//     }
//     // -------------------------------------------------------------------------
//     bool GImpactMeshCollisionShape::drawWireFrame(DebugLines *wire) const
//     {
//     //         const int numTris = mTriMesh->getNumTriangles ();
//     //         if (numTris > 0)
//     //         {
//     // /*
//     //             for (int currTriIdx = 0; numTris > currTriIdx; currTriIdx++)
//     //             {
//     //                 btMyTriangle myTri = mTriMesh->getTriangle (currTriIdx);
//     // 
//     //                 wire->addLine (BtOgreConverter::to(myTri.m_vert0), BtOgreConverter::to(myTri.m_vert1));
//     //                 wire->addLine (BtOgreConverter::to(myTri.m_vert1), BtOgreConverter::to(myTri.m_vert2));
//     //                 wire->addLine (BtOgreConverter::to(myTri.m_vert2), BtOgreConverter::to(myTri.m_vert0));
//     // 
//     //                 
//     //                 //wire->addPoint (BtOgreConverter::to(myTri.m_vert0));
//     //                 //wire->addPoint (BtOgreConverter::to(myTri.m_vert1));
//     //                 //wire->addPoint (BtOgreConverter::to(myTri.m_vert2));
//     //             }
//     //             return true
//     // ;*/
//     //         }
//         return false;
//     }
// }
// 
