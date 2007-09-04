#ifndef COABLASTER_LEVELOBJECT_INCLUDED
#define COABLASTER_LEVELOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class StaticObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;


public:
    StaticObject(std::string identifier, std::string meshFile, OgreBulletCollisions::CollisionShape* shape, Ogre::Vector3 pos, Ogre::Quaternion rot);
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
    
}

#endif
