
#ifndef COAJNR_SCENERYTEST_INCLUDED
#define COAJNR_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "Character.h"
#include "Scenery.h"
#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "InputHandler.h"
#include "DynamicObject.h"
#include "GrassFrameListener.h"
#include "MyRaySceneQueryListener.h"

namespace CoAJnR
{

#define BOX_COUNT 10

// class Character;
class InputController;

class SceneryTest : public Scenery
{
    Ogre::SceneNode* m_baseLevelNode;
    Ogre::Entity* m_baseLevel;
    OgreBulletCollisions::CollisionShape* m_baseLevelShape;
    OgreBulletDynamics::RigidBody* m_baseLevelBody;

    Ogre::Entity* m_terrain;
    OgreBulletCollisions::CollisionShape* m_floorShape;
    OgreBulletDynamics::RigidBody* m_floorBody;

    Ogre::Light* m_light;

    OgreBulletCollisions::CollisionShape* m_standardBoxShape;
    Ogre::Quaternion m_rot;


    InputController* m_movementInputController;

    Character* m_character;

public:

    SceneryTest();

    virtual ~SceneryTest();

    virtual void setup();

    virtual void cleanup();

    static float getTerrainHeight(const float x, const float z);

    void initPagedGeometry();

    void getStaticGeometry(
            Ogre::StaticGeometry* mesh,
            size_t &vertex_count,
            Ogre::Vector3* &vertices,
            size_t &index_count,
            unsigned long* &indices,
            const Ogre::Vector3 &position,
            const Ogre::Quaternion &orient,
            const Ogre::Vector3 &scale);

    void getMeshInformation(
            const Ogre::MeshPtr mesh,
            size_t &vertex_count,
            Ogre::Vector3* &vertices,
            size_t &index_count,
            unsigned long* &indices,
            const Ogre::Vector3 &position,
            const Ogre::Quaternion &orient,
            const Ogre::Vector3 &scale);

    bool raycastFromPoint(
            const Ogre::Vector3 &point,
            const Ogre::Vector3 &normal,
            Ogre::Vector3 &result);

    Ogre::RaySceneQuery* m_raySceneQuery;

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

class DestroyTouchingObjectsCollisionHandler :
public OgreBulletCollisions::CollisionHandler
{
    static uint m_psCount;
    std::list<OgreBulletCollisions::Object*> m_deletedObjects;

public:
    DestroyTouchingObjectsCollisionHandler()
    {
    }

    void handleCollision(OgreBulletCollisions::CollisionInfo* info)
    {
        if(info)
            if(info->getPartner())
            {
                std::list<OgreBulletCollisions::Object*>::iterator it;
                
                for(it = m_deletedObjects.begin(); 
                it != m_deletedObjects.end(); it++)
                    if(*it == info->getPartner())
                    {
                        //TODO: list keeps growing and growing
                        break;
                    }

                if(it == m_deletedObjects.end()) // not already deleted
                {
                    if(dynamic_cast<DynamicObject*>(info->getPartner()))
                    {
                        DynamicObjectManager::get()->destroyDynamicObject(dynamic_cast<DynamicObject*>(info->getPartner()));
                        m_deletedObjects.push_back(info->getPartner());
                        return;
                    }
                    if(dynamic_cast<Character*>(info->getPartner()))
                    {
                        //TODO: let character die
                        return;
                    }
                    info->getPartner()->sceneNode()->detachAllObjects();
                    delete info->getPartner();
                    m_deletedObjects.push_back(info->getPartner());

//                    Ogre::ParticleSystem* ps = 
//                        GraphicsManager::get()->sceneManager()->
//                        createParticleSystem("destroy ps " + 
//                                Ogre::StringConverter::toString(m_psCount++),
//                        "Examples/Smoke");                            
//                    Ogre::SceneNode* psNode = GraphicsManager::get()->
//                    sceneManager()->getRootSceneNode()->
//                    createChildSceneNode("destroy ps " + 
//                            Ogre::StringConverter::toString(m_psCount++));
//
//                    psNode->setPosition(info->getPartner()->
//                            sceneNode()->getPosition());
//                    psNode->attachObject(ps);

                }
            }
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
    }

    void handleCollision(OgreBulletCollisions::CollisionInfo* info)
    {
        if(SDL_GetTicks() - m_lastSpawn < 500)
            return;

        DynamicObjectManager::get()->createBox("QuestionCube.mesh",7,Ogre::Vector3(32, 5, 0));

        m_lastSpawn = SDL_GetTicks();
    }  
};

class TubeCollisionHandler : 
public OgreBulletCollisions::CollisionHandler
{
    static uint m_spawnId;

    uint m_lastSpawn;

public:
    TubeCollisionHandler() : OgreBulletCollisions::CollisionHandler()
    {
        m_lastSpawn = 0;
    }

    void handleCollision(OgreBulletCollisions::CollisionInfo* info)
    {
        if(SDL_GetTicks() - m_lastSpawn < 100)
            return;

        if(info->getLocalPosition().z() < 1)
            return;

        OIS::Keyboard* keyboard = InputHandler::get()->keyboard();

        if(!keyboard->isKeyDown(OIS::KC_DOWN))
            return;

        m_spawnId++;

        DynamicObjectManager::get()->createBox("QuestionCube.mesh",3,Ogre::Vector3(32, 10, 0));

        m_lastSpawn = SDL_GetTicks();
    }

};



}

#endif
