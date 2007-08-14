
#ifndef COABLASTER_CHARACTERMOVEMENTCONTROLLER_INCLUDED
#define COABLASTER_CHARACTERMOVEMENTCONTROLLER_INCLUDED

#include "Dependencies.h"

#include "InputController.h"

namespace CoABlaster
{

class Character;

class CharacterMovementController : public InputController
{
    Character* m_character;

public:
    CharacterMovementController(Character* character);

    virtual ~CharacterMovementController();
    
    virtual void handleInput(double timeSinceLastFrame);
};

}

#endif
