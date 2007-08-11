
#include "CharacterMovementController.h"

#include "CharacterCollisionHandler.h"

#include "InputHandler.h"

#include "MainApplication.h"
#include "PhysicsManager.h"

namespace CoABlaster
{

CharacterMovementController::CharacterMovementController(
        OgreBulletDynamics::RigidBody* p_body)
{
    m_body = p_body;
}

CharacterMovementController::~CharacterMovementController()
{
    
}

/// @todo TODO: dynamic (character size)
/// @todo TODO: maybe another way to prevent ray-self collision?
/// @todo TODO: different ray tresholds to approx sphere shape and prevent
///             jumping off corners
bool
CharacterMovementController::isCharacterOnGround()
{
    Ogre::Vector3 origin = m_body->getWorldPosition();
    Ogre::Vector3 target;
    
    // go one to the left
    origin.x--;
        
    /// test 3, left, middle, right (!!! adjust with char size !!!)
    for(int i = 0; i < 3; i++)
    {   
        // ray in negative y axis
        target = Ogre::Vector3(origin.x, origin.y - 1.1, origin.z);
        
        Ogre::Ray ray(origin, target);
        OgreBulletCollisions::CollisionClosestRayResultCallback
                cb(ray, PhysicsManager::get()->world());
    
        PhysicsManager::get()->world()->launchRay(cb);
        
        if(cb.doesCollide() && cb.getCollidedObject() != m_body 
                && (origin.y - cb.getCollisionPoint().y) < 1.1)
            return true;

        origin.x++;
    }
    
    return false;
}

void
CharacterMovementController::handleInput(double p_timeSinceLastFrame)
{
    OIS::Keyboard* keyboard = InputHandler::get()->keyboard();
        
    double speed = 4;
    double impulse = 1;
    double jumpSpeed = 10;
    
    bool runKeyPressed = false;
    bool jumpKeyPressed = false;

    
    MainApplication::lockPhysics();

    Ogre::Vector3 angularVelocity = Ogre::Vector3::ZERO; 
    Ogre::Vector3 linearImpulse = Ogre::Vector3::ZERO; 

    if(keyboard->isKeyDown(OIS::KC_RSHIFT) || 
        keyboard->isKeyDown(OIS::KC_LSHIFT))
    {    
        speed *= 2.8;
        impulse *= 2.8;
    }
    
    if(keyboard->isKeyDown(OIS::KC_LEFT))
    {
        angularVelocity += Ogre::Vector3(0,0,speed);
        linearImpulse += Ogre::Vector3(-impulse,0,0);
        runKeyPressed = true;
    }
    
    if(keyboard->isKeyDown(OIS::KC_RIGHT))
    {
        angularVelocity += Ogre::Vector3(0,0,-speed);
        linearImpulse += Ogre::Vector3(impulse,0,0);
        runKeyPressed = true;
    }
    
    if((keyboard->isKeyDown(OIS::KC_SPACE) || keyboard->isKeyDown(OIS::KC_UP)) 
            && isCharacterOnGround())
    // if((keyboard->isKeyDown(OIS::KC_SPACE) || keyboard->isKeyDown(OIS::KC_UP)) 
    //         && ((CharacterCollisionHandler*)m_body->
    //                 getCollisionHandler())->isCharacterOnGround())
    {
        jumpKeyPressed = true;
    }
    

    if(angularVelocity != Ogre::Vector3::ZERO || jumpKeyPressed)
        m_body->enableActiveState();

    if(!runKeyPressed)
    {
        Ogre::Vector3 vel = m_body->getLinearVelocity();
        if((vel != Ogre::Vector3::ZERO))
        {
            m_body->enableActiveState();
            m_body->setLinearVelocity(
                    vel.x * (pow(0.0001, p_timeSinceLastFrame)), vel.y, 0);
        }
    }

    if(jumpKeyPressed)
    {
        Ogre::Vector3 vel = m_body->getLinearVelocity();
        m_body->setLinearVelocity(vel.x, jumpSpeed, vel.z);
    }
    
    if(runKeyPressed)
    {
        m_body->setAngularVelocity(angularVelocity);
        m_body->applyImpulse(linearImpulse, Ogre::Vector3(0,0,0));
    }    
    
    Ogre::Vector3 pos = m_body->getWorldPosition();
    m_body->setPosition(pos.x, pos.y, 0);

    ((CharacterCollisionHandler*)m_body->
            getCollisionHandler())->setCharacterOnGround(false);

    MainApplication::unlockPhysics();
}

}
