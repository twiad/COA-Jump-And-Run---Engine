#ifndef COABLASTER_DYNAMICOBJECT_INCLUDED
#define COABLASTER_DYNAMICOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class DynamicObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;


public:
    DynamicObject(std::string identifier, std::string meshFile, Ogre::Vector3 pos, Ogre::Quaternion &rot);
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
