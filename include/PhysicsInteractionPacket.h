
#ifndef COABLASTER_PHYSICSINTERACTIONPACKET_INCLUDED
#define COABLASTER_PHYSICSINTERACTIONPACKET_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

class PhysicsInteractionPacket
{
public:
    enum Type
    {
        ApplyLinearVelocity = 0,
        ApplyImpulse
    };

private:
    Type m_type;
    Ogre::Vector3 m_parameterOne;
    Ogre::Vector3 m_parameterTwo;

public:
    PhysicsInteractionPacket(Type type, 
            Ogre::Vector3 parameterOne = Ogre::Vector3::ZERO, 
            Ogre::Vector3 parameterTwo = Ogre::Vector3::ZERO)
    {
        m_type = type;
        m_parameterOne = parameterOne;
        m_parameterTwo = parameterTwo;
    }
    
    const Type& type() const
    {
        return m_type;
    }
    
    const Ogre::Vector3& parameterOne() const
    {
        return m_parameterOne;
    }

    const Ogre::Vector3& parameterTwo() const
    {
        return m_parameterTwo;
    }
};

}

#endif
