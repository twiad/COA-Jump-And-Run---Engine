#include "SceneryTest.h"

#include "CameraSmoothFollow.h"
#include "Character.h"
#include "CharacterMovementController.h"
#include "GraphicsManager.h"
#include "InputHandler.h"
#include "PhysicsManager.h"
#include "StaticObject.h"
#include "DynamicObject.h"//
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
    //TODO: delete m_baseLevelBody;
    //TODO: delete m_character;
    //	delete m_baseLevelNode;
    //delete m_baseLevelShape;
    //    delete m_baseLevel;
    //
    //
    //    delete m_terrainNode;
    //    delete m_terrain;
    //
    //    delete m_light;
    //
    //    delete m_testConstraintObject;
    //
    //    delete m_movementInputController;
    //


    std::cout << " ~scenerytest"<< std::endl;
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
    m_baseLevel = sm->createEntity("plane", "Level4.mesh");
    m_baseLevel->setNormaliseNormals(true);

    m_baseLevelNode = sm->getRootSceneNode()->
    createChildSceneNode("plane-node");

    OgreBulletCollisions::MeshToShapeConverter converter(m_baseLevel);

    m_baseLevelShape = converter.createTrimesh();

    m_baseLevelBody = new OgreBulletDynamics::RigidBody(
            "plane-body", PhysicsManager::get()->world());

    m_baseLevelNode->attachObject(m_baseLevel);
    m_baseLevelBody->setStaticShape(m_baseLevelNode, m_baseLevelShape, 0.0, // restitution //
            0.5, // friction
            Ogre::Vector3(0, 0, 0), Ogre::Quaternion(Ogre::Degree(-90),
                    Ogre::Vector3::UNIT_X));

    m_baseLevelBody->setPosition(Ogre::Vector3(0, 0, 0));
    m_baseLevelBody->getBulletRigidBody()->getWorldTransform().setOrigin(btVector3(0, 0, 0));

    m_terrain = sm->createEntity("terrain", "terrain.mesh");
    m_terrain->setNormaliseNormals(true);

    Ogre::StaticGeometry* sg = new Ogre::StaticGeometry(sm, "static terrain");
    sg->addEntity(m_terrain, Ogre::Vector3(0, -0.01, 0), Ogre::Quaternion(
            Ogre::Degree(-90), Ogre::Vector3::UNIT_X));

    sg->build();

    // pagedBla();

    // convex hull object mesh to shape converter//
    dom->createConvexObject("stone.mesh", 8, Ogre::Vector3(35, 4, 0));
    dom->createConvexObject("stone2.mesh", 11, Ogre::Vector3(40, 4, 0));

    // this is the box stack
    for (int j = 0; j < 3; j++)
        for (int i = 0; i < BOX_COUNT; i++)
            dom->createBox("QuestionCube.mesh", 5, Ogre::Vector3(
                    2 + i+ (i*0.3), 1 + j + (j*0.1), 0));

    for (int i = 0; i < 4; i++)
        som->createBox("NormalCube.mesh", Ogre::Vector3(26 + i, 5, 0));

    for (int i = 0; i < 5; i++)
    {
        som->createBox("NormalCube.mesh", Ogre::Vector3(17 + i, 9, 0));
        som->createBox("NormalCube.mesh", Ogre::Vector3(26 + i, 13, 0));
    }

    for (int i = 0; i < 7; i++)
    {
        som->createBox("NormalCube.mesh", Ogre::Vector3(10 + i, 11, 0));
        som->createBox("BrownCube.mesh", Ogre::Vector3(70, 0.5+i, 0));
    }

    /// @todo TODO: defaultParam?//
    StaticObject* tube = som->createTube("Tube.mesh", Ogre::Vector3(31.5, 15, 0));

    tube->setOrientation(Ogre::Quaternion(Ogre::Degree(-180),
            (Ogre::Vector3::UNIT_Z))*m_rot);
    tube->setCollisionHandler(new TubeCollisionHandler);

    /// @todo TODO: collisionHandler as Param?
    StaticObject* spawnBox1 = som->createBox("QuestionCube.mesh", Ogre::Vector3(30, 5,
            0));

    spawnBox1->setCollisionHandler(new CubeSpawnCollisionHandler);

    StaticObject* spawnBox2 = som->createBox("QuestionCube.mesh", Ogre::Vector3(9, 16,
            0));

    spawnBox2->setCollisionHandler(new CubeSpawnCollisionHandler);

    //Wippe eng:seesaw
    som->createConvexObject("seesaw.mesh", Ogre::Vector3(64, 0, 0));

    dom->createConvexObject("Plane.mesh", 5, Ogre::Vector3(64, 2.2, 0));

    // Character
    m_character = new Character("player", "Player.mesh");
    m_movementInputController = new CharacterMovementController(m_character);
    InputHandler::get()->addInputController(m_movementInputController);

    GraphicsManager::get()->root()->addFrameListener(new CameraSmoothFollow(
            GraphicsManager::get()->camera(),
            m_character->sceneNode()));

}

float SceneryTest::getTerrainHeight(const float x, const float z)
{
    if (x< 0 || z< 0 || x > 1500 ||z > 1500) return 0;

    CoAJnR::GraphicsManager* gm = CoAJnR::GraphicsManager::get();

    gm->scenery()->updateRay.setOrigin(Ogre::Vector3(x, 0.0f, z));
    gm->scenery()->updateRay.setDirection(Ogre::Vector3::UNIT_Y);
    gm->scenery()->raySceneQuery->setRay(gm->scenery()->updateRay);
    gm->scenery()->raySceneQuery->execute(gm->scenery()->raySceneQueryListener);

    return gm->scenery()->raySceneQueryListener->resultDistance;
}

void
SceneryTest::cleanup()
{
    //	delete DynamicObjectManager::get();//
    //	delete StaticObjectManager::get();
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

void
SceneryTest::pagedBla()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();

    // TREESss

    //Initialize the PagedGeometry engine
    trees = new PagedGeometry();
    trees->setCamera(gm->camera());
    trees->setPageSize(50);
    trees->setBounds(TBounds(-100, -100, 1500, 1500));

    //Set up LODs
    //trees->addDetailLevel<EntityPage>(50);
    trees->addDetailLevel<BatchPage>(100, 50);
    trees->addDetailLevel<ImpostorPage>(700, 50);

    //Set up a TreeLoader for easy use 
    TreeLoader2D *treeLoader = new TreeLoader2D(trees, TBounds(-100, -100, 1500, 1500));
    trees->setPageLoader(treeLoader);
    treeLoader->setHeightFunction(&getTerrainHeight);

    //And add 20,000 trees to the scene through the "easy-to-use" TreeLoader class
    Ogre::Entity *myTree = sm->createEntity("MyTree", "tree2.mesh");
    float x = 0, z = 0, yaw, scale;
    /*
     for (int i = 0; i < 1000; i++){
     if (Ogre::Math::RangeRandom(0, 1) <= 0.8f){
     x += Ogre::Math::RangeRandom(-10.0f, 10.0f);
     z += Ogre::Math::RangeRandom(-10.0f, 10.0f);
     if (x < 0) x = 0; else if (x > 1500) x = 1500;
     if (z < 0) z = 0; else if (z > 1500) z = 1500;
     } else {
     x = Ogre::Math::RangeRandom(0, 1500);
     z = Ogre::Math::RangeRandom(0, 1500);
     }
     */
    yaw = Ogre::Math::RangeRandom(0, 360);
    scale = Ogre::Math::RangeRandom(0.9f, 1.1f);

    treeLoader->addTree(myTree, Ogre::Vector2(28, -5), Ogre::Degree(yaw), scale);
    //}

    //Grass
    grass = new PagedGeometry();
    //Initialize the PagedGeometry engine
    grass = new PagedGeometry(gm->camera(), 30);
    //trees = new PagedGeometry(camera, 50, TBounds(0, 0, 1500, 1500));

    //Set up LODs
    grass->addDetailLevel<GrassPage>(80);

    //Set up a TreeLoader for easy use
    GrassLoader* grassLoader = new GrassLoader(grass);
    grass->setPageLoader(grassLoader);
    grassLoader->setHeightFunction(&getTerrainHeight);

    GrassLayer* grassLayer = grassLoader->addLayer("grass");

    grassLayer->setAnimationEnabled(true);
    grassLayer->setSwaySpeed(0.5f);
    grassLayer->setSwayLength(0.5f);
    grassLayer->setSwayDistribution(10.0f);

    grassLayer->setDensity(0.2f);
    //grassLayer->setDensityMap("GrassDensityMap.png");
    grassLayer->setMapBounds(TBounds(0, 0, 150, 150));

    //grassLayer->setMinimumSize(0.5,0.5);
    //grassLayer->setMaximumSize(1.0, 1.0); 

    grassLayer->setMinimumSize(4.0,2.0);
    grassLayer->setMaximumSize(4.0,2.0);

    grassLayer->setFadeTechnique(FADETECH_GROW);

    //RaySceneQuery for terrain height checks
    updateRay.setOrigin(gm->camera()->getPosition());
    updateRay.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
    raySceneQuery = sm->createRayQuery(updateRay);
    raySceneQuery->setQueryTypeMask(Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK );
    raySceneQuery->setWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);
    raySceneQueryListener = new MyRaySceneQueryListener;

    GrassFrameListener* grassFrameListener = new GrassFrameListener(grassLoader);
    GraphicsManager::get()->root()->addFrameListener(grassFrameListener);
}

}
