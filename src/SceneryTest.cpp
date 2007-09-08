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
    m_planeNode = 0;
    m_plane = 0;
    m_light = 0;
    m_planeShape = 0;
    m_planeBody = 0;
    m_movementInputController = 0;
    m_character = 0;
    m_rot = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
    m_standardBoxShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5, 0.5, 0.5));
}

SceneryTest::~SceneryTest()
{
    
}

void
SceneryTest::setup()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();
    StaticObjectManager* som = StaticObjectManager::get();

    gm->camera()->setPosition(Ogre::Vector3(-16, 15, 23));
    gm->camera()->lookAt(Ogre::Vector3(0, 0, 0));

    sm->setSkyBox(true, "CoAJnR/SkyBox/miramar");
    
    gm->viewport()->setBackgroundColour(Ogre::ColourValue( 0.8, 0.8, 0.85));
    
    sm->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));

    OgreBulletCollisions::CollisionShape* floorShape = 
            new OgreBulletCollisions::StaticPlaneCollisionShape(
                    Ogre::Vector3(0, 1, 0), -2);

    OgreBulletDynamics::RigidBody* floorBody = 
            new OgreBulletDynamics::RigidBody(
                    "floor", PhysicsManager::get()->world());


    floorBody->setStaticShape(floorShape, 1, 1, Ogre::Vector3(0, 1, 0));
    floorBody->setCollisionHandler(new DestroyTouchingObjectsCollisionHandler);

    
    // light
    m_light = sm->createLight("point-light");
    m_light->setType(Ogre::Light::LT_POINT);
    m_light->setPosition(Ogre::Vector3(10, 20, 20));

    // plane
    // m_plane = sm->createEntity("plane", "ground.mesh");
    m_plane = sm->createEntity("plane", "Level4.mesh");
    m_plane->setNormaliseNormals(true);
    
    m_planeNode = sm->getRootSceneNode()->
            createChildSceneNode("plane-node");

    m_terrain = sm->createEntity("terrain", "terrain.mesh");
    m_terrain->setNormaliseNormals(true);
    // 
    // m_terrainNode = sm->getRootSceneNode()->
    //         createChildSceneNode("terrain-node");
    // m_terrainNode->attachObject(m_terrain);
    // m_terrainNode->rotate(
    //     Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X));
    // m_terrainNode->setPosition(0,-10,-30);
    // 
    Ogre::StaticGeometry* sg = new Ogre::StaticGeometry(sm, "static terrain");
    sg->addEntity(m_terrain, Ogre::Vector3(0,-0.01,0), 
        Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X));
    
    sg->build();

    OgreBulletCollisions::MeshToShapeConverter converter(m_plane);

    m_planeShape = converter.createTrimesh();
    
    m_planeBody = new OgreBulletDynamics::RigidBody(
            "plane-body", PhysicsManager::get()->world());
    
    m_planeNode->attachObject(m_plane);
    m_planeBody->setStaticShape(
    		m_planeNode,
            m_planeShape,
            0.0,   // restitution 
            0.5,  // friction
            Ogre::Vector3(0, 0, 0),
            Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
            );
    
    m_planeBody->setPosition(Ogre::Vector3(0,0,0));
    m_planeBody->getBulletRigidBody()->getWorldTransform().setOrigin(btVector3(0,0,0));

    // // gimpact test
    //     Ogre::Entity* gimpactEnt = GraphicsManager::get()->sceneManager()->
    //             createEntity("gimpact-test-entity", "QuestionCube.mesh");
    //     Ogre::SceneNode* gimpactNode = sm->getRootSceneNode()->
    //                 createChildSceneNode("gimpact-test-node");
    //     OgreBulletDynamics::RigidBody* gimpactBody = new OgreBulletDynamics::RigidBody(
    //             "gimpact-test-body", PhysicsManager::get()->world());
    // 
    //     OgreBulletCollisions::MeshToShapeConverter gimpactConv(gimpactEnt);
    //     
    //     gimpactBody->setShape(
    //              gimpactNode,
    //                 gimpactConv.createGImpact(),
    //                 0.0,   // restitution 
    //                 0.5,  // friction
    //                 0.5,  // friction
    //                 Ogre::Vector3(10, 1, 0),
    //                 Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
    //                 );
    //     
    //     gimpactNode->attachObject(gimpactEnt);
    //     
    //     Ogre::Quaternion q(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
    //     DynamicObject* d = new DynamicObject(
    //                     "QBox1123123", 
    //                     "QuestionCube.mesh",
    //                     Ogre::Vector3(22, 1, 0),
    //                     q
    //                  );
    //     
    //      OgreBulletDynamics::PointToPointConstraint* constr = new OgreBulletDynamics::PointToPointConstraint(
    //          gimpactBody, 
    //          d, 
    //          Ogre::Vector3::ZERO,            
    //          Ogre::Vector3(3,0,0));
    // 
    //      PhysicsManager::get()->world()->addConstraint(constr);
    // 


    // convex hull object hardcoded
    Ogre::Real points[8][3] = 
    { 
        {0.5,0.5,0.5},
        {0.5,0.5,-0.5},
        {0.5,-0.5,0.5},
        {0.5,-0.5,-0.5},
        {-0.5,0.5,0.5},
        {-0.5,0.5,-0.5},
        {-0.5,-0.5,0.5},
        {-0.5,-0.5,-0.5}
    };
    
    new DynamicObject(
         "QBox-convex", 
            "QuestionCube.mesh",
            new OgreBulletCollisions::ConvexHullCollisionShape((Ogre::Real*)points, 8, 3*sizeof(Ogre::Real)),
            5,
            Ogre::Vector3(13.5, 5, 0),
         m_rot
         );


     // convex hull object mesh to shape converter
     Ogre::Entity* convexEnt = GraphicsManager::get()->sceneManager()->
             createEntity("convex-test-entity", "stone.mesh");
     Ogre::SceneNode* convexNode = sm->getRootSceneNode()->
                 createChildSceneNode("convex-test-node");
     OgreBulletDynamics::RigidBody* convexBody = new OgreBulletDynamics::RigidBody(
             "convex-test-body", PhysicsManager::get()->world());

     OgreBulletCollisions::MeshToShapeConverter convexConv(convexEnt);

     convexBody->setShape(
              convexNode,
                 convexConv.createConvex(),
                 2.0,   // restitution 
                 2.0,  // friction
                 8,  // mass
                 Ogre::Vector3(35, 4, 0),
                 Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
                 );

     convexNode->attachObject(convexEnt);

     Ogre::Entity* convex2Ent = GraphicsManager::get()->sceneManager()->
             createEntity("convex2-test-entity", "stone2.mesh");
     Ogre::SceneNode* convex2Node = sm->getRootSceneNode()->
                 createChildSceneNode("convex2-test-node");
     OgreBulletDynamics::RigidBody* convex2Body = new OgreBulletDynamics::RigidBody(
             "convex2-test-body", PhysicsManager::get()->world());

     OgreBulletCollisions::MeshToShapeConverter convex2Conv(convex2Ent);

     convex2Body->setShape(
              convex2Node,
                 convex2Conv.createConvex(),
                 2.0,   // restitution 
                 2.0,  // friction
                 11,  // mass
                 Ogre::Vector3(40, 4, 0),
                 Ogre::Quaternion(Ogre::Degree(20), Ogre::Vector3::UNIT_Y)
                 );

     convex2Node->attachObject(convex2Ent);


        
    // this is the box stack
    for(int j = 0; j < 3; j++)
        for(int i = 0; i < BOX_COUNT; i++)
        {
            new DynamicObject(
                 "QBox" + Ogre::StringConverter::toString(j) + 
                    " " + Ogre::StringConverter::toString(i), 
                    "QuestionCube.mesh",
                    new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5, 0.5, 0.5)),
                    5,
                    Ogre::Vector3(2 + i + (i*0.3), 1 + j + (j*0.1), 0),
                 m_rot
                 );
        }

    for(int i = 0; i < 4; i++)
    	{
    		som->createBox("NormalCube.mesh", Ogre::Vector3(26 + i, 5, 0));
    	}
    
    for(int i = 0; i < 5; i++)
    	{
    		som->createBox("NormalCube.mesh", Ogre::Vector3(17 + i, 9, 0));
    		som->createBox("NormalCube.mesh", Ogre::Vector3(26 + i, 13, 0));
		}
    
    for(int i = 0; i < 7; i++)
    	{
        	som->createBox("NormalCube.mesh", Ogre::Vector3(10 + i, 11, 0));
        	som->createBox("BrownCube.mesh", Ogre::Vector3(70, 0.5+i, 0));
    	}
    
    /// @todo TODO: defaultParam?
    StaticObject* T1 = som->createTube("Tube.mesh", Ogre::Vector3(31.5, 15, 0));

    Ogre::Quaternion m_trot = Ogre::Quaternion(Ogre::Degree(-180), (Ogre::Vector3::UNIT_Z)); 
    T1->setOrientation(m_trot*m_rot);
    T1->setCollisionHandler(new TubeCollisionHandler);
    
    /// @todo TODO: collisionHandler as Param?
    som->createBox("QuestionCube.mesh", Ogre::Vector3(30, 5, 0))
       ->setCollisionHandler(new CubeSpawnCollisionHandler);
    
    som->createBox("QuestionCube.mesh", Ogre::Vector3(9, 16, 0))
       ->setCollisionHandler(new CubeSpawnCollisionHandler);
    
    //Wippe eng:seesaw
    som->createConvexObject("seesaw.mesh",Ogre::Vector3(64, 0, 0));
    
    new DynamicObject(
    		"seesawplane", 
    		"Plane.mesh",
            new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(4, 1, 0.1)),
            5,
            Ogre::Vector3(64, 2.2, 0),
            m_rot
            );
    
    // Character
    m_character = new Character("player", "Player.mesh");
    m_movementInputController = new CharacterMovementController(m_character);
    InputHandler::get()->addInputController(m_movementInputController);
    // 
    GraphicsManager::get()->root()->addFrameListener(
            new CameraSmoothFollow(
                    GraphicsManager::get()->camera(), 
                    m_character->sceneNode()));
}

void
SceneryTest::cleanup()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();

    InputHandler::get()->removeInputController(m_movementInputController);
    delete m_movementInputController;

    // cleaning up ogre objects
    sm->destroyEntity("plane");
    m_plane = 0;

    sm->destroySceneNode("plane-node");
    m_planeNode = 0;

    sm->destroyLight("point-light");
    m_light = 0;
    
    delete m_planeBody;
    m_planeBody = 0;
    m_planeShape = 0;
    
}

}
