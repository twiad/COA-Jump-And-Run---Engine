#ifndef COAJNR_DYNAMICOBJECT_INCLUDED
#define COAJNR_DYNAMICOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class DynamicObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;

    void createObject(std::string& indentifier, std::string& meshFile,
            OgreBulletCollisions::CollisionShape* shape,
            double mass, double restitution, double friction,
            Ogre::Vector3& position, Ogre::Quaternion& rotation);

public:
    DynamicObject(
    		std::string identifier, 
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
    
}

#endif
