#include "SceneryTest.h"

#include "CameraSmoothFollow.h"
#include "Character.h"
#include "CharacterMovementController.h"
#include "GraphicsManager.h"
#include "InputHandler.h"
#include "PhysicsManager.h"

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
    m_plane = sm->createEntity("plane", "level2.mesh");
    m_plane->setNormaliseNormals(true);

    m_planeNode = sm->getRootSceneNode()->
            createChildSceneNode("plane-node");

    m_planeNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(-90));

    // m_planeShape = new OgreBulletCollisions::StaticPlaneCollisionShape(
    //         Ogre::Vector3(0,1,0), 0.97);

    Ogre::Matrix4 transform;
    m_planeNode->getWorldTransforms(&transform);
    OgreBulletCollisions::MeshToShapeConverter converter(m_plane, transform);
    m_planeShape = converter.createTrimesh();
    
    m_planeBody = new OgreBulletDynamics::RigidBody(
            "plane-body", PhysicsManager::get()->world());

    m_planeNode->attachObject(m_plane);
    m_planeBody->setStaticShape(
            m_planeShape,
            0.0,   // restitution 
            0.5  // friction
            );

    // questionmark box
    m_box = sm->createEntity("QBox" , "QuestionCube.mesh");
    m_box->setNormaliseNormals(true);
        
    m_boxNode = sm->getRootSceneNode()->createChildSceneNode("QBoxNode");
    
    m_boxNode->attachObject(m_box);
    
    m_boxBody = new OgreBulletDynamics::RigidBody("BoxBody", PhysicsManager::get()->world());
    m_boxBody->setStaticShape(
            m_boxNode,
            new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1, 1, 1)),
            0.0,
            0.0,
            Ogre::Vector3(4, 8, 0)
            );
    
    // Character
    m_character = new Character("player", "Cube.mesh");
    m_movementInputController = new CharacterMovementController(m_character);
    InputHandler::get()->addInputController(m_movementInputController);

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
