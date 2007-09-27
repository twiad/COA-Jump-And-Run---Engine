#include "PhysicsManager.h"

#include "GraphicsManager.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

namespace CoAJnR
{

PhysicsManager* PhysicsManager::m_instance = 0;

PhysicsManager::PhysicsManager()
{
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
     //m_world->setShowDebugShapes(true);
    // m_world->setShowDebugContactPoints(true); // does not seem to work
}

PhysicsManager::~PhysicsManager()
{
    if(m_world)
        delete m_world;
        
    // GraphicsManager::get()->sceneManager()->
    //         destroySceneNode("ogrebullet-debug-node");
    std::cout << "~pm" << std::endl;
}

void
PhysicsManager::update(double p_elapsed)
{
    m_world->stepSimulation(p_elapsed);
}

void
PhysicsManager::synchronize()
{
    m_world->synchronizeToOgre();
}

bool 
PhysicsManager::frameStarted(const Ogre::FrameEvent& p_event)
{
    update(p_event.timeSinceLastFrame);
    synchronize();
    return true;
}

}
