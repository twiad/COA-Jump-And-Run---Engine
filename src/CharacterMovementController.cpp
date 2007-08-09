
#include "CharacterMovementController.h"

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
CharacterMovementController::handleInput()
{
    OIS::Keyboard* keyboard = InputHandler::get()->keyboard();
    
    double speed = 8;
    
    MainApplication::lockPhysics();

    Ogre::Vector3 linearVelocity = Ogre::Vector3::ZERO; 
    linearVelocity.y += m_body->getLinearVelocity().y;
    
    linearVelocity.x += m_body->getLinearVelocity().x * 0.2;
    linearVelocity.z += m_body->getLinearVelocity().z * 0.2;

    if(keyboard->isKeyDown(OIS::KC_RSHIFT) || 
        keyboard->isKeyDown(OIS::KC_LSHIFT))
        speed *= 1.8;
    
    // if(keyboard->isKeyDown(OIS::KC_UP))
    //     linearVelocity += Ogre::Vector3(speed,0,0);
    // 
    // if(keyboard->isKeyDown(OIS::KC_DOWN))
    //     linearVelocity += Ogre::Vector3(-speed,0,0);

    if(keyboard->isKeyDown(OIS::KC_LEFT))
        linearVelocity += Ogre::Vector3(-speed,0,0);

    if(keyboard->isKeyDown(OIS::KC_RIGHT))
        linearVelocity += Ogre::Vector3(speed,0,0);

    if(linearVelocity != Ogre::Vector3::ZERO)
    {
        m_body->enableActiveState();
        m_body->setLinearVelocity(linearVelocity);
    }
    
    if(keyboard->isKeyDown(OIS::KC_SPACE))
        m_body->applyImpulse(Ogre::Vector3(0,10,0), Ogre::Vector3(0,0,0));

    // for cube collision shape...
    // m_body->setAngularVelocity(Ogre::Vector3::ZERO);
    // m_body->setOrientation(0,0,0,1);
    
    Ogre::Vector3 pos = m_body->getWorldPosition();
    m_body->setPosition(pos.x, pos.y, 0);

    MainApplication::unlockPhysics();
}

}
