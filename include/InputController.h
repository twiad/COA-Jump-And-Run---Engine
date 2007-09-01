
#ifndef COABLASTER_INPUTCONTROLLER_INCLUDED
#define COABLASTER_INPUTCONTROLLER_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

/**
 * handles certain input events.
 */
class InputController
{

public:
    InputController() { }
    
    virtual ~InputController() { }
    
    virtual void handleInput() = 0;
};

}

#endif
