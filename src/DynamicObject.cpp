#include "DynamicObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoAJnR
{

DynamicObject::DynamicObject(
		std::string p_identifier, 
		std::string p_meshFile, 
		OgreBulletCollisions::CollisionShape* p_shape,
		int p_mass,
		Ogre::Vector3 p_pos, 
		Ogre::Quaternion &p_rot)
    	: OgreBulletDynamics::RigidBody(p_identifier, 
    	        PhysicsManager::get()->world()) 
{
    createObject(p_identifier, p_meshFile, p_shape, p_mass, 
            1.0, 1.0, p_pos, p_rot);
}

DynamicObject::~DynamicObject()
{
    /// @todo TODO: cleanup

    GraphicsManager::get()->sceneManager()->destroyEntity(m_identifier);
    mRootNode->removeAndDestroyAllChildren();
    GraphicsManager::get()->sceneManager()->getRootSceneNode()->
            removeAndDestroyChild(m_identifier);

    mRootNode = 0;
}


void
DynamicObject::createObject(std::string& p_identifier, std::string& p_meshFile,
        OgreBulletCollisions::CollisionShape* p_shape,
        double p_mass, double p_restitution, double p_friction,
        Ogre::Vector3& p_position, Ogre::Quaternion& p_rotation)
{
    m_identifier = p_identifier;   
    
    m_entity = GraphicsManager::get()->sceneManager()->
            createEntity(p_identifier, p_meshFile);

    m_entity->setNormaliseNormals(true);
    
    setShape(GraphicsManager::get()->sceneManager()->getRootSceneNode()->
                    createChildSceneNode(p_identifier), 
            p_shape,p_restitution, p_friction, p_mass, p_position, p_rotation);
            
    mRootNode->attachObject(m_entity);
    
    //Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X)
    //setPosition(p_pos);
    //getBulletRigidBody()->getWorldTransform().setOrigin(btVector3(5,8,0));
}


}
