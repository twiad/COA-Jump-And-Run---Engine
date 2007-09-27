#include "StaticObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoAJnR
{

StaticObject::StaticObject(
		Ogre::Entity* p_statEntity, 
		std::string p_meshFile, 
		OgreBulletCollisions::CollisionShape* p_shape,
		Ogre::Vector3 p_pos, 
		Ogre::Quaternion p_rot)
    	: OgreBulletDynamics::RigidBody(p_statEntity->getName(), PhysicsManager::get()->world()) 
{
    p_statEntity->setNormaliseNormals(true);
    
    setStaticShape(
            GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(p_statEntity->getName()), 
            p_shape, 
            2.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            p_pos,
            p_rot
            );
    
    mRootNode->attachObject(p_statEntity);
}

StaticObject::~StaticObject()
{
	GraphicsManager::get()->sceneManager()->destroyEntity(m_identifier);
	GraphicsManager::get()->sceneManager()->getRootSceneNode()->removeAndDestroyChild(m_identifier);

	mRootNode = 0;
}

StaticObjectManager* StaticObjectManager::m_instance = 0;

StaticObjectManager::StaticObjectManager()
{
	m_objectCount = 0;
	
	m_rot = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
}

StaticObjectManager::~StaticObjectManager()
{	
    
}

StaticObject*
StaticObjectManager::createBox(
		std::string p_meshFile, 
		Ogre::Vector3 p_pos)
{
    Ogre::Entity* boxEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "StaticObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter boxConv(boxEntity);
    OgreBulletCollisions::CollisionShape* boxShape = boxConv.createCylinder();
	StaticObject* boxObject = 
		new StaticObject(
			boxEntity, 
	        p_meshFile,
	        boxShape,
	        p_pos,
	        m_rot
	        );
	m_staticObjects.push_back(boxObject);
	return boxObject;
}

StaticObject*
StaticObjectManager::createTube(
		std::string p_meshFile, 
		Ogre::Vector3 p_pos)
{	
    Ogre::Entity* tubeEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "StaticObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter tubeConv(tubeEntity);
    OgreBulletCollisions::CollisionShape* tubeShape = tubeConv.createCylinder();
    
	StaticObject* tubeObject = 
		new StaticObject(
			tubeEntity, 
	        p_meshFile,
	        tubeShape,
	        p_pos,
	        m_rot
	        );
	
	m_staticObjects.push_back(tubeObject);
	
	return tubeObject;
}

StaticObject*
StaticObjectManager::createConvexObject(
		std::string p_meshFile, 
		Ogre::Vector3 p_pos
		)
{
	Ogre::Entity* convexEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "StaticObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter convexConv(convexEntity);
    OgreBulletCollisions::CollisionShape* convexShape = convexConv.createCylinder();

	StaticObject* convexObject = 
		new StaticObject(
			convexEntity, 
	        p_meshFile,
	        convexShape,
	        p_pos,
	        m_rot
	        );
	
	
	
	m_staticObjects.push_back(convexObject);
	
	return convexObject;
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
