
#ifndef COAJNR_GRASSFRAMELISTENER_INCLUDED
#define COAJNR_GRASSFRAMELISTENER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class GrassFrameListener :public Ogre::FrameListener
{
public:
	GrassFrameListener(GrassLoader *Gloader){grassloader = Gloader;}

	bool frameStarted(const Ogre::FrameEvent& evt);

private:
	GrassLoader *grassloader;
};

}
#endif
