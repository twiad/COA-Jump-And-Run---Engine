#include "SceneryTest.h"

#include "CameraSmoothFollow.h"
#include "Character.h"
#include "CharacterMovementController.h"
#include "GraphicsManager.h"
#include "InputHandler.h"
#include "PhysicsManager.h"
#include "StaticObject.h"
#include "DynamicObject.h"

namespace CoAJnR
{

uint CubeSpawnCollisionHandler::m_spawnId = 0;
uint TubeCollisionHandler::m_spawnId = 0;

uint DestroyTouchingObjectsCollisionHandler::m_psCount = 0;

SceneryTest::SceneryTest()
{
    m_baseLevelNode = 0;
    m_baseLevel = 0;
    m_light = 0;
    m_baseLevelShape = 0;
    m_baseLevelBody = 0;
    m_movementInputController = 0;
    m_character = 0;
    m_rot = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
    m_standardBoxShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5, 0.5, 0.5));
}

SceneryTest::~SceneryTest()
{
    cleanup();
}

void SceneryTest::setup()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();
    StaticObjectManager* som = StaticObjectManager::get();
    DynamicObjectManager* dom = DynamicObjectManager::get();

    gm->camera()->setPosition(Ogre::Vector3(-16, 15, 23));
    gm->camera()->lookAt(Ogre::Vector3(0, 0, 0));

    sm->setSkyBox(true, "CoAJnR/SkyBox/miramar");

    //Heightfield Test

    //sm->setWorldGeometry( "terrain.cfg" );
    
    // light
    m_light = sm->createLight("point-light");
    m_light->setType(Ogre::Light::LT_POINT);
    m_light->setPosition(Ogre::Vector3(10, 20, 20));

    gm->viewport()->setBackgroundColour(Ogre::ColourValue( 0.8, 0.8, 0.85));

    m_floorShape = new OgreBulletCollisions::StaticPlaneCollisionShape(
            Ogre::Vector3(0, 1, 0), -2);

    m_floorBody = new OgreBulletDynamics::RigidBody(
            "floor", PhysicsManager::get()->world());

    m_floorBody->setStaticShape(m_floorShape, 1, 1, Ogre::Vector3(0, 1, 0));
    m_floorBody->setCollisionHandler(new DestroyTouchingObjectsCollisionHandler);

    //plane
    som->createTrimeshObject("Level4.mesh", Ogre::Vector3::ZERO);
    
    m_terrain = sm->createEntity("terrain", "terrain2.mesh");
    m_terrain->setNormaliseNormals(true);

//    som->createTrimeshObject("terrain.mesh", Ogre::Vector3::ZERO);
//    som->createTrimeshObject("terrain.mesh", Ogre::Vector3(-130,0,0));
    
    Ogre::StaticGeometry* sg = new Ogre::StaticGeometry(sm, "static terrain");
    sg->addEntity(m_terrain, Ogre::Vector3(0, -0.01, 0), Ogre::Quaternion(
            Ogre::Degree(-90), Ogre::Vector3::UNIT_X));

    sg->build();
    
//    raySceneQuery = mSceneMgr->createRayQuery(Ray(mCamera->getPosition(), Vector3::NEGATIVE_UNIT_Y));
//    raySceneQueryListener = new myRaySceneQueryListener;
//    Ogre::RaySceneQueryListener
    initPagedGeometry();

    // convex hull object mesh to shape converter//
    dom->createConvexObject("stone.mesh", 8, Ogre::Vector3(35, 4, 0));
    dom->createConvexObject("stone2.mesh", 11, Ogre::Vector3(40, 4, 0));

    // this is the box stack
   for (int j = 0; j < 3; j++)
       for (int i = 0; i < BOX_COUNT; i++)
           dom->createBox("QuestionCube.mesh", 5, Ogre::Vector3(
                   2 + i+ (i*0.1), 0.5 + j , 0));

    for (int i = 0; i < 4; i++)
        som->createBox("NormalCube.mesh", Ogre::Vector3(26 + i, 5, 0));
    for (int i = 0; i < 5; i++)
    {
        som->createBox("NormalCube.mesh", Ogre::Vector3(17 + i, 9, 0));
        som->createBox("NormalCube.mesh", Ogre::Vector3(26 + i, 13, 0));
    }

    for (int i = 0; i < 3; i++)
    {
        som->createBox("NormalCube.mesh", Ogre::Vector3(10 + i, 11, 0));
        som->createBox("BrownCube.mesh", Ogre::Vector3(70, 0.5+i, 0));
    }

    som->createTube("Tube.mesh", 
            Ogre::Vector3::UNIT_Z, Ogre::Vector3(31.5, 15, 0))->
                setCollisionHandler(new TubeCollisionHandler);

    som->createBox("QuestionCube.mesh", Ogre::Vector3(30, 5, 0))->
            setCollisionHandler(new CubeSpawnCollisionHandler);

    som->createBox("QuestionCube.mesh", Ogre::Vector3(9, 16, 0))->
            setCollisionHandler(new CubeSpawnCollisionHandler);

    //Wippe eng:seesaw
    som->createConvexObject("seesaw.mesh", Ogre::Vector3(64, 0, 0));

    dom->createBox("Plane.mesh", 8, Ogre::Vector3(64, 2.2, 0));

    // Character
    m_character = new Character("player", "Player.mesh");
    m_movementInputController = new CharacterMovementController(m_character);
    InputHandler::get()->addInputController(m_movementInputController);

    GraphicsManager::get()->root()->addFrameListener(new CameraSmoothFollow(
            GraphicsManager::get()->camera(),
            m_character->sceneNode()));  
    
    // create the ray scene query object
    m_raySceneQuery = sm->createRayQuery(Ogre::Ray(Ogre::Vector3(3, 10, -10),Ogre::Vector3::NEGATIVE_UNIT_Y),Ogre::SceneManager::STATICGEOMETRY_TYPE_MASK);

    if (NULL == m_raySceneQuery)
    {
        std::cout << "Failed to create Ogre::RaySceneQuery instance" << std::endl;
      return ;
    }
    m_raySceneQuery->setSortByDistance(true); 
}

float SceneryTest::getTerrainHeight(float p_x, float p_z)
{
    Ogre::Vector3 point = Ogre::Vector3(p_x,100,p_z);
    Ogre::Vector3 direction = Ogre::Vector3::NEGATIVE_UNIT_Y;
    Ogre::Vector3 pos = Ogre::Vector3::ZERO;

    if(dynamic_cast<SceneryTest*>(GraphicsManager::get()->scenery())->raycastFromPoint(point,direction,pos))
        return pos.y;

    return 0.0;
}

void
SceneryTest::initPagedGeometry()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();

    // TREESss

    //Initialize the PagedGeometry engine
    trees = new PagedGeometry();
    trees->setCamera(gm->camera());
    trees->setPageSize(50);
    //trees->setBounds(TBounds(-150, -150, 150, 150));

    //Set up LODs
    //trees->addDetailLevel<EntityPage>(10);
    trees->addDetailLevel<BatchPage>(100);
    trees->addDetailLevel<ImpostorPage>(700, 50);

    //Set up a TreeLoader for easy use
    //                                                         x,  z,  x,  z
    TreeLoader2D *treeLoader = new TreeLoader2D(trees, TBounds(-150, -150, 150, 150));
    
    trees->setPageLoader(treeLoader);
    treeLoader->setHeightFunction(&getTerrainHeight);

    //And add 20,000 trees to the scene through the "easy-to-use" TreeLoader class
    Ogre::Entity *myTree = sm->createEntity("MyTree", "tree2.mesh");
    float x = 0.0, z = -10.0, yaw = 0.0;

    for (int i = 0; i < 15; i++)
    {        
        if (x < 0) x = 0; else if (x > 150) x = 150;
        if (z > -10) z = -10; else if (z < -25) z = -20;
        yaw = Ogre::Math::RangeRandom(0, 360);
        treeLoader->addTree(myTree, Ogre::Vector2(x, z), Ogre::Degree(yaw), 0.5);
        yaw = Ogre::Math::RangeRandom(0, 360);
        treeLoader->addTree(myTree, Ogre::Vector2(x+5, z-5), Ogre::Degree(yaw), 1);
        yaw = Ogre::Math::RangeRandom(0, 360);
        treeLoader->addTree(myTree, Ogre::Vector2(x+2.5, z-10), Ogre::Degree(yaw), 2);
        yaw = Ogre::Math::RangeRandom(0, 360);
        treeLoader->addTree(myTree, Ogre::Vector2(x+7.5, z-10), Ogre::Degree(yaw), 2);
        
        x += 10.0;
    }

    //}

    //Grass
    //Initialize the PagedGeometry engine
    grass = new PagedGeometry(gm->camera(), 50);
    
    //Set up LODs
    grass->addDetailLevel<GrassPage>(100);

    //Set up a TreeLoader for easy use//
    GrassLoader* grassLoader = new GrassLoader(grass);
    grass->setPageLoader(grassLoader);
    grassLoader->setHeightFunction(&getTerrainHeight);

    GrassLayer* grassLayer = grassLoader->addLayer("grass");

    grassLayer->setAnimationEnabled(true);
    grassLayer->setSwaySpeed(0.1f);
    grassLayer->setSwayLength(0.5f);
    grassLayer->setSwayDistribution(10.0f);

    grassLayer->setDensity(0.1f);
    //grassLayer->setDensityMap("GrassDensityMap.png");
    //grassLayer->setMapBounds(TBounds(-150, -150, 150, 150));

    //grassLayer->setMinimumSize(0.5,0.5);
    //grassLayer->setMaximumSize(1.0, 1.0); 

    grassLayer->setMinimumSize(4.0,2.0);
    grassLayer->setMaximumSize(4.0,2.0);

    grassLayer->setFadeTechnique(FADETECH_GROW);

//    GrassFrameListener* grassFrameListener = new GrassFrameListener(grassLoader);
//    GraphicsManager::get()->root()->addFrameListener(grassFrameListener);
}

void SceneryTest::getStaticGeometry(
        Ogre::StaticGeometry* mesh,
        size_t &overtex_count,
        Ogre::Vector3* &overtices,
        size_t &oindex_count,
        unsigned long* &oindices,
        const Ogre::Vector3 &position,
        const Ogre::Quaternion &orient,
        const Ogre::Vector3 &scale)
{
    //  iterate regions
    std::vector<Ogre::Vector3> vertices;
    std::vector<unsigned int> indices;
    size_t next_offset = 0;

    Ogre::StaticGeometry::RegionIterator iter(mesh->getRegionIterator());
    while (iter.hasMoreElements()) {

        Ogre::StaticGeometry::Region *r = iter.getNext();
        Ogre::Vector3 center = r->getCentre();
        Ogre::StaticGeometry::Region::LODIterator lit(r->getLODIterator());

        //  use the closest LOD of each block
        float sqdist = 1e24;
        Ogre::StaticGeometry::LODBucket *theBucket = 0;

        while (lit.hasMoreElements()) {

            Ogre::StaticGeometry::LODBucket *b = lit.getNext();
            if (!theBucket || b->getSquaredDistance() < sqdist) {
                sqdist = b->getSquaredDistance();
                theBucket = b;
            }
        }

        Ogre::StaticGeometry::LODBucket::MaterialIterator mit(theBucket->getMaterialIterator());

        while (mit.hasMoreElements()) {

            Ogre::StaticGeometry::MaterialBucket *mb = mit.getNext();
            Ogre::StaticGeometry::MaterialBucket::GeometryIterator git(mb->getGeometryIterator());
            while (git.hasMoreElements()) {

                int cv = 0;
                Ogre::StaticGeometry::GeometryBucket *gb = git.getNext();

                Ogre::VertexData const *vertex_data = gb->getVertexData();

                const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
                Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
                unsigned char* vertex = reinterpret_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                vertices.resize(vertices.size() + vertex_data->vertexCount);
                posElem->baseVertexPointerToElement(vertex, reinterpret_cast<Ogre::Real **>(&vertex));
                for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize()) {
                    ++cv;
                    Ogre::Vector3 pt = *(Ogre::Vector3 const *)vertex;
                    vertices[next_offset + j] = (orient * (pt * scale)) + position;// + center;
                }
                std::cout << "vertices: " << cv << std::endl;
                vbuf->unlock();

                Ogre::IndexData const* index_data = gb->getIndexData();

                Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
                bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
                void *pIndex = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

                for(size_t k = 0; k < index_data->indexCount; ++k) {

                    unsigned int vindex = use32bitindexes ? *(unsigned int *)pIndex : *(unsigned short *)pIndex;
                    pIndex = (char *)pIndex + (use32bitindexes ? 4 : 2);
                    indices.push_back(vindex + next_offset);
                }
                ibuf->unlock();

                next_offset += vertex_data->vertexCount;
            }
        }
    }

    overtex_count = vertices.size();
    overtices = 0;
    if (overtex_count > 0) {
        overtices = new Ogre::Vector3[overtex_count];
        memcpy(overtices, &vertices[0], sizeof(*overtices)*overtex_count);
    }
    oindex_count = indices.size();
    oindices = 0;
    if (oindex_count > 0) {
        oindices = new unsigned long[oindex_count];
        memcpy(oindices, &indices[0], sizeof(*oindices)*oindex_count);
    }
} 

// raycast from a point in to the scene.
// returns success or failure.
// on success the point is returned in the result.
bool 
SceneryTest::raycastFromPoint(const Ogre::Vector3 &point,
        const Ogre::Vector3 &normal,
        Ogre::Vector3 &result)
{
    //DEBUG
    int hitCheckCount = 0;
    // create the ray to test
    Ogre::Ray ray(Ogre::Vector3(point.x, point.y, point.z),
            Ogre::Vector3(normal.x, normal.y, normal.z));

    // check we are initialised
    if (m_raySceneQuery != NULL)
    {
        // create a query object
        m_raySceneQuery->setRay(ray);

        // execute the query, returns a vector of hits

        if (m_raySceneQuery->execute().size() <= 0)
        {
            // raycast did not hit an objects bounding box
            return (false);
        }
    }
    else
    {
        std::cout << "Cannot raycast without RaySceneQuery instance" << std::cout;
        return (false);
    }   

    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real closest_distance = -1.0f;
    Ogre::Vector3 closest_result;
    Ogre::RaySceneQueryResult &query_result = m_raySceneQuery->getLastResults();
    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) &&
                (closest_distance < query_result[qr_idx].distance))
        {
            break;
        }

        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL) &&
                (query_result[qr_idx].movable->getMovableType().compare("StaticGeometry") == 0))
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);
            Ogre::StaticGeometry::Region *rg = static_cast<Ogre::StaticGeometry::Region*>(query_result[qr_idx].movable);
            
            // mesh data to retrieve         
            size_t vertex_count;
            size_t index_count;
            Ogre::Vector3 *vertices;
            unsigned long *indices;

            // get the mesh information
            getStaticGeometry(rg->getParent(), vertex_count, vertices, index_count, indices,             
                    pentity->getParentNode()->getWorldPosition(),
                    pentity->getParentNode()->getWorldOrientation(),
                    pentity->getParentNode()->getScale());

            // test for hitting individual triangles on the mesh
            bool new_closest_found = false;
            for (int i = 0; i < static_cast<int>(index_count); i += 3)
            {
                // check for a hit against this triangle
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
                        vertices[indices[i+1]], vertices[indices[i+2]], true, false);
                ++hitCheckCount;
                // if it was a hit check if its the closest
                if (hit.first)
                {
                    std::cout << "hitcheck: " << hitCheckCount << std::endl;
                    if ((closest_distance < 0.0f) ||
                            (hit.second < closest_distance))
                    {
                        // this is the closest so far, save it off
                        closest_distance = hit.second;
                        new_closest_found = true;
                        //break added for CoAJnR as theirs alwas just one hit
                        break;
                    }
                }
            }

            // free the verticies and indicies memory
            delete[] vertices;
            delete[] indices;

            // if we found a new closest raycast for this object, update the
            // closest_result before moving on to the next object.
            if (new_closest_found)
            {
                closest_result = ray.getPoint(closest_distance);               
            }
        }       
    }
    std::cout << "check: " << hitCheckCount << std::endl;

    // return the result
    if (closest_distance >= 0.0f)
    {
        // raycast success
        //std::cout << "success " << closest_result << std::endl;;
        result = closest_result;
        return (true);
    }
    else
    {
        // raycast failed
        return (false);
    } 

}

void 
SceneryTest::getMeshInformation(const Ogre::MeshPtr p_mesh,
        size_t &p_vertex_count,
        Ogre::Vector3* &p_vertices,
        size_t &p_index_count,
        unsigned long* &p_indices,
        const Ogre::Vector3 &p_position,
        const Ogre::Quaternion &p_orient,
        const Ogre::Vector3 &p_scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    p_vertex_count = p_index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < p_mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = p_mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                p_vertex_count += p_mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            p_vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        p_index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    p_vertices = new Ogre::Vector3[p_vertex_count];
    p_indices = new unsigned long[p_index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < p_mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = p_mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? p_mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
                //  as second argument. So make it float, to avoid trouble when Ogre::Real will
                //  be comiled/typedefed as double:
                //      Ogre::Real* pReal;
                float* pReal;

                for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
                {
                    posElem->baseVertexPointerToElement(vertex, &pReal);

                    Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                    p_vertices[current_offset + j] = (p_orient * (pt * p_scale)) + p_position;
                }

                vbuf->unlock();
                next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                p_indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                p_indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
} 

void
SceneryTest::cleanup()
{
    //  delete DynamicObjectManager::get();//
    //  delete StaticObjectManager::get();
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();

    InputHandler::get()->removeInputController(m_movementInputController);
    //TODO: make destroy function
    delete m_movementInputController;

    // cleaning up ogre objects
    delete m_standardBoxShape;
    m_standardBoxShape = 0;

    sm->destroyLight("point-light");
    m_light = 0;

    sm->destroyEntity("plane");
    delete m_baseLevelBody;
    m_baseLevelBody = 0;
    m_baseLevelShape = 0;
    m_baseLevelNode = 0;
    m_baseLevel = 0;

    sm->destroyEntity(m_terrain);
    m_terrain = 0;

    delete StaticObjectManager::get();
    delete DynamicObjectManager::get();
}

}

