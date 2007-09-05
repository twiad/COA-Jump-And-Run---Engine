
#include "CharacterMovementController.h"

#include "Character.h"
#include "InputHandler.h"

#include "MainApplication.h"
#include "PhysicsManager.h"

namespace CoAJnR
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
CharacterMovementController::handleInput()
{    
    OIS::Keyboard* keyboard = InputHandler::get()->keyboard();

    if(keyboard->isKeyDown(OIS::KC_LEFT))
        m_character->moveLeft(0.1);
    if(keyboard->isKeyDown(OIS::KC_RIGHT))
        m_character->moveRight(0.1);
    if(keyboard->isKeyDown(OIS::KC_UP))
        m_character->jump(0.1);

    if(keyboard->isKeyDown(OIS::KC_SPACE))
        m_character->grab();
    else
    {
        if(keyboard->isKeyDown(OIS::KC_DOWN))
            m_character->ungrab();
        else
            m_character->throwAway();
    }
}

}
