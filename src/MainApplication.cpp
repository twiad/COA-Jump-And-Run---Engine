#include "MainApplication.h"

#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "SceneryTest.h"

#define COAJNR_GRAPHICS_FPS 100;
#define COAJNR_PHYSICS_FPS 100;
#define COAJNR_INIT_SCENE SceneryTest

namespace CoABlaster
{
    
SDL_mutex*  MainApplication::m_graphicsLock = 0;
SDL_mutex*  MainApplication::m_physicsLock = 0;
bool        MainApplication::m_physicsKeepRunning = true;    


void
MainApplication::initialize()
{
    m_graphicsLock = SDL_CreateMutex();
    m_physicsLock = SDL_CreateMutex();

    assert(m_graphicsLock && m_physicsLock);
}

void
MainApplication::go()
{
    std::cout << COABLASTER_VERION_STRING << std::endl;

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);

    initialize();

    SDL_Thread* graphicsThread;
    SDL_Thread* physicsThread;
    
    graphicsThread = 
            SDL_CreateThread(&(MainApplication::graphicsWorkerThread), 0);
    
    SDL_Delay(5000);

    physicsThread =
            SDL_CreateThread(&(MainApplication::physicsWorkerThread), 0);
    
    SDL_WaitThread(graphicsThread, 0);

    m_physicsKeepRunning = false;

    SDL_WaitThread(physicsThread, 0);

    SDL_Quit();
}

int 
MainApplication::graphicsWorkerThread(void* data)
{
    lockGraphics();

    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    double elapsedSeconds = 0;
    
    uint minFrameTime = 1000 / COAJNR_GRAPHICS_FPS;
    
    GraphicsManager::get()->init(new COAJNR_INIT_SCENE);

    unlockGraphics();

    while(true)
    {
        startTime = SDL_GetTicks();

        lockGraphics();

        if(GraphicsManager::get()->update(0) == false)
        {
            unlockGraphics();
            break;
        }
        
        elapsedMilliSeconds = SDL_GetTicks() - startTime;
        elapsedSeconds = elapsedMilliSeconds / 1000.0f;
        
        unlockGraphics();
        
        // SDL_Delay(std::max<int>(minFrameTime - elapsedMilliSeconds, 0));
        
        // const Ogre::RenderTarget::FrameStats& stats = 
        //         GraphicsManager::get()->window()->getStatistics();
        // std::cout << "avgFPS " << stats.avgFPS << std::endl;
    }

    delete GraphicsManager::get();

    return 0;
}

int 
MainApplication::physicsWorkerThread(void* data)
{
    while(m_physicsKeepRunning)
    {
        lockPhysics();
        PhysicsManager::get()->update(0.1);
        lockGraphics();
        PhysicsManager::get()->synchronize();
        unlockGraphics();
        unlockPhysics();
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




}
