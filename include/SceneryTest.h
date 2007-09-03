
#ifndef COABLASTER_SCENERYTEST_INCLUDED
#define COABLASTER_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "Scenery.h"
#include "GraphicsManager.h"
#include "PhysicsManager.h"

namespace CoAJnR
{

#define BOX_COUNT 10

class Character;
class InputController;

class SceneryTest : public Scenery
{
    Ogre::SceneNode* m_planeNode;
    Ogre::SceneNode* m_terrainNode;
    Ogre::SceneNode* m_boxStackNodes[BOX_COUNT];
    Ogre::SceneNode* m_testConstraintNodes[2];
    
    Ogre::Entity* m_plane;
    Ogre::Entity* m_terrain;
    Ogre::Entity* m_boxStack[BOX_COUNT];
    Ogre::Entity* m_testConstraint[2];
    
    Ogre::Light* m_light;
    
    OgreBulletCollisions::CollisionShape* m_planeShape;
    OgreBulletCollisions::CollisionShape* m_boxStackShape[BOX_COUNT];
    OgreBulletCollisions::CollisionShape* m_testConstraintShape[2];
    
    OgreBulletDynamics::RigidBody* m_planeBody;
    OgreBulletDynamics::RigidBody* m_boxStackBodies[BOX_COUNT];
    OgreBulletDynamics::RigidBody* m_testConstraintBodies[2];

    // OgreBulletDynamics::ConeTwistConstraint* m_testConstraintObject;
    // OgreBulletDynamics::SixDofConstraint* m_testConstraintObject;
    OgreBulletDynamics::HingeConstraint* m_testConstraintObject;

    InputController* m_movementInputController;
    
    Character* m_character;
    
    Ogre::Quaternion* m_rot;
    
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

class CubeSpawnCollisionHandler : 
        public OgreBulletCollisions::CollisionHandler
{
    static uint m_spawnId;
    
    uint m_lastSpawn;
    
public:
    CubeSpawnCollisionHandler() : OgreBulletCollisions::CollisionHandler()
    {
        m_lastSpawn = 0;
    }

    void handleCollision(OgreBulletCollisions::CollisionInfo* info)
    {
        if(SDL_GetTicks() - m_lastSpawn < 500)
            return;
        
        GraphicsManager* gm = GraphicsManager::get();
        Ogre::SceneManager* sm = gm->sceneManager();
        
        m_spawnId++;
        
        Ogre::Entity* ent = sm->createEntity(
                "SpawnBox" + Ogre::StringConverter::toString(m_spawnId), 
                "QuestionCube.mesh");
        ent->setNormaliseNormals(true);

        Ogre::SceneNode* node = sm->getRootSceneNode()->
                createChildSceneNode("SpawnBoxNode" + 
                    Ogre::StringConverter::toString(m_spawnId));
    
        OgreBulletDynamics::RigidBody* body = new OgreBulletDynamics::RigidBody(
                "BoxSpawnBody" + Ogre::StringConverter::toString(m_spawnId), 
                PhysicsManager::get()->world());
    
        body->setShape(
            node, 
            new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5,0.5,0.5)), 
            2.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            3,   /* ............................................. mass        */
            Ogre::Vector3(32, 5, 0));    


        node->attachObject(ent);

        m_lastSpawn = SDL_GetTicks();
    }  
};

}

#endif
