
#ifndef COAJNR_INPUTCONTROLLER_INCLUDED
#define COAJNR_INPUTCONTROLLER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
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
