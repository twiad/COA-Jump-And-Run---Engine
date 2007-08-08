#include "SceneryTest.h"

#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "InputHandler.h"
#include "MovementInputController.h"

namespace CoABlaster
{

SceneryTest::SceneryTest()
{
    m_cubeNode = 0;
    m_planeNode = 0;
    
    m_cube = 0;
    m_plane = 0;
    
    m_light = 0;
    
    m_cubeShape = 0;
    m_planeShape = 0;
    
    m_cubeBody = 0;
    m_planeBody = 0;

    m_movementInputController = 0;
}

SceneryTest::~SceneryTest()
{
    
}

void
SceneryTest::setup()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();
    
    gm->camera()->setPosition(Ogre::Vector3(20, 7, 20));
    gm->camera()->lookAt(Ogre::Vector3(0, 0, 0));
    
    gm->viewport()->setBackgroundColour(Ogre::ColourValue( 0.8, 0.8, 0.85));
    
    sm->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
    
    // light
    m_light = sm->createLight("point-light");
    m_light->setType(Ogre::Light::LT_POINT);
    m_light->setPosition(Ogre::Vector3(10, 20, 20));


    // cube
    m_cube = sm->createEntity("cube", "Cube.mesh");
    m_cube->setNormaliseNormals(true);

    m_cubeShape = new OgreBulletCollisions::BoxCollisionShape(
            Ogre::Vector3(1, 1, 1));
    
    m_cubeBody = new OgreBulletDynamics::RigidBody(
                    "cube-body", PhysicsManager::get()->world());

    m_cubeNode = sm->getRootSceneNode()->createChildSceneNode("cube-node");
    m_cubeNode->setPosition(Ogre::Vector3(0, 7, 0));

    m_cubeNode->attachObject(m_cube);
    m_cubeBody->setShape(
            m_cubeNode, 
            m_cubeShape, 
            0.1, // restitution 
            0.9, // friction
            5,   // mass
            Ogre::Vector3(0, 7, 0));


    // plane
    m_plane = sm->createEntity("plane", "Plane.mesh");
    m_plane->setNormaliseNormals(true);

    m_planeShape = new OgreBulletCollisions::StaticPlaneCollisionShape(
            Ogre::Vector3(0,1,0), 0.97);

    m_planeNode = sm->getRootSceneNode()->
            createChildSceneNode("plane-node");

    m_planeNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));
    
    m_planeBody = new OgreBulletDynamics::RigidBody(
            "plane-body", PhysicsManager::get()->world());

    m_planeNode->attachObject(m_plane);
    m_planeBody->setStaticShape(
            m_planeShape,
            1,   // restitution 
            0.9  // friction
            );

    m_movementInputController = new MovementInputController(m_cubeBody);

    InputHandler::get()->addInputController(m_movementInputController);
}

void
SceneryTest::cleanup()
{
    GraphicsManager* gm = GraphicsManager::get();
    Ogre::SceneManager* sm = gm->sceneManager();

    InputHandler::get()->removeInputController(m_movementInputController);
    delete m_movementInputController;

    // cleaning up ogre objects
    sm->destroyEntity("cube");
    m_cube = 0;
    
    sm->destroyEntity("plane");
    m_plane = 0;
    
    sm->destroySceneNode("cube-node");
    m_cubeNode = 0;

    sm->destroySceneNode("plane-node");
    m_planeNode = 0;

    sm->destroyLight("point-light");
    m_light = 0;

    // cleaning up ogrebullet objects
    delete m_cubeBody;
    m_cubeBody = 0;
    m_cubeShape = 0;
    
    delete m_planeBody;
    m_planeBody = 0;
    m_planeShape = 0;
}


}