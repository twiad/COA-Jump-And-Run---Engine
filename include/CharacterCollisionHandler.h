
#ifndef COABLASTER_CHARACTERCOLLISIONHANDLER_INCLUDED
#define COABLASTER_CHARACTERCOLLISIONHANDLER_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

/**
 * interface for sceneries, setting up environment.
 */
class CharacterCollisionHandler : public OgreBulletCollisions::CollisionHandler
{
    bool m_characterOnGround;

public:
    CharacterCollisionHandler() { }
    
    virtual ~CharacterCollisionHandler() { }
    
    virtual void handleCollision(OgreBulletCollisions::CollisionInfo& info);

    bool isCharacterOnGround() { return m_characterOnGround; }
    void setCharacterOnGround(bool flag) { m_characterOnGround = flag; }
};

}

#endif
