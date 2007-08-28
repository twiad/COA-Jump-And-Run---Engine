
#include "CharacterMovementController.h"

#include "Character.h"
#include "InputHandler.h"

#include "MainApplication.h"
#include "PhysicsManager.h"

namespace CoABlaster
{

CharacterMovementController::CharacterMovementController(Character* p_character)
{
    assert(p_character && "character must not be null");
    m_character = p_character;
}

CharacterMovementController::~CharacterMovementController()
{
    
}

void
CharacterMovementController::handleInput(double p_timeSinceLastFrame)
{    
    OIS::Keyboard* keyboard = InputHandler::get()->keyboard();

    if(keyboard->isKeyDown(OIS::KC_LEFT))
        m_character->moveLeft(p_timeSinceLastFrame);
    if(keyboard->isKeyDown(OIS::KC_RIGHT))
        m_character->moveRight(p_timeSinceLastFrame);
    if(keyboard->isKeyDown(OIS::KC_SPACE) || keyboard->isKeyDown(OIS::KC_UP)) 
        m_character->jump(p_timeSinceLastFrame);
}

}
