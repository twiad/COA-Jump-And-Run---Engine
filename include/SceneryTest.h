
#ifndef COABLASTER_SCENERYTEST_INCLUDED
#define COABLASTER_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "Scenery.h"

namespace CoABlaster
{

class InputController;

class SceneryTest : public Scenery
{
    Ogre::SceneNode* m_cubeNode;
    Ogre::SceneNode* m_planeNode;
    
    Ogre::Entity* m_cube;
    Ogre::Entity* m_plane;
    
    Ogre::Light* m_light;
    
    OgreBulletCollisions::BoxCollisionShape* m_cubeShape;
    OgreBulletCollisions::StaticPlaneCollisionShape* m_planeShape;
    
    OgreBulletDynamics::RigidBody* m_cubeBody;
    OgreBulletDynamics::RigidBody* m_planeBody;

    InputController* m_movementInputController;
    
public:
    SceneryTest();
    
    virtual ~SceneryTest();

    virtual void setup();

    virtual void cleanup();
};

}

#endif
