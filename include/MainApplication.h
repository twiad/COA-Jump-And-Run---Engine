
#ifndef COAJNR_MAINAPPLICATION_INCLUDED
#define COAJNR_MAINAPPLICATION_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

/**
 * the applications root.
 *
 * this class manages the threads main loops, initialization and cleanup
 * of the application. Thread synchroniztion is also handled by this class.
 */
class MainApplication
{
    /// lock for ogre in general and ogre thread data
    static SDL_mutex* m_graphicsLock;
    
    /// lock for bullet in general and bullet thread data
    static SDL_mutex* m_physicsLock;
    
    /// physics waits for this until it starts up (needs ogre scene manager)
    static SDL_cond* m_physicsCanStartCondition;
    
    /// mutex for the physics start condition
    static SDL_mutex* m_physicsCanStartMutex;

    /// thread object for ogre rendering
    static SDL_Thread* m_graphicsThread;

    /// thread object for bullet physics
    static SDL_Thread* m_physicsThread;
    
    /// thread object for user interaction, collision handling, logic, etc.
    static SDL_Thread* m_interactionThread;

#ifdef _DEBUG
    /// start time of the application
    static uint m_startTime;
    
    /// number of physics loops
    static uint m_physicsUpdates;

    /// number of graphics loops
    static uint m_graphicsUpdates;

    /// number of interaction thread loops
    static uint m_interactionUpdates;

    static double m_physicsWaitTime;
    static double m_graphicsWaitTime;
    static double m_interactionWaitTime;
    
#endif
    
    /// when set to false, physics thread terminates after next update
    static bool m_physicsKeepRunning;
    
    /// when set to false, graphics thread terminates after next update
    static bool m_graphicsKeepRunning;

    /// initialize the application
    static void initialize();
    
    /// cleanup the applications resources
    static void cleanup();

    /// call waits for the physics can start condition signaled by graphics
    static void waitPhysicsCanStart();
    
    /// signals the physics thread to start, called by graphics
    static void signalPhysicsCanStart();

public:
    /**
     * ctor.
     */
    MainApplication() {};

    /**
     * dtor.
     */
    virtual ~MainApplication() {};    
    
    /**
     * runs the application.
     */
    static void go();

    /**
     * graphics thread main loop methode.
     */
    static int graphicsWorkerThread(void* data);

    /**
     * physics thread main loop methode.
     */
    static int physicsWorkerThread(void* data);

    /**
     * interaction thread main loop methode.
     */
    static int interactionWorkerThread(void* data);

    /**
     * lock the graphics context.
     */
    static int lockGraphics();

    /**
     * unlock the graphics context.
     */
    static int unlockGraphics();
    
    /**
     * lock the physics context.
     */
    static int lockPhysics();

    /**
     * unlock the physics context.
     */
    static int unlockPhysics();

};

}

#endif
