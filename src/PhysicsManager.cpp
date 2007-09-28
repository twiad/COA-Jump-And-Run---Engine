#include "PhysicsManager.h"

#include "Character.h"
#include "GraphicsManager.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

namespace CoAJnR
{

PhysicsManager* PhysicsManager::m_instance = 0;

PhysicsManager::PhysicsManager()
{
    m_doMovementCorrections = false;
    
    // Start Bullet
    m_world = new DynamicsWorld(
            GraphicsManager::get()->sceneManager(),
            Ogre::AxisAlignedBox(-1000, -1000, -1000, 1000, 1000, 1000), 
            Ogre::Vector3(0, -9.81, 0));

    // DebugDrawer* debugDrawer = new DebugDrawer();
    //     m_world->setDebugDrawer(debugDrawer);
    //     m_debugNode = GraphicsManager::get()->sceneManager()->getRootSceneNode()->
    //             createChildSceneNode("ogrebullet-debug-node");
    //                  
    // m_debugNode->attachObject(debugDrawer);
    
    // enabling debug shapes crashes ogre when creating ogre stuff in ia thread
     // m_world->setShowDebugShapes(true);
    // m_world->setShowDebugContactPoints(true); // does not seem to work
}

PhysicsManager::~PhysicsManager()
{
    if(m_world)
        delete m_world;
        
    // GraphicsManager::get()->sceneManager()->
    //         destroySceneNode("ogrebullet-debug-node");
}

void
PhysicsManager::update(double p_elapsed)
{
    if(m_doMovementCorrections)
    {
        applyMovementCorrections();
        m_doMovementCorrections = false;
    }
    
    m_world->stepSimulation(p_elapsed);
}

void
PhysicsManager::synchronize()
{
    m_world->synchronizeToOgre();
}

/// @todo TODO: remove this
bool 
PhysicsManager::frameStarted(const Ogre::FrameEvent& p_event)
{
    update(p_event.timeSinceLastFrame);
    synchronize();
    return true;
}

void
PhysicsManager::applyMovementCorrections()
{
    std::vector<Character*>::iterator it;
    for(it = m_characters.begin(); it != m_characters.end(); it++)
        (*it)->applyMovementCorrections();
}

void
PhysicsManager::addCharacter(Character* p_character)
{
    assert(p_character && "character must not be null");
    m_characters.push_back(p_character);
}

void
PhysicsManager::removeCharacter(Character* p_character)
{
    assert(p_character && "character must not be null");
    std::vector<Character*>::iterator it;
    for(it = m_characters.begin(); it != m_characters.end(); it++)
        if(*it == p_character)
            m_characters.erase(it);
}

void
PhysicsManager::scheduleMovementCorrections()
{
    m_doMovementCorrections = true;
}
}
