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
//
	m_identifier = p_identifier;   
			    
	m_entity = GraphicsManager::get()->sceneManager()->createEntity(m_identifier, p_meshFile);

	m_entity->setNormaliseNormals(true);
			    
	setShape(
			GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(m_identifier), 
			p_shape, 
			2.0, /* ............................................. restitution */
			2.0, /* ............................................. friction    */
			p_mass,
			p_pos,
			p_rot
			);
			    
	mRootNode->attachObject(m_entity);
}

DynamicObject::~DynamicObject()
{
    /// @todo TODO: cleanup
	std::cout << "   ~DynObj: " << m_identifier << std::endl;
    GraphicsManager::get()->sceneManager()->destroyEntity(m_identifier);
    //mRootNode->removeAndDestroyAllChildren();
    GraphicsManager::get()->sceneManager()->getRootSceneNode()->removeAndDestroyChild(m_identifier);

    mRootNode = 0;
}

DynamicObjectManager* DynamicObjectManager::m_instance = 0;

DynamicObjectManager::DynamicObjectManager()
{
	m_objectCount = 0;
	
	m_rot = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
	
	m_standardBoxShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.5, 0.5, 0.5));
	m_standardTubeShape = new OgreBulletCollisions::CylinderCollisionShape(Ogre::Vector3(1, 1, 1),Ogre::Vector3::UNIT_Z);
}

DynamicObjectManager::~DynamicObjectManager()
{
//	std::list<DynamicObject*>::iterator it;
//	for(it = m_dynamicObjects.begin(); it != m_dynamicObjects.end(); it++)
//		delete *it;
	
	m_dynamicObjects.clear();
	
	delete m_standardBoxShape;
	delete m_standardTubeShape;
	
	std::cout << "  ~Dom" << std::endl;
}

DynamicObject*
DynamicObjectManager::createBox(
		std::string p_meshFile, 
		float p_mass,
		Ogre::Vector3 p_pos)
{
	DynamicObject* tmpObject = 
		new DynamicObject(
			"DynamicObject" + Ogre::StringConverter::toString(m_objectCount++), 
	        p_meshFile,
	        m_standardBoxShape,
	        p_mass,
	        p_pos,
	        m_rot
	        );
	//
	std::cout << "BoxNr: " << Ogre::StringConverter::toString(m_objectCount) << std::endl;
	
	m_dynamicObjects.push_back(tmpObject);
	
	return tmpObject;
}

DynamicObject*
DynamicObjectManager::createTube(
		std::string p_meshFile, 
		float p_mass,
		Ogre::Vector3 p_pos)
{	
	DynamicObject* tmpObject = 
		new DynamicObject(
			"DynamicObject" + Ogre::StringConverter::toString(m_objectCount++), 
	        p_meshFile,
	        m_standardTubeShape,
	        p_mass,
	        p_pos,
	        m_rot
	        );
	
	std::cout << "TubeNr: " << Ogre::StringConverter::toString(m_objectCount) << std::endl;
	
	m_dynamicObjects.push_back(tmpObject);
	
	return tmpObject;
}

DynamicObject*
DynamicObjectManager::createConvexObject(
		std::string p_meshFile, 
		float p_mass,
		Ogre::Vector3 p_pos
		)
{
	Ogre::Entity* tmpEnt = 
		GraphicsManager::get()->sceneManager()->createEntity("DomTmpEntity", p_meshFile);
		OgreBulletCollisions::MeshToShapeConverter convexConv(tmpEnt);
		OgreBulletCollisions::CollisionShape* tmpshape = convexConv.createConvex();

		DynamicObject* tmpObject = 
		new DynamicObject(
			"DynamicObject" + Ogre::StringConverter::toString(m_objectCount++), 
	        p_meshFile,
	        tmpshape,
	        p_mass,
	        p_pos,
	        m_rot
	        );
	
	std::cout << "ConvexNr: " << Ogre::StringConverter::toString(m_objectCount) << std::endl;
	
	m_dynamicObjects.push_back(tmpObject);
	
	GraphicsManager::get()->sceneManager()->destroyEntity("DomTmpEntity");
	
	
	return tmpObject;
}

void 
DynamicObjectManager::deleteDynamicObject(DynamicObject* p_object)
{
	std::list<DynamicObject*>::iterator it;
	for(it = m_dynamicObjects.begin(); it != m_dynamicObjects.end(); it++)
	{
		if (*it == p_object)
		{
			GraphicsManager::get()->sceneManager()->destroyEntity(p_object->getName());
			//GraphicsManager::get()->sceneManager()->getRootSceneNode()->removeAndDestroyChild(p_object->getName());
			m_dynamicObjects.erase(it);
			std::cout << "geklappt" << std::endl;
			break;
		}
	}
	return;
}

}
