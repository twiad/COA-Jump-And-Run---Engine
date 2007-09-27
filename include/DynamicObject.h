#ifndef COAJNR_DYNAMICOBJECT_INCLUDED
#define COAJNR_DYNAMICOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class DynamicObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

public:
    DynamicObject(
    		Ogre::Entity* dynEntity, 
    		std::string meshFile, 
    		OgreBulletCollisions::CollisionShape* shape,
    		int mass,
    		Ogre::Vector3 pos, 
    		Ogre::Quaternion &rot);
    virtual ~DynamicObject();

    Ogre::SceneNode* sceneNode()
    {
        assert(mRootNode);
        return mRootNode;
    }

    Ogre::Entity* entity()
    {
        assert(m_entity);
        return m_entity;
    }

};
    
class DynamicObjectManager
{
	static DynamicObjectManager* m_instance;
	
	uint m_objectCount;
	std::list<DynamicObject*> m_dynamicObjects;
	
	Ogre::Quaternion m_rot;
	
public:
	DynamicObjectManager();
	virtual ~DynamicObjectManager();
	
	static DynamicObjectManager* get()
	    {
	        if(!m_instance)
	            m_instance = new DynamicObjectManager;

	        assert(m_instance);
	        return m_instance;
	    }
	
	DynamicObject* createBox(std::string meshFile, float mass, Ogre::Vector3 pos);
	DynamicObject* createTube(std::string meshFile, float mass, Ogre::Vector3 pos);
	DynamicObject* createConvexObject(std::string meshFile, float mass, Ogre::Vector3 pos);
	void destroyAllObjects();	
	void destroyDynamicObject(DynamicObject* object);
};
    
}

#endif
