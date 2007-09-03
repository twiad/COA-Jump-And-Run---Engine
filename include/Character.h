
#ifndef COABLASTER_CHARACTER_INCLUDED
#define COABLASTER_CHARACTER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{
    
class Character : public OgreBulletDynamics::RigidBody
{
    Ogre::SceneNode* m_sceneNode;
    Ogre::Entity* m_entity;

    std::string m_identifier;

    double m_moveRotation;
    double m_moveImpule;
    double m_jumpForce;

    double m_maxGrabDistance;

    void move(double factor);

    OgreBulletDynamics::RigidBody* findNextObject();

    // OgreBulletDynamics::ConeTwistConstraint* m_grabConstraint;
    OgreBulletDynamics::PointToPointConstraint* m_grabConstraint;
    OgreBulletDynamics::RigidBody* m_grabbedObject;

public:
    Character(std::string identifier, std::string meshFile);
    virtual ~Character();
    
    virtual void moveLeft(double elapsed);
    virtual void moveRight(double elapsed);
    virtual void jump(double elapsed);

    virtual void grab();
    virtual void ungrab();

    virtual bool isCharacterOnGround();

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

    void applyMovementCorrections();

};
    
}

#endif
