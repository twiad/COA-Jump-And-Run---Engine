
#ifndef COABLASTER_SCENERYTEST_INCLUDED
#define COABLASTER_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "PhysicsManager.h"
#include "Scenery.h"

namespace CoABlaster
{

class SceneryTest : public Scenery
{

public:
    SceneryTest()
    {
        
    }
    
    virtual ~SceneryTest()
    {
        
    }
    
    virtual void setup()
    {
        GraphicsManager* gm = GraphicsManager::get();
        Ogre::SceneManager* sm = gm->sceneManager();
        
        gm->camera()->setPosition(Ogre::Vector3(20, 7, 20));
        gm->camera()->lookAt(Ogre::Vector3(0, 0, 0));
        
        gm->viewport()->setBackgroundColour(Ogre::ColourValue( 0.8, 0.8, 0.85));
        
        sm->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
        
        // light
        Ogre::Light* light = sm->createLight("point-light");
        light->setType(Ogre::Light::LT_POINT);
        light->setPosition(Ogre::Vector3(10, 20, 20));

        // cube
        // entity, mesh, tex, etc
        Ogre::Entity* cube = sm->createEntity("cube", "Cube.mesh");
        cube->setNormaliseNormals(true);

        // collision shape
        OgreBulletCollisions::BoxCollisionShape* cubeShape = 
                new OgreBulletCollisions::BoxCollisionShape(
                        Ogre::Vector3(2, 2, 2));
        
        // dynamics body
        OgreBulletDynamics::RigidBody* cubeBody = 
                new OgreBulletDynamics::RigidBody(
                        "cube-body", PhysicsManager::get()->world());

        // scene node
        Ogre::SceneNode* cubeNode = 
                sm->getRootSceneNode()->createChildSceneNode("cube-node");
        cubeNode->setPosition(Ogre::Vector3(0, 7, 0));

        // combine stuff
        cubeNode->attachObject(cube);
        cubeBody->setShape(
                cubeNode, cubeShape, 1, 1, 1, Ogre::Vector3(0, 7, 0));



        // plane
        Ogre::Entity* plane = sm->createEntity("plane", "Plane.mesh");
        plane->setNormaliseNormals(true);

        // collision shape
        OgreBulletCollisions::StaticPlaneCollisionShape* planeShape =
                new OgreBulletCollisions::StaticPlaneCollisionShape(
                        Ogre::Vector3(0,1,0), 0);

        // scene node
        Ogre::SceneNode* planeNode = 
                sm->getRootSceneNode()->createChildSceneNode("plane-node");

        planeNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));
        
        // body
        OgreBulletDynamics::RigidBody* planeBody = 
                new OgreBulletDynamics::RigidBody(
                        "plane-body", PhysicsManager::get()->world());

        // conbine
        planeNode->attachObject(plane);
        planeBody->setStaticShape(planeShape, 1, 1);

        
    }
    
    virtual void cleanup()
    {
        GraphicsManager* gm = GraphicsManager::get();
        Ogre::SceneManager* sm = gm->sceneManager();

        sm->destroyEntity("cube");
        sm->destroyEntity("plane");
        sm->destroySceneNode("cube-node");
        sm->destroySceneNode("plane-node");
        sm->destroyLight("point-light");
    }
};

}

#endif
