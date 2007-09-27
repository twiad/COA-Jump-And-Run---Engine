
#ifndef COAJNR_CONFIG_INCLUDED
#define COAJNR_CONFIG_INCLUDED

/// this is the string used to print the version identifier
#define COAJNR_VERION_STRING "CoA JnR Version 0.0.69"

/// target loops per second for the graphics thread
#define COAJNR_GRAPHICS_FPS 50

/// target loops per second for the phyics thread
#define COAJNR_PHYSICS_FPS 50

/// target loops per second for the interaction thread
/// changing this value may negate all behaviour tweaks
#define COAJNR_INTERACTION_FPS 20

/// scenery class name to init on startup
#define COAJNR_INIT_SCENE SceneryTest

/// physics acceleration factor (makes it look more realistic)
/// 1 for realtime, 2 looks more realistic
#define COAJNR_PHYSICS_SPEED_FACTOR 2

#endif
