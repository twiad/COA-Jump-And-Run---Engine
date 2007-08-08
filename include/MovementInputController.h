
#ifndef COABLASTER_MOVEMENTINPUTCONTROLLER_INCLUDED
#define COABLASTER_MOVEMENTINPUTCONTROLLER_INCLUDED

#include "Dependencies.h"

#include "InputController.h"

namespace CoABlaster
{

class MovementInputController : public InputController
{

    OgreBulletDynamics::RigidBody* m_body;

public:
    MovementInputController(OgreBulletDynamics::RigidBody* body);

    virtual ~MovementInputController();
    
    virtual void handleInput();
};

}

#endif
