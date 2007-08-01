
#ifndef COABLASTER_DEPENDENCIES_INCLUDED
#define COABLASTER_DEPENDENCIES_INCLUDED

/// general c++ input output streams
#include <iostream>

/// std::string
#include <string>

/// simple direct media layer
#include <SDL.h>

/// ogre rendering engine
#include <Ogre.h>

/// ois input system
#include <OIS.h>

/// ogre bullet physics binding
#define _PRECOMP
#include "OgreBulletCollisions.h"
#include "OgreBulletDynamics.h"

/// CoA-Blaster compile time configuration
#include "Config.h"

#endif
