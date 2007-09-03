#ifndef COABLASTER_LEVELOBJECT_INCLUDED
#define COABLASTER_LEVELOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class LevelObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;


public:
    LevelObject(std::string identifier, std::string meshFile, Ogre::Vector3 pos, Ogre::Quaternion &rot);
    virtual ~LevelObject();

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
