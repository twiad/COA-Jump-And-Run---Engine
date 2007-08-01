#include "MainApplication.h"

#include "GraphicsManager.h"
#include "SceneryTest.h"

#define COAJNR_GRAPHICS_FPS 100;
#define COAJNR_PHYSICS_FPS 100;
#define COAJNR_INIT_SCENE SceneryTest

namespace CoABlaster
{
    
SDL_mutex* MainApplication::m_graphicsLock = 0;
SDL_mutex* MainApplication::m_physicsLock = 0;
    
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
    
    graphicsThread = 
            SDL_CreateThread(&(MainApplication::graphicsWorkerThread), 0);
    
    SDL_WaitThread(graphicsThread, 0);
    
    // delete gm;
    
    SDL_Quit();
}

int 
MainApplication::graphicsWorkerThread(void* data)
{
    uint startTime = 0;
    uint elapsedMilliSeconds = 0;
    double elapsedSeconds = 0;
    
    uint minFrameTime = 1000 / COAJNR_GRAPHICS_FPS;
    
    GraphicsManager::get()->init(new COAJNR_INIT_SCENE);

    while(true)
    {
        startTime = SDL_GetTicks();
        
        lockGraphics();
        
        if(GraphicsManager::get()->update(0) == false)
            break; /// @todo TODO: do a clean shut down here
        
        elapsedMilliSeconds = SDL_GetTicks() - startTime;
        elapsedSeconds = elapsedMilliSeconds / 1000.0f;
        
        unlockGraphics();
        
        SDL_Delay(std::max<int>(minFrameTime - elapsedMilliSeconds, 0));
        
        // const Ogre::RenderTarget::FrameStats& stats = 
        //         GraphicsManager::get()->window()->getStatistics();
        // std::cout << "avgFPS " << stats.avgFPS << std::endl;
    }

    return 0;
}

int 
MainApplication::physicsWorkerThread(void* data)
{
    
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
