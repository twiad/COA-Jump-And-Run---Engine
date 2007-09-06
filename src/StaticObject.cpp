#include "StaticObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoAJnR
{

StaticObject::StaticObject(
		std::string p_idenitfier, 
		std::string p_meshFile, 
		OgreBulletCollisions::CollisionShape* p_shape,
		Ogre::Vector3 p_pos, 
		Ogre::Quaternion p_rot)
    	: OgreBulletDynamics::RigidBody(p_idenitfier, PhysicsManager::get()->world()) 
{
    m_identifier = p_idenitfier;   
    
    m_entity = GraphicsManager::get()->sceneManager()->createEntity(p_idenitfier, p_meshFile);

    m_entity->setNormaliseNormals(true);
    
    setStaticShape(
            GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(p_idenitfier), 
            p_shape, 
            0.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            p_pos,
            p_rot
            //Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
            );
    
    mRootNode->attachObject(m_entity);
    
    //setPosition(p_pos);
    //getBulletRigidBody()->getWorldTransform().setOrigin(btVector3(5,8,0));
}

StaticObject::~StaticObject()
{
    /// @todo TODO: cleanup
}

}
