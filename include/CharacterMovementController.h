
#ifndef COABLASTER_CHARACTERMOVEMENTCONTROLLER_INCLUDED
#define COABLASTER_CHARACTERMOVEMENTCONTROLLER_INCLUDED

#include "Dependencies.h"

#include "InputController.h"

namespace CoABlaster
{

class CharacterMovementController : public InputController
{

    OgreBulletDynamics::RigidBody* m_body;

    bool isCharacterOnGround();

public:
    CharacterMovementController(OgreBulletDynamics::RigidBody* body);

    virtual ~CharacterMovementController();
    
    virtual void handleInput(double timeSinceLastFrame);
};

}

#endif
