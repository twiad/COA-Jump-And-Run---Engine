#include "MainApplication.h"

#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "SceneryTest.h"
#include "InteractionManager.h"


namespace CoAJnR
{

SDL_mutex*  MainApplication::m_graphicsLock = 0;
SDL_mutex*  MainApplication::m_physicsLock = 0;
bool        MainApplication::m_physicsKeepRunning = true;    
bool        MainApplication::m_graphicsKeepRunning = true;    
SDL_cond*   MainApplication::m_physicsCanStartCondition = 0;
SDL_mutex*  MainApplication::m_physicsCanStartMutex = 0;
SDL_Thread* MainApplication::m_graphicsThread = 0;
SDL_Thread* MainApplication::m_physicsThread = 0;
SDL_Thread* MainApplication::m_interactionThread = 0;

#ifdef _DEBUG
uint        MainApplication::m_startTime = 0;
uint        MainApplication::m_physicsUpdates = 0;
uint        MainApplication::m_graphicsUpdates = 0;
uint        MainApplication::m_interactionUpdates = 0;
double      MainApplication::m_physicsWaitTime = 0;
double      MainApplication::m_graphicsWaitTime = 0;
double      MainApplication::m_interactionWaitTime = 0;
#endif

void
MainApplication::initialize()
{
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);

    m_graphicsLock = SDL_CreateMutex();
    m_physicsLock = SDL_CreateMutex();

    m_physicsCanStartCondition = SDL_CreateCond();
    m_physicsCanStartMutex = SDL_CreateMutex();

    assert(m_graphicsLock);
    assert(m_physicsLock);
    assert(m_physicsCanStartCondition);
    assert(m_physicsCanStartMutex);
}

void
MainApplication::cleanup()
{
    /// @todo TODO: clean exit
    // delete PhysicsManager::get();
    // delete GraphicsManager::get();

    SDL_Quit();
}

void
MainApplication::go()
{
    std::cout << COAJNR_VERION_STRING << std::endl;

    initialize();

#ifdef _DEBUG
    m_startTime = SDL_GetTicks();
#endif

    SDL_LockMutex(m_physicsCanStartMutex);
    m_physicsThread = SDL_CreateThread(
            &(MainApplication::physicsWorkerThread), 0);    
    SDL_UnlockMutex(m_physicsCanStartMutex);

    m_graphicsThread = SDL_CreateThread(
            &(MainApplication::graphicsWorkerThread), 0);
    
    m_interactionThread = SDL_CreateThread(
            &(MainApplication::interactionWorkerThread), 0);
    
    SDL_WaitThread(m_physicsThread, 0);
    SDL_WaitThread(m_graphicsThread, 0);
    SDL_WaitThread(m_interactionThread, 0);

    cleanup();

#ifdef _DEBUG
    uint runTimeMS = SDL_GetTicks() - m_startTime;
    double runTimeSec = runTimeMS / 1000.0;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "*** STATISTICS ***" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Physics Update Cycles: " 
            << m_physicsUpdates << std::endl;
    std::cout << "Physics Update Cycles per second: " 
            << m_physicsUpdates /  runTimeSec << std::endl;
    std::cout << "Graphics Update Cycles: " 
            << m_graphicsUpdates << std::endl;
    std::cout << "Physics Update Cycles per second: " 
            << m_graphicsUpdates /  runTimeSec << std::endl;
    std::cout << "Interaction Update Cycles: " 
            << m_interactionUpdates << std::endl;
    std::cout << "Interaction Update Cycles per second: " 
            << m_interactionUpdates /  runTimeSec << std::endl;
    
    std::cout << "Physics Usage: " << (int)(100 -
            ((m_physicsWaitTime / m_physicsUpdates) / 
            (1.0 / COAJNR_PHYSICS_FPS) * 100)) << "%"<< std::endl;
    std::cout << "Graphics Usage: " << (int)(100 -
            ((m_graphicsWaitTime / m_graphicsUpdates) /
            (1.0 / COAJNR_GRAPHICS_FPS) * 100)) << "%"<< std::endl;
    std::cout << "Interaction Usage: " << (int)(100 -
            ((m_interactionWaitTime / m_interactionUpdates) / 
            (1.0 / COAJNR_INTERACTION_FPS) * 100)) << "%"<< std::endl;

    std::cout << std::endl;
#endif
}

int 
MainApplication::interactionWorkerThread(void* data)
{
    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    uint timeToWait = 0;
    
    double updateTime = 0;
    
    uint minFrameTime = 1000 / COAJNR_INTERACTION_FPS;

    while(true)
    {
#ifdef _DEBUG
        m_interactionUpdates++;
#endif

        startTime = SDL_GetTicks();

        MainApplication::lockPhysics();
        MainApplication::lockGraphics();
        
        if(!InteractionManager::get()->update())
        {
            m_graphicsKeepRunning = false;
            MainApplication::unlockPhysics();
            MainApplication::unlockGraphics();            
            break;
        }
        
        MainApplication::unlockPhysics();
        MainApplication::unlockGraphics();
        
        elapsedMilliSeconds = SDL_GetTicks() - startTime;
        updateTime = std::max<int>(minFrameTime, elapsedMilliSeconds) / 1000.0f;
        timeToWait = std::max<int>(minFrameTime - elapsedMilliSeconds, 0);
        
        m_interactionWaitTime += timeToWait / 1000.0f;
        
        SDL_Delay(timeToWait);
        
        // if(!timeToWait)
        //     std::cout << "WARNING! interaction update too slow!" 
        //             << std::endl;

    }

    return 0;
}

int 
MainApplication::graphicsWorkerThread(void* p_data)
{
    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    uint timeToWait = 0;
    
    double updateTime = 0;
    
    uint minFrameTime = 1000 / COAJNR_GRAPHICS_FPS;
    
    lockGraphics();
    GraphicsManager::get()->init(new COAJNR_INIT_SCENE);
    unlockGraphics();

    signalPhysicsCanStart();

    lockGraphics();
    while(m_graphicsKeepRunning)
    {
        unlockGraphics();

#ifdef _DEBUG
        m_graphicsUpdates++;
#endif
        
        startTime = SDL_GetTicks();

        lockGraphics();
        GraphicsManager::get()->update(updateTime);
        unlockGraphics();
        
        elapsedMilliSeconds = SDL_GetTicks() - startTime;
        updateTime = std::max<int>(minFrameTime, elapsedMilliSeconds) / 1000.0f;
        timeToWait = std::max<int>(minFrameTime - elapsedMilliSeconds, 0);
        
        m_graphicsWaitTime += timeToWait / 1000.0f;

        SDL_Delay(timeToWait);
        
        // if(!timeToWait)
        //     std::cout << "WARNING! graphics update too slow!" 
        //             << std::endl;

#ifdef _DEBUG
        if(m_graphicsUpdates % (COAJNR_GRAPHICS_FPS * 2) == 0)
        {
            const Ogre::RenderTarget::FrameStats& stats = 
                    GraphicsManager::get()->window()->getStatistics();
            std::cout << "Graphics FPS: " << stats.avgFPS << std::endl;
        }
#endif

        lockGraphics();
    }
    unlockGraphics();

    lockPhysics();
    m_physicsKeepRunning = false;
    unlockPhysics();

    return 0;
}

int 
MainApplication::physicsWorkerThread(void* p_data)
{
    /// @todo TODO: make the following line possible
    // sleep(3); // breakes the synchronization
    
    waitPhysicsCanStart();

    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    uint timeToWait = 0;

    double updateTime = 0;
    
    uint minFrameTime = 1000 / COAJNR_PHYSICS_FPS;
    
    lockPhysics();
    while(m_physicsKeepRunning)
    {
        unlockPhysics();

#ifdef _DEBUG
        m_physicsUpdates++;
#endif
        
        startTime = SDL_GetTicks();

        lockPhysics();
        PhysicsManager::get()->update(updateTime * COAJNR_PHYSICS_SPEED_FACTOR);
        unlockPhysics();
        
        lockGraphics();
        PhysicsManager::get()->synchronize();
        unlockGraphics();
        
        elapsedMilliSeconds = SDL_GetTicks() - startTime;
        updateTime = std::max<int>(minFrameTime, elapsedMilliSeconds) / 1000.0f;
        timeToWait = std::max<int>(minFrameTime - elapsedMilliSeconds, 0);

        /// @todo TODO: measure real phy update time
        m_physicsWaitTime += timeToWait / 1000.0f;

        SDL_Delay(timeToWait);
        
        // if(!timeToWait)
        //     std::cout << "WARNING! physics update too slow!" 
        //         << std::endl;

        lockPhysics();
    }
    unlockPhysics();

    return 0;
}

int
MainApplication::lockGraphics()
{
    return SDL_LockMutex(m_graphicsLock);
}

int
MainApplication::unlockGraphics()
{
    return SDL_UnlockMutex(m_graphicsLock);
}

int
MainApplication::lockPhysics()
{
    return SDL_LockMutex(m_physicsLock);
}

int
MainApplication::unlockPhysics()
{
    return SDL_UnlockMutex(m_physicsLock);
}

void
MainApplication::waitPhysicsCanStart()
{
    SDL_LockMutex(m_physicsCanStartMutex);
    SDL_CondWait(m_physicsCanStartCondition, m_physicsCanStartMutex);
}

void
MainApplication::signalPhysicsCanStart()
{
    SDL_CondSignal(m_physicsCanStartCondition);
}

}
