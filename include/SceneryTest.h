
#ifndef COABLASTER_SCENERYTEST_INCLUDED
#define COABLASTER_SCENERYTEST_INCLUDED

#include "Dependencies.h"

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
        
        sm->setAmbientLight(Ogre::ColourValue(0.9, 0.9, 0.9));

        // cube
        Ogre::Entity* cube = sm->createEntity("cube", "Cube.mesh");
        Ogre::SceneNode* cubeNode = 
                sm->getRootSceneNode()->createChildSceneNode("cube-node");
        cubeNode->attachObject(cube);
        cubeNode->setPosition(Ogre::Vector3(0, 3, 0));

        // plane
        Ogre::Entity* plane = sm->createEntity("plane", "Plane.mesh");
        Ogre::SceneNode* planeNode = 
                sm->getRootSceneNode()->createChildSceneNode("plane-node");
        planeNode->attachObject(plane);
        planeNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));
        
    }
    
    virtual void cleanup()
    {
        GraphicsManager* gm = GraphicsManager::get();
        Ogre::SceneManager* sm = gm->sceneManager();

        sm->destroyEntity("cube");
        sm->destroyEntity("plane");
        sm->destroySceneNode("cube-node");
        sm->destroySceneNode("plane-node");
    }
};

}

#endif
