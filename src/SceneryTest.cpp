#include "SceneryTest.h"

#include "CameraSmoothFollow.h"
#include "Character.h"
#include "CharacterMovementController.h"
#include "GraphicsManager.h"
#include "InputHandler.h"
#include "PhysicsManager.h"
#include "LevelObject.h"

namespace CoABlaster
{

SceneryTest::SceneryTest()
{
    m_planeNode = 0;
    m_boxNode = 0;
    m_plane = 0;
    m_box = 0;
    m_light = 0;
    m_planeShape = 0;
    m_planeBody = 0;
    m_movementInputController = 0;
    m_character = 0;
}

SceneryTest::~SceneryTest()
{
    
}

void
SceneryTest::setup()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();
    
    gm->camera()->setPosition(Ogre::Vector3(-16, -10, 23));
    gm->camera()->lookAt(Ogre::Vector3(0, 0, 0));
    
    gm->viewport()->setBackgroundColour(Ogre::ColourValue( 0.8, 0.8, 0.85));
    
    sm->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
    
    // light
    m_light = sm->createLight("point-light");
    m_light->setType(Ogre::Light::LT_POINT);
    m_light->setPosition(Ogre::Vector3(10, 20, 20));

    // plane
    // m_plane = sm->createEntity("plane", "ground.mesh");
    m_plane = sm->createEntity("plane", "Level3.mesh");
    m_plane->setNormaliseNormals(true);
    
    m_planeNode = sm->getRootSceneNode()->
            createChildSceneNode("plane-node");

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

    // questionmark box
    new LevelObject("NBox_1" , "NormalCube.mesh", Ogre::Vector3(4, 8, 0));
    LevelObject* tmp = 
        new LevelObject("QBox_1" , "QuestionCube.mesh", Ogre::Vector3(5, 8, 0));
    new LevelObject("NBox_2" , "NormalCube.mesh", Ogre::Vector3(6, 8, 0));

    tmp->setCollisionHandler(new DebugOutputCollisionHandler);


    // for(int i = 0; i < BOX_COUNT; i++)
    // {
    //     m_boxStack[i] = sm->createEntity(
    //             "QBox" + Ogre::StringConverter::toString(i), 
    //             "QuestionCube.mesh");
    //     m_boxStack[i]->setNormaliseNormals(true);
    // 
    //     m_boxStackNodes[i] = sm->getRootSceneNode()->
    //             createChildSceneNode("QBoxNode" + 
    //                     Ogre::StringConverter::toString(i));
    // 
    //     m_boxStackNodes[i]->attachObject(m_boxStack[i]);
    // 
    //     m_boxStackBodies[i] = new OgreBulletDynamics::RigidBody(
    //             "BoxBody" + Ogre::StringConverter::toString(i), 
    //             PhysicsManager::get()->world());
    //     
    //     m_boxStackBodies[i]->setShape(
    //             m_boxStackNodes[i] = sm->getRootSceneNode()->
    //                     createChildSceneNode("QBoxBodyNode" + 
    //                             Ogre::StringConverter::toString(i)),
    //             new OgreBulletCollisions::BoxCollisionShape(
    //                     Ogre::Vector3(1, 1, 1)),
    //             0.0, 0.0, 0.0, Ogre::Vector3(-i * 2, i, 0));
    // }
    
    // Character
    m_character = new Character("player", "Cube.mesh");
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
