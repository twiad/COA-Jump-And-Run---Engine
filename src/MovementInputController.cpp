
#include "MovementInputController.h"

#include "InputHandler.h"

#include "MainApplication.h"

namespace CoABlaster
{

MovementInputController::MovementInputController(
        OgreBulletDynamics::RigidBody* p_body)
{
    m_body = p_body;
}

MovementInputController::~MovementInputController()
{
    
}

void
MovementInputController::handleInput()
{
    OIS::Keyboard* keyboard = InputHandler::get()->keyboard();
    
    Ogre::Vector3 linearVelocity = m_body->getLinearVelocity();
    
    if(keyboard->isKeyDown(OIS::KC_UP))
        linearVelocity += Ogre::Vector3(1,0,0);

    if(keyboard->isKeyDown(OIS::KC_DOWN))
        linearVelocity += Ogre::Vector3(-1,0,0);

    if(keyboard->isKeyDown(OIS::KC_LEFT))
        linearVelocity += Ogre::Vector3(0,0,1);

    if(keyboard->isKeyDown(OIS::KC_RIGHT))
        linearVelocity += Ogre::Vector3(0,0,-1);

    // MainApplication::lockPhysics();

    m_body->enableActiveState();
    m_body->setLinearVelocity(linearVelocity);

    if(keyboard->isKeyDown(OIS::KC_SPACE))
        m_body->applyImpulse(Ogre::Vector3(0,3,0), Ogre::Vector3(0,0,0));

    // MainApplication::unlockPhysics();
}

}
