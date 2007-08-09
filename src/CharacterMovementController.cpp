
#include "CharacterMovementController.h"

#include "CharacterCollisionHandler.h"

#include "InputHandler.h"

#include "MainApplication.h"

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

void
CharacterMovementController::handleInput(double p_timeSinceLastFrame)
{
    OIS::Keyboard* keyboard = InputHandler::get()->keyboard();
        
    double speed = 4;
    double impulse = 1;
    double jumpImpulse = 42;
    
    bool runKeyPressed = false;
    bool jumpKeyPressed = false;
    
    MainApplication::lockPhysics();

    Ogre::Vector3 angularVelocity = Ogre::Vector3::ZERO; 
    Ogre::Vector3 linearImpulse = Ogre::Vector3::ZERO;

    if(keyboard->isKeyDown(OIS::KC_RSHIFT) || 
        keyboard->isKeyDown(OIS::KC_LSHIFT))
        speed *= 2.8;
    
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
    
    if(keyboard->isKeyDown(OIS::KC_SPACE) && 
            ((CharacterCollisionHandler*)m_body->
                    getCollisionHandler())->isCharacterOnGround())
    {
        linearImpulse += Ogre::Vector3(0,jumpImpulse,0);
        jumpKeyPressed = true;
    }
    

    if(angularVelocity != Ogre::Vector3::ZERO 
            || linearImpulse != Ogre::Vector3::ZERO)
        m_body->enableActiveState();

    if(!runKeyPressed)
    {
        Ogre::Vector3 vel = m_body->getLinearVelocity();
        if((vel != Ogre::Vector3::ZERO))
        {
            m_body->enableActiveState();
            m_body->setLinearVelocity(
                    vel.x * (pow(0.01, p_timeSinceLastFrame)), vel.y, 0);
        }
    }

    if(runKeyPressed || jumpKeyPressed)
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
