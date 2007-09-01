
#ifndef COABLASTER_SCENERYTEST_INCLUDED
#define COABLASTER_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "Scenery.h"

namespace CoABlaster
{

#define BOX_COUNT 10

class Character;
class InputController;

class SceneryTest : public Scenery
{
    Ogre::SceneNode* m_planeNode;
    Ogre::SceneNode* m_boxNode;
    Ogre::SceneNode* m_boxStackNodes[BOX_COUNT];
    
    Ogre::Entity* m_plane;
    Ogre::Entity* m_box;
    Ogre::Entity* m_boxStack[BOX_COUNT];
    
    Ogre::Light* m_light;
    
    OgreBulletCollisions::CollisionShape* m_planeShape;
    OgreBulletCollisions::CollisionShape* m_boxShape;
    OgreBulletCollisions::CollisionShape* m_boxStackShape[BOX_COUNT];
    
    OgreBulletDynamics::RigidBody* m_planeBody;
    OgreBulletDynamics::RigidBody* m_boxBody;
    OgreBulletDynamics::RigidBody* m_boxStackBodies[BOX_COUNT];

    InputController* m_movementInputController;
    
    Character* m_character;
    
public:
    SceneryTest();
    
    virtual ~SceneryTest();

    virtual void setup();

    virtual void cleanup();
};

class DebugOutputCollisionHandler : 
        public OgreBulletCollisions::CollisionHandler
{
public:
    void handleCollision(OgreBulletCollisions::CollisionInfo* info)
    {
        std::cout << "*** collision happened ***" << std::endl;

        std::cout << " object: " << info->getObject() << std::endl;
        std::cout << " partner: " << info->getPartner() << std::endl;
        std::cout << " local position: " << 
                info->getLocalPosition().x() << " " <<
                info->getLocalPosition().y() << " " <<
                info->getLocalPosition().z() << " " <<
                std::endl;
        std::cout << " world position: " << 
                info->getWorldPosition().x() << " " <<
                info->getWorldPosition().y() << " " <<
                info->getWorldPosition().z() << " " <<
                std::endl;
    }  
};

}

#endif
