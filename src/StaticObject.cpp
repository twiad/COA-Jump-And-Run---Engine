#include "StaticObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoAJnR
{

StaticObject::StaticObject(
		std::string p_identifier, 
		std::string p_meshFile, 
		OgreBulletCollisions::CollisionShape* p_shape,
		Ogre::Vector3 p_pos, 
		Ogre::Quaternion p_rot)
    	: OgreBulletDynamics::RigidBody(p_identifier, PhysicsManager::get()->world()) 
{
    m_identifier = p_identifier;   
    
    m_entity = GraphicsManager::get()->sceneManager()->createEntity(m_identifier, p_meshFile);

    m_entity->setNormaliseNormals(true);
    
    setStaticShape(
            GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(m_identifier), 
            p_shape, 
            2.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            p_pos,
            p_rot
            );
    
    mRootNode->attachObject(m_entity);
}

StaticObject::~StaticObject()
{
    /// @todo TODO: cleanup
	GraphicsManager::get()->sceneManager()->destroyEntity(m_identifier);
	// ? mRootNode->removeAndDestroyAllChildren();
	GraphicsManager::get()->sceneManager()->getRootSceneNode()->removeAndDestroyChild(m_identifier);

	mRootNode = 0;
	std::cout << "   ~StatObj" << std::endl;
}

StaticObjectManager* StaticObjectManager::m_instance = 0;

StaticObjectManager::StaticObjectManager()
{
	m_objectCount = 0;
	
	m_rot = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
	
	m_standardBoxShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5, 0.5, 0.5));
	m_standardTubeShape = new OgreBulletCollisions::CylinderCollisionShape(Ogre::Vector3(1, 1, 1),Ogre::Vector3::UNIT_Z);
}

StaticObjectManager::~StaticObjectManager()
{	
	delete m_standardBoxShape;
	delete m_standardTubeShape;

}

StaticObject*
StaticObjectManager::createBox(
		std::string p_meshFile, 
		Ogre::Vector3 p_pos)
{
	StaticObject* tmpObject = 
		new StaticObject(
			"StaticObject" + Ogre::StringConverter::toString(m_objectCount++), 
	        p_meshFile,
	        m_standardBoxShape,
	        p_pos,
	        m_rot
	        );
	
	m_staticObjects.push_back(tmpObject);
	
	return tmpObject;
}

StaticObject*
StaticObjectManager::createTube(
		std::string p_meshFile, 
		Ogre::Vector3 p_pos)
{	
	StaticObject* tmpObject = 
		new StaticObject(
			"StaticObject" + Ogre::StringConverter::toString(m_objectCount++), 
	        p_meshFile,
	        m_standardTubeShape,
	        p_pos,
	        m_rot
	        );
	
	m_staticObjects.push_back(tmpObject);
	
	return tmpObject;
}

StaticObject*
StaticObjectManager::createConvexObject(
		std::string p_meshFile, 
		Ogre::Vector3 p_pos
		)
{
	Ogre::Entity* tmpEnt = 
		GraphicsManager::get()->sceneManager()->createEntity("SomTmpEntity", p_meshFile);
		OgreBulletCollisions::MeshToShapeConverter convexConv(tmpEnt);
		OgreBulletCollisions::CollisionShape* tmpshape = convexConv.createConvex();

	StaticObject* tmpObject = 
		new StaticObject(
			"StaticObject" + Ogre::StringConverter::toString(m_objectCount++), 
	        p_meshFile,
	        tmpshape,
	        p_pos,
	        m_rot
	        );
	
	
	
	m_staticObjects.push_back(tmpObject);
	
	GraphicsManager::get()->sceneManager()->destroyEntity("SomTmpEntity");
	
	return tmpObject;
}

void 
StaticObjectManager::destroyAllObjects()
{
	    std::list<StaticObject*>::iterator it;
	    for(it = m_staticObjects.begin(); it != m_staticObjects.end(); it++)
	        delete *it;
	    
	    m_staticObjects.clear();
}

}
