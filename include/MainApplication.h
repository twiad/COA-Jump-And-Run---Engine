
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
    
    static SDL_cond* m_physicsCanStartCondition;
    static SDL_mutex* m_physicsCanStartMutex;

    static SDL_Thread* m_graphicsThread;
    static SDL_Thread* m_physicsThread;

    static uint m_physicsUpdates;
    static uint m_graphicsUpdates;
    
    static bool m_physicsKeepRunning;

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

    static void waitPhysicsCanStart();
    static void signalPhysicsCanStart();
};

}

#endif
