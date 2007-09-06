
#ifndef COAJNR_DEPENDENCIES_INCLUDED
#define COAJNR_DEPENDENCIES_INCLUDED

#ifdef DEBUG
#define _DEBUG
#endif

/// general c++ input output streams
#include <iostream>

/// std::string
#include <string>

/// vector, map, list
#include <vector>
#include <map>
#include <list>

/// simple direct media layer
#include <SDL.h>
#include <SDL_thread.h>

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

/// lazy writers prefer uint
#ifndef uint
#  define uint unsigned int
#endif

#define ZPOS 0

#endif
