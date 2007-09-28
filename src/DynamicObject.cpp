#include "DynamicObject.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"

#include "MainApplication.h"

namespace CoAJnR
{

DynamicObject::DynamicObject(
		Ogre::Entity* p_dynEntity, 
		std::string p_meshFile, 
		OgreBulletCollisions::CollisionShape* p_shape,
		int p_mass,
		Ogre::Vector3 p_pos, 
		Ogre::Quaternion &p_rot)
    	: OgreBulletDynamics::RigidBody(p_dynEntity->getName(), 
    	        PhysicsManager::get()->world()) 
{   			    
	p_dynEntity->setNormaliseNormals(true);
			    
	setShape(
			GraphicsManager::get()->sceneManager()->getRootSceneNode()->
			        createChildSceneNode(p_dynEntity->getName()), 
			p_shape, 
			2.0, /* ............................................. restitution */
			2.0, /* ............................................. friction    */
			p_mass,
			p_pos,
			p_rot
			);
			    
	mRootNode->attachObject(p_dynEntity);
}

DynamicObject::~DynamicObject()
{
    /// @todo TODO: cleanup
    GraphicsManager::get()->sceneManager()->destroyEntity(getName());
    //mRootNode->removeAndDestroyAllChildren();
    GraphicsManager::get()->sceneManager()->getRootSceneNode()->
            removeAndDestroyChild(getName());

    mRootNode = 0;
}

DynamicObjectManager* DynamicObjectManager::m_instance = 0;

DynamicObjectManager::DynamicObjectManager()
{
	m_objectCount = 0;
	
	m_rot = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);
}

DynamicObjectManager::~DynamicObjectManager()
{	
    
}

DynamicObject*
DynamicObjectManager::createBox(
		std::string p_meshFile, 
		float p_mass,
		Ogre::Vector3 p_pos)
{
    Ogre::Entity* boxEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "DynamicObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter boxConv(boxEntity);
    OgreBulletCollisions::CollisionShape* boxShape = boxConv.createBox();
    
	DynamicObject* boxObject = 
		new DynamicObject(
			boxEntity, 
	        p_meshFile,
	        boxShape,
	        p_mass,
	        p_pos,
	        m_rot
	        );
	
	m_dynamicObjects.push_back(boxObject);
	
	return boxObject;
}

DynamicObject*
DynamicObjectManager::createTube(
		std::string p_meshFile, 
		float p_mass,
		Ogre::Vector3 p_axis,
		Ogre::Vector3 p_pos)
{	
    Ogre::Entity* tubeEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "DynamicObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter tubeConv(tubeEntity);
    OgreBulletCollisions::CollisionShape* tubeShape = tubeConv.createCylinder(p_axis);
        
	DynamicObject* tubeObject = 
		new DynamicObject(
			tubeEntity, 
	        p_meshFile,
	        tubeShape,
	        p_mass,
	        p_pos,
	        m_rot
	        );
	
	m_dynamicObjects.push_back(tubeObject);
	
	return tubeObject;
}

DynamicObject*
DynamicObjectManager::createConvexObject(
		std::string p_meshFile, 
		float p_mass,
		Ogre::Vector3 p_pos
		)
{
	Ogre::Entity* convexEntity = 
		GraphicsManager::get()->sceneManager()->createEntity(
		        "DynamicObject" + Ogre::StringConverter::toString(m_objectCount++),
		        p_meshFile);
		OgreBulletCollisions::MeshToShapeConverter convexConv(convexEntity);
		OgreBulletCollisions::CollisionShape* convexShape = convexConv.createConvex();

		DynamicObject* convexObject = 
		new DynamicObject(
			convexEntity, 
	        p_meshFile,
	        convexShape,
	        p_mass,
	        p_pos,
	        m_rot
	        );
		
	m_dynamicObjects.push_back(convexObject);	
	
	return convexObject;
}

void 
DynamicObjectManager::destroyDynamicObject(DynamicObject* p_object)
{
	std::list<DynamicObject*>::iterator it;
	for(it = m_dynamicObjects.begin(); it != m_dynamicObjects.end(); it++)
	{
		if (*it == p_object)
		{
			delete *it;
			m_dynamicObjects.erase(it);
			break;
		}
	}
	return;
}

void 
DynamicObjectManager::destroyAllObjects()
{
    std::list<DynamicObject*>::iterator it;
    for(it = m_dynamicObjects.begin(); it != m_dynamicObjects.end(); it++)
        delete *it;
    
    m_dynamicObjects.clear();
}

}
