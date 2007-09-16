
#ifndef COAJNR_SCENERYTEST_INCLUDED
#define COAJNR_SCENERYTEST_INCLUDED

#include "Dependencies.h"

#include "Character.h"
#include "Scenery.h"
#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "InputHandler.h"
#include "DynamicObject.h"
#include "MainApplication.h"

namespace CoAJnR
{

#define BOX_COUNT 10

// class Character;
class InputController;
class myRaySceneQueryListener;

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
    OgreBulletCollisions::CollisionShape* m_standardBoxShape;
    
    
    OgreBulletDynamics::RigidBody* m_planeBody;
    OgreBulletDynamics::RigidBody* m_boxStackBodies[BOX_COUNT];
    OgreBulletDynamics::RigidBody* m_testConstraintBodies[2];

    // OgreBulletDynamics::ConeTwistConstraint* m_testConstraintObject;
    // OgreBulletDynamics::SixDofConstraint* m_testConstraintObject;
    OgreBulletDynamics::HingeConstraint* m_testConstraintObject;

    InputController* m_movementInputController;
    
    Character* m_character;
    
    Ogre::Quaternion m_rot;
    
public:
    SceneryTest();
    
    virtual ~SceneryTest();

    virtual void setup();

    virtual void cleanup();
    
    PagedGeometry* grass;
    PagedGeometry* trees;
    
    Ogre::RaySceneQuery* raySceneQuery;
    Ogre::Ray updateRay;
    myRaySceneQueryListener* raySceneQueryListener;
    	
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
                // causes crash
                /// @todo TODO: check why it crashes...
                // if(dynamic_cast<Character*>(info->getPartner()))
                //     return;
                
                
                std::list<OgreBulletCollisions::Object*>::iterator it;
                for(it = m_deletedObjects.begin(); 
                    it != m_deletedObjects.end(); it++)
                    if(*it == info->getPartner())
                        break;
                
                if(it == m_deletedObjects.end()) // not already deleted
                {
                    Ogre::ParticleSystem* ps = 
                    GraphicsManager::get()->sceneManager()->
                            createParticleSystem("destroy ps " + 
                            Ogre::StringConverter::toString(m_psCount++),
                            "Examples/Smoke");
                            
                    info->getPartner()->sceneNode()->detachAllObjects();

                    Ogre::SceneNode* psNode = GraphicsManager::get()->
                            sceneManager()->getRootSceneNode()->
                                createChildSceneNode("destroy ps " + 
                                Ogre::StringConverter::toString(m_psCount++));
                    
                    psNode->setPosition(info->getPartner()->
                            sceneNode()->getPosition());
                    psNode->attachObject(ps);

                    delete info->getPartner();
                    m_deletedObjects.push_back(info->getPartner());
                }
            }
    }
    
    void allCollisionsPublished()
    {
        m_deletedObjects.clear();
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
            
        Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
        OgreBulletCollisions::CollisionShape* b = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5, 0.5, 0.5));
        
        new DynamicObject(
        		"SpawnBox" + Ogre::StringConverter::toString(++m_spawnId), 
                "QuestionCube.mesh",
                b,
                7,
                Ogre::Vector3(32, 5, 0), 
                q);

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

        std::cout << " local position: " << 
                        info->getLocalPosition().x() << " " <<
                        info->getLocalPosition().y() << " " <<
                        info->getLocalPosition().z() << " " <<
                        std::endl;
        
        if(info->getLocalPosition().z() < 1)
        	return;
        
        OIS::Keyboard* keyboard = InputHandler::get()->keyboard();
        
        if(!keyboard->isKeyDown(OIS::KC_DOWN))
        	return;
        
        GraphicsManager* gm = GraphicsManager::get();
        Ogre::SceneManager* sm = gm->sceneManager();
        
        m_spawnId++;
        
        Ogre::Entity* ent = sm->createEntity(
                "SpawnBox2" + Ogre::StringConverter::toString(m_spawnId), 
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
            Ogre::Vector3(32, 10, 0));    


        node->attachObject(ent);

        m_lastSpawn = SDL_GetTicks();
    }
};
    
class myRaySceneQueryListener : public Ogre::RaySceneQueryListener
{
public:
	inline bool queryResult(Ogre::SceneQuery::WorldFragment *fragment, Ogre::Real distance)
    {
    	resultDistance = distance;
    	return false;
    }
    inline bool queryResult(Ogre::MovableObject* obj, Ogre::Real distance)
    {
    	resultDistance = distance;
    	return false;
    }

    float resultDistance;
};

extern SceneryTest* s;
inline float getTerrainHeight(const float x, const float z){
	if (x < 0 || z < 0 || x > 1500 || z > 1500) return 0;

	s->updateRay.setOrigin(Ogre::Vector3(x, 0.0f, z));
	s->updateRay.setDirection(Ogre::Vector3::UNIT_Y);
	s->raySceneQuery->setRay(s->updateRay);
	s->raySceneQuery->execute(s->raySceneQueryListener);
	
	return s->raySceneQueryListener->resultDistance;
}

}

#endif
