
#ifndef COABLASTER_SCENERYTEST_INCLUDED
#define COABLASTER_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "Scenery.h"

namespace CoABlaster
{

#define BOX_COUNT

class InputController;

class SceneryTest : public Scenery
{
    Ogre::SceneNode* m_cubeNode;
    Ogre::SceneNode* m_planeNode;
    // Ogre::SceneNode* m_boxStackNodes[10];
    
    Ogre::Entity* m_cube;
    Ogre::Entity* m_plane;
    // Ogre::Entity* m_boxStack[10];
    
    Ogre::Light* m_light;
    
    OgreBulletCollisions::CollisionShape* m_cubeShape;
    OgreBulletCollisions::CollisionShape* m_planeShape;
    // OgreBulletCollisions::CollisionShape* m_boxStackShape[10];
    
    OgreBulletDynamics::RigidBody* m_cubeBody;
    OgreBulletDynamics::RigidBody* m_planeBody;
    // OgreBulletDynamics::RigidBody* m_boxStackBodies[10];

    InputController* m_movementInputController;
    
public:
    SceneryTest();
    
    virtual ~SceneryTest();

    virtual void setup();

    virtual void cleanup();
};

}

#endif
