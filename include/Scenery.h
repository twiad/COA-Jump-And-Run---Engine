
#ifndef COAJNR_SCENERY_INCLUDED
#define COAJNR_SCENERY_INCLUDED

#include "Dependencies.h"
#include "myRaySceneQueryListener.h"

namespace CoAJnR
{

/**
 * interface for sceneries, setting up environment.
 */
class Scenery
{

public:

    PagedGeometry* grass;
    PagedGeometry* trees;
    
    Ogre::RaySceneQuery* raySceneQuery;
    Ogre::Ray updateRay;
    MyRaySceneQueryListener* raySceneQueryListener;
    
    Scenery() { }
    
    virtual ~Scenery() { }
    
    virtual void setup() = 0;
    
    virtual void cleanup() = 0;
};

}

#endif
