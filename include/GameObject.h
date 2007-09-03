#ifndef COABLASTER_GAMEOBJECT_INCLUDED
#define COABLASTER_GAMEOBJECT_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{
    
class GameObject : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;


public:
    GameObject(std::string identifier, std::string meshFile, Ogre::Vector3 pos, Ogre::Quaternion &rot);
    virtual ~GameObject();

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
