#ifndef COAJNR_STATICOBJECT_INCLUDED
#define COAJNR_STATICOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class StaticObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;


public:
    StaticObject(Ogre::Entity* statEntity, std::string meshFile, OgreBulletCollisions::CollisionShape* shape, Ogre::Vector3 pos, Ogre::Quaternion rot);
    virtual ~StaticObject();
    
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

class StaticObjectManager
{
	static StaticObjectManager* m_instance;
	
	uint m_objectCount;
	std::list<StaticObject*> m_staticObjects;
	
	Ogre::Quaternion m_rot;
	
public:
	StaticObjectManager();
	virtual ~StaticObjectManager();
	
	static StaticObjectManager* get()
	    {
	        if(!m_instance)
	            m_instance = new StaticObjectManager;

	        assert(m_instance);
	        return m_instance;
	    }
	
	StaticObject* createBox(std::string meshFile, Ogre::Vector3 pos);
	StaticObject* createTube(std::string meshFile, Ogre::Vector3 axis, Ogre::Vector3 pos);
	StaticObject* createConvexObject(std::string meshFile, Ogre::Vector3 pos);
	StaticObject* createTrimeshObject(std::string meshFile, Ogre::Vector3 pos);
        
	void destroyAllObjects();
};
    
}

#endif
