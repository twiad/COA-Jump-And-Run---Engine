#include "MainApplication.h"

#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "SceneryTest.h"

#define COAJNR_GRAPHICS_FPS 60
#define COAJNR_PHYSICS_FPS 60
#define COAJNR_INIT_SCENE SceneryTest
#define COAJNR_PHYSICS_SPEED_FACTOR 2

namespace CoABlaster
{
    
SDL_mutex*  MainApplication::m_graphicsLock = 0;
SDL_mutex*  MainApplication::m_physicsLock = 0;
bool        MainApplication::m_physicsKeepRunning = true;    
SDL_cond*   MainApplication::m_physicsCanStartCondition = 0;
SDL_mutex*  MainApplication::m_physicsCanStartMutex = 0;
SDL_Thread* MainApplication::m_graphicsThread = 0;
SDL_Thread* MainApplication::m_physicsThread = 0;

#ifdef _DEBUG
uint        MainApplication::m_physicsUpdates = 0;
uint        MainApplication::m_graphicsUpdates = 0;
#endif

void
MainApplication::initialize()
{
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
MainApplication::go()
{
    std::cout << COABLASTER_VERION_STRING << std::endl;

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);

    initialize();

    SDL_LockMutex(m_physicsCanStartMutex);
    m_physicsThread = SDL_CreateThread(
            &(MainApplication::physicsWorkerThread), 0);    
    SDL_UnlockMutex(m_physicsCanStartMutex);

    m_graphicsThread = SDL_CreateThread(
            &(MainApplication::graphicsWorkerThread), 0);
    
    SDL_WaitThread(m_physicsThread, 0);
    SDL_WaitThread(m_graphicsThread, 0);

#ifdef _DEBUG
    std::cout << "Physics Update Cycles : " << m_physicsUpdates << std::endl;
    std::cout << "Graphics Update Cycles: " << m_graphicsUpdates << std::endl;
#endif

    SDL_Quit();
}

int 
MainApplication::graphicsWorkerThread(void* data)
{
    lockGraphics();

    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    uint timeToWait = 0;
    
    double elapsedSeconds = 0;
    double updateTime = 0;
    
    uint minFrameTime = 1000 / COAJNR_GRAPHICS_FPS;
    
    GraphicsManager::get()->init(new COAJNR_INIT_SCENE);

    unlockGraphics();

    signalPhysicsCanStart();

    while(true)
    {
#ifdef _DEBUG
        m_graphicsUpdates++;
#endif
        
        startTime = SDL_GetTicks();

        lockGraphics();

        if(GraphicsManager::get()->update(updateTime) == false)
        {
            unlockGraphics();
        
            lockPhysics();
            m_physicsKeepRunning = false;
            unlockPhysics();
                    
            break;
        }
        
        unlockGraphics();

        elapsedMilliSeconds = SDL_GetTicks() - startTime;
        elapsedSeconds = elapsedMilliSeconds / 1000.0f;
        updateTime = std::max<int>(minFrameTime, elapsedMilliSeconds) / 1000.0f;
        timeToWait = std::max<int>(minFrameTime - elapsedMilliSeconds, 0);
        
        SDL_Delay(timeToWait);
        
        if(!timeToWait)
            std::cout << "WARNING! graphics update too slow!" 
                    << std::endl;

#ifdef _DEBUG
        if(m_graphicsUpdates % (COAJNR_GRAPHICS_FPS * 2) == 0)
        {
            const Ogre::RenderTarget::FrameStats& stats = 
                    GraphicsManager::get()->window()->getStatistics();
            std::cout << "Graphics FPS: " << stats.avgFPS << std::endl;
        }
#endif
    }

#ifdef _DEBUG
    delete GraphicsManager::get();
#endif

    return 0;
}

int 
MainApplication::physicsWorkerThread(void* data)
{
    waitPhysicsCanStart();

    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    uint timeToWait = 0;
    double elapsedSeconds = 0;
    double updateTime = 0;
    
    uint minFrameTime = 1000 / COAJNR_PHYSICS_FPS;
    
    while(m_physicsKeepRunning)
    {
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
        elapsedSeconds = elapsedMilliSeconds / 1000.0f;
        updateTime = std::max<int>(minFrameTime, elapsedMilliSeconds) / 1000.0f;
        timeToWait = std::max<int>(minFrameTime - elapsedMilliSeconds, 0);

        SDL_Delay(timeToWait);
        
        if(!timeToWait)
            std::cout << "WARNING! physics update too slow!" 
                << std::endl;
    }

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
