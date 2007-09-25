#include "GrassFrameListener.h"

#include "GraphicsManager.h"
#include "Scenery.h"

namespace CoAJnR
{

bool GrassFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	GraphicsManager::get()->scenery()->trees->update();
	GraphicsManager::get()->scenery()->grass->update();
	grassloader->updateAnimation();
	return true; 
}

}
