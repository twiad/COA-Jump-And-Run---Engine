
#ifndef COABLASTER_SCENERY_INCLUDED
#define COABLASTER_SCENERY_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
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
