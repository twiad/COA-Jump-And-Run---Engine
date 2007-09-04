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
    

    gm->camera()->setPosition(Ogre::Vector3(-16, 15, 23));
    gm->camera()->lookAt(Ogre::Vector3(0, 0, 0));

    sm->setSkyBox(true, "CoAJnR/SkyBox/miramar");
    
    gm->viewport()->setBackgroundColour(Ogre::ColourValue( 0.8, 0.8, 0.85));
    
    sm->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));

    // sm->setWorldGeometry("terrain.cfg");

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
    sg->addEntity(m_terrain, Ogre::Vector3(0,-10,-30), 
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


    // // this is the constraint test
    // {
    //     m_testConstraint[0] = sm->createEntity("const0", "QuestionCube.mesh");
    //     m_testConstraint[0]->setNormaliseNormals(true);
    // 
    //     m_testConstraintNodes[0] = sm->getRootSceneNode()->
    //             createChildSceneNode("constnode0");
    // 
    //     m_testConstraintBodies[0] = new OgreBulletDynamics::RigidBody(
    //             "constbody0", 
    //             PhysicsManager::get()->world());
    // 
    //     m_testConstraintBodies[0]->setShape(
    //         m_testConstraintNodes[0], 
    //         new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5,0.5,0.5)), 
    //         2.0, /* ............................................. restitution */
    //         2.0, /* ............................................. friction    */
    //         1,   /* ............................................. mass        */
    //         Ogre::Vector3(-2, 9, 0));    
    // 
    //     m_testConstraintNodes[0]->attachObject(m_testConstraint[0]);
    // 
    //     m_testConstraint[1] = sm->createEntity("const1", "QuestionCube.mesh");
    //     m_testConstraint[1]->setNormaliseNormals(true);
    // 
    //     m_testConstraintNodes[1] = sm->getRootSceneNode()->
    //             createChildSceneNode("constnode1");
    // 
    //     m_testConstraintBodies[1] = new OgreBulletDynamics::RigidBody(
    //             "constbody1", 
    //             PhysicsManager::get()->world());
    // 
    //     m_testConstraintBodies[1]->setShape(
    //         m_testConstraintNodes[1], 
    //         new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5,0.5,0.5)), 
    //         2.0, /* ............................................. restitution */
    //         2.0, /* ............................................. friction    */
    //         1,   /* ............................................. mass        */
    //         Ogre::Vector3(-2, 5, 0));    
    // 
    //     m_testConstraintNodes[1]->attachObject(m_testConstraint[1]);
    //     
    //     // m_testConstraintObject = new OgreBulletDynamics::ConeTwistConstraint(
    //     //     m_testConstraintBodies[0], m_testConstraintBodies[1], 
    //     //             Ogre::Vector3(0,0,0),
    //     //             Ogre::Quaternion::IDENTITY,
    //     //             Ogre::Vector3(3,0,0),
    //     //             Ogre::Quaternion::IDENTITY);
    // 
    //     // m_testConstraintObject = new OgreBulletDynamics::SixDofConstraint(
    //     //     m_testConstraintBodies[0], m_testConstraintBodies[1], 
    //     //             Ogre::Vector3(0,0,0),
    //     //             Ogre::Quaternion::IDENTITY,
    //     //             Ogre::Vector3(0,4,0),
    //     //             Ogre::Quaternion::IDENTITY);
    // 
    //     m_testConstraintObject = new OgreBulletDynamics::HingeConstraint(
    //         m_testConstraintBodies[0], m_testConstraintBodies[1], 
    //                 Ogre::Vector3(0,5,0),
    //                 Ogre::Vector3(0,1,0),
    //                 Ogre::Vector3(0,1,0),
    //                 Ogre::Vector3(0,1,0));
    // 
    //     // m_testConstraintObject->setLinearUpperLimit(Ogre::Vector3(0,5,0));
    //     // m_testConstraintObject->setLinearLowerLimit(Ogre::Vector3(0,2,0));
    //     
    //     PhysicsManager::get()->world()->addConstraint(m_testConstraintObject);
    // }

 
    // this is the box stack
    for(int j = 0; j < 4; j++)
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
                new StaticObject(
                		"NBox" + Ogre::StringConverter::toString(i), 
                        "NormalCube.mesh",
                        m_standardBoxShape,
                        Ogre::Vector3(26 + i, 5, 0),
                		m_rot
                		);
            }

    for(int i = 0; i < 5; i++)
                {
                    new StaticObject(
                    		"NBox1" + Ogre::StringConverter::toString(i), 
                            "NormalCube.mesh",
                            m_standardBoxShape,
                            Ogre::Vector3(17 + i, 9, 0),
                    		m_rot
                    		);
                }
    
    for(int i = 0; i < 5; i++)
                {
                    new StaticObject(
                    		"NBox2" + Ogre::StringConverter::toString(i), 
                            "NormalCube.mesh",
                            m_standardBoxShape,
                            Ogre::Vector3(26 + i, 13, 0),
                    		m_rot
                    		);
                }
    
    for(int i = 0; i < 7; i++)
                {
                    new StaticObject(
                    		"NBox3" + Ogre::StringConverter::toString(i), 
                            "NormalCube.mesh",
                            m_standardBoxShape,
                            Ogre::Vector3(10 + i, 11, 0),
                    		m_rot
                    		);
                }
    OgreBulletCollisions::CollisionShape* m_tubeShape = 
    	new OgreBulletCollisions::CylinderCollisionShape(Ogre::Vector3(1, 1, 1),Ogre::Vector3::UNIT_Z);

    StaticObject* T1 = new StaticObject("Tube_1" , "Tube.mesh", m_tubeShape, Ogre::Vector3(31.5, 15, 0), m_rot);
    Ogre::Quaternion m_trot = Ogre::Quaternion(Ogre::Degree(-180), (Ogre::Vector3::UNIT_Z)); 
    T1->setOrientation(m_trot*m_rot);
    T1->setCollisionHandler(new TubeCollisionHandler);
    
    StaticObject* Q1 = new StaticObject("QBox_1" , "QuestionCube.mesh", m_standardBoxShape, Ogre::Vector3(30, 5, 0), m_rot);
    // tmp->setCollisionHandler(new DebugOutputCollisionHandler);
    Q1->setCollisionHandler(new CubeSpawnCollisionHandler);
    
    StaticObject* Q2 = new StaticObject("QBox_2" , "QuestionCube.mesh", m_standardBoxShape, Ogre::Vector3(9, 16, 0), m_rot);
    // tmp->setCollisionHandler(new DebugOutputCollisionHandler);
    Q2->setCollisionHandler(new CubeSpawnCollisionHandler);
    
    //new StaticObject("Wall","Plane.mesh",Ogre::Vector3(40, 5, 0),*m_rot);
    
    //DynamicObject* tmp2 = new DynamicObject("Wippe","Plane.mesh",Ogre::Vector3(40,8,0), m_rot);
    //tmp2->setOrientation(*new Ogre::Quaternion(Ogre::Degree(-90), (Ogre::Vector3::UNIT_Y)));
    
    //tmp2->setOrientation(*new Ogre::Quaternion(Ogre::Degree(-90), (Ogre::Vector3::UNIT_Y)));
    //tmp2->setOrientation(*new Ogre::Quaternion(Ogre::Degree(-90), (Ogre::Vector3::UNIT_Z)));
    
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
