
#ifndef COAJNR_MYRAYSCENEQUERYLISTENER_INCLUDED
#define COAJNR_MYRAYSCENEQUERYLISTENER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class MyRaySceneQueryListener : public Ogre::RaySceneQueryListener
{
public:
	inline bool queryResult(Ogre::SceneQuery::WorldFragment *fragment, Ogre::Real distance)
    {
    	resultDistance = distance;
    	return false;
    }
    inline bool queryResult(Ogre::MovableObject* obj, Ogre::Real distance)
    {
    	resultDistance = distance;
    	return false;
    }

    float resultDistance;
};

}
#endif
