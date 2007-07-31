#include "PhysicsManager.h"

#include "GraphicsManager.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

namespace CoABlaster
{

PhysicsManager* PhysicsManager::m_instance = 0;

PhysicsManager::PhysicsManager()
{
    // Start Bullet
    m_world = new DynamicsWorld(
            GraphicsManager::get()->sceneManager(),
            Ogre::AxisAlignedBox(-1000, -1000, -1000, 1000, 1000, 1000), 
            Ogre::Vector3(0, -9.81, 0));

    // // add Debug info displa tool
    // DebugDrawer *debugDrawer = new DebugDrawer();
    // 
    // m_world->setDebugDrawer(debugDrawer);
    // 
    // SceneNode *node = mSceneMgr->getRootSceneNode()->
    //      createChildSceneNode("debugDrawer", Vector3::ZERO);
    // node->attachObject (static_cast <SimpleRenderable *> (debugDrawer));
    // 
}

PhysicsManager::~PhysicsManager()
{
    if(m_world)
        delete m_world;
}


bool 
PhysicsManager::frameStarted(const Ogre::FrameEvent& p_event)
{
    m_world->stepSimulation(p_event.timeSinceLastFrame);
    
    return true;
}

}
