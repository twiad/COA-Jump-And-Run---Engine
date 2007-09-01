#include "LevelObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoABlaster
{

LevelObject::LevelObject(std::string p_idenitfier, std::string p_meshFile, Ogre::Vector3 p_pos)
    : OgreBulletDynamics::RigidBody(p_idenitfier, PhysicsManager::get()->world()) 
{
    m_identifier = p_idenitfier;   
    
    m_entity = GraphicsManager::get()->sceneManager()->createEntity(p_idenitfier, p_meshFile);

    m_entity->setNormaliseNormals(true);

    setStaticShape(
            GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(p_idenitfier), 
            new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1, 1, 1)), 
            0.0, /* ............................................. restitution */
            0.0, /* ............................................. friction    */
            p_pos,
            Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
            );

    mRootNode->attachObject(m_entity);
    
}

LevelObject::~LevelObject()
{
    /// @todo TODO: cleanup
}

}
