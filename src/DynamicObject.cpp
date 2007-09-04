#include "DynamicObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoAJnR
{

DynamicObject::DynamicObject(
		std::string p_idenitfier, 
		std::string p_meshFile, 
		OgreBulletCollisions::CollisionShape* p_shape,
		int p_mass,
		Ogre::Vector3 p_pos, 
		Ogre::Quaternion &p_rot)
    	: OgreBulletDynamics::RigidBody(p_idenitfier, PhysicsManager::get()->world()) 
{
    m_identifier = p_idenitfier;   
    
    m_entity = GraphicsManager::get()->sceneManager()->createEntity(p_idenitfier, p_meshFile);

    m_entity->setNormaliseNormals(true);
    
    setShape(
            GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(p_idenitfier), 
            p_shape, 
            2.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            p_mass,
            p_pos,
            p_rot
            //Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
            );
    
    mRootNode->attachObject(m_entity);
    
    //setPosition(p_pos);
    //getBulletRigidBody()->getWorldTransform().setOrigin(btVector3(5,8,0));
}

DynamicObject::~DynamicObject()
{
    /// @todo TODO: cleanup
}

}
