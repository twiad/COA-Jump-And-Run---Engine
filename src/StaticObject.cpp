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
    
    m_staticGeometry = new Ogre::StaticGeometry(
            GraphicsManager::get()->sceneManager(), 
            p_statEntity->getName());
    m_staticGeometry->addEntity(p_statEntity, p_pos, p_rot);

    m_staticGeometry->build();
    
    setStaticShape(
            // GraphicsManager::get()->sceneManager()->getRootSceneNode()->createChildSceneNode(p_statEntity->getName()), 
            p_shape, 
            2.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            p_pos,
            p_rot
            );
}

StaticObject::~StaticObject()
{
	GraphicsManager::get()->sceneManager()->destroyEntity(m_identifier);
    delete m_staticGeometry;
    
    // GraphicsManager::get()->sceneManager()->getRootSceneNode()->removeAndDestroyChild(m_identifier);

    // mRootNode = 0;
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
    OgreBulletCollisions::CollisionShape* boxShape = boxConv.createBox();
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
		Ogre::Vector3 p_axis,
		Ogre::Vector3 p_pos)
{	
    Ogre::Entity* tubeEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "StaticObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter tubeConv(tubeEntity);
    OgreBulletCollisions::CollisionShape* tubeShape = tubeConv.createCylinder(p_axis);
    
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
    OgreBulletCollisions::CollisionShape* convexShape = convexConv.createConvex();

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

StaticObject*
StaticObjectManager::createTrimeshObject(
        std::string p_meshFile, 
        Ogre::Vector3 p_pos
        )
{
    Ogre::Entity* triEntity = 
        GraphicsManager::get()->sceneManager()->createEntity(
                "StaticObject" + Ogre::StringConverter::toString(m_objectCount++),
                p_meshFile);
    OgreBulletCollisions::MeshToShapeConverter triConv(triEntity);
    OgreBulletCollisions::CollisionShape* triShape = triConv.createTrimesh();

    StaticObject* triObject = 
        new StaticObject(
            triEntity, 
            p_meshFile,
            triShape,
            p_pos,
            m_rot
            );
    
    m_staticObjects.push_back(triObject);
    
    return triObject;
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
