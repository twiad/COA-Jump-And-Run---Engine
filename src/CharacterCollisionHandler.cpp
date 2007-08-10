#include "CharacterCollisionHandler.h"

#include "MainApplication.h"

namespace CoABlaster
{
    
void
CharacterCollisionHandler::handleCollision(
        OgreBulletCollisions::CollisionInfo& p_info)
{   
    // std::cout << "o: " << getCollisionObject()->getWorldPosition().y 
    //     << " c: " << p_info.getWorldPosition().y() << " d: " 
    //     << p_info.getWorldPosition().y() - 
    //         getCollisionObject()->getWorldPosition().y<< std::endl;
    
    if(getCollisionObject()->getWorldPosition().y > 
            p_info.getWorldPosition().y())
    {
/// @todo TODO: THREADS?????????

        m_characterOnGround = true;

        // std::cout << "GROUND COLLISION" << std::endl;
    }
}
    
}