
#ifndef COAJNR_SCENERY_INCLUDED
#define COAJNR_SCENERY_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

/**
 * interface for sceneries, setting up environment.
 */
class Scenery
{

public:
    Scenery() { }
    
    virtual ~Scenery() { }
    
    virtual void setup() = 0;
    
    virtual void cleanup() = 0;
};

}

#endif
