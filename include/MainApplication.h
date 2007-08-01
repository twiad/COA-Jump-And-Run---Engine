
#ifndef COABLASTER_MAINAPPLICATION_INCLUDED
#define COABLASTER_MAINAPPLICATION_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

/**
 * the applications root
 */
class MainApplication
{
    static SDL_mutex* m_graphicsLock;    
    static SDL_mutex* m_physicsLock;

    static void initialize();

public:
    MainApplication() {};

    virtual ~MainApplication() {};    
    
    static void go();

    static int graphicsWorkerThread(void* data);
    static int physicsWorkerThread(void* data);

    static int lockGraphics();
    static int unlockGraphics();
    
    static int lockPhysics();
    static int unlockPhysics();
};

}

#endif
