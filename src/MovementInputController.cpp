
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
    
    double speed = 8;
    
    MainApplication::lockPhysics();

    Ogre::Vector3 linearVelocity = Ogre::Vector3::ZERO; 
    linearVelocity.y += m_body->getLinearVelocity().y;

    if(keyboard->isKeyDown(OIS::KC_RSHIFT) || 
        keyboard->isKeyDown(OIS::KC_LSHIFT))
        speed *= 1.4;
    
    if(keyboard->isKeyDown(OIS::KC_UP))
        linearVelocity += Ogre::Vector3(speed,0,0);

    if(keyboard->isKeyDown(OIS::KC_DOWN))
        linearVelocity += Ogre::Vector3(-speed,0,0);

    if(keyboard->isKeyDown(OIS::KC_LEFT))
        linearVelocity += Ogre::Vector3(0,0,speed);

    if(keyboard->isKeyDown(OIS::KC_RIGHT))
        linearVelocity += Ogre::Vector3(0,0,-speed);

    if(linearVelocity != Ogre::Vector3::ZERO)
    {
        m_body->enableActiveState();
        m_body->setLinearVelocity(linearVelocity);
    }
    
    if(keyboard->isKeyDown(OIS::KC_SPACE))
        m_body->applyImpulse(Ogre::Vector3(0,10,0), Ogre::Vector3(0,0,0));

    m_body->setAngularVelocity(Ogre::Vector3::ZERO);
    m_body->setOrientation(0,0,0,1);

    MainApplication::unlockPhysics();
}

}
