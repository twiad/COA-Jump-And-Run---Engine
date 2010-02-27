/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2007  Adrian Jäkel                       *
 *                     Copyright (C) 2007  Franz Köhler                       *
 *                     Copyright (C) 2007  Robert Timm                        *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/


#include "MainApplication.h"

#include "DataStorage.h"
#include "DeferredCall.h"
#include "IdManager.h"
#include "InputManager.h"
#include "GraphicsManager.h"
#include "LogicsManager.h"
#include "ThreadPool.h"
#include "UserThreadPool.h"
#include "PhysicsManager.h"
#include "TimeManager.h"

#include "ComponentContainer.h"

#include "GraphicsMeshComponent.h"
#include "GraphicsParticleComponent.h"
#include "GraphicsStaticMeshComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsConstraintsComponent.h"
#include "GraphicsCameraComponent.h"
#include "GraphicsSkyComponent.h"

#include "CollisionHandlerComponentTest.h"

#include "BufferedInputHandlerComponentTest.h"
#include "UnbufferedInputHandlerComponentTest.h"

#include "ContentManager.h"

#include "InputEvent.h"

#include "TimerComponent.h"


namespace CoAJnR
{

boost::mutex MainApplication::m_windowMessageQueueMutex;


class TimerComponentTest : public TimerComponent
{
public:
    std::string defaultName() const
    {
        return std::string("TimerTest");
    }
    
    TimerComponentTest() : TimerComponent(0.5)
    {
        
    }
    
    void update(TimeValue timeSinceLastUpdate)
    {
        std::cout << "<<<<<< " << timeSinceLastUpdate << std::endl;
    }
};

/**
 * handles key event for application shutdown.
 */
class ShutdownKeyListener : public BufferedInputHandlerComponent
{
public:
    virtual void keyReleased(const OIS::KeyEvent arg) 
    { 
        if(arg.key == OIS::KC_Q)
            MainApplication::shutdown();
    }
    
    std::string defaultName() const
    {
        return std::string("ShutdownKeyListener");
    }
};


void createTestScene()
{
    // ComponentContainerPtr camera = 
    //         ContentManager::get().createDynamicObject(
    //                 "QuestionCube.mesh", Ogre::Vector3(0, 0, -20));
    // camera->attach(ComponentPtr(new GraphicsCameraComponent));
            
    ComponentContainerPtr camera = 
            ContentManager::get().createEmptyContainer();
    camera->attach(ComponentPtr(
            new PositionComponent(
                    Ogre::Vector3(0, -7, 30),
                    Ogre::Quaternion(
                            Ogre::Radian(Ogre::Degree(0)), 
                            Ogre::Vector3(0,1,0)))));
    camera->attach(ComponentPtr(new GraphicsCameraComponent()));

    // -------------------------------------------------------------------------

    ComponentContainerPtr prot = ContentManager::get().createEmptyContainer();
    ContentManager::get().addToGroup(prot, "protected");
    prot->attach(ComponentPtr(new UnbufferedInputHandlerComponentTest));
    prot->attach(ComponentPtr(new ShutdownKeyListener));
    // prot->attach(ComponentPtr(new CombinedComponent));



    ComponentContainerPtr spawner = 
            ContentManager::get().createEmptyContainer();
    // spawner->attach(BufferedInputHandlerComponentPtr(
    //         new BufferedInputHandlerComponentTest(
    //                 InputEvent(KEY_PRESS | KEY_RELEASE))));
    spawner->attach(ComponentPtr(new TimerComponentTest()));

    // -------------------------------------------------------------------------

    ComponentContainerPtr checkerplane = 
            ContentManager::get().createStaticObject(
                    "checkers.mesh", 
                    Ogre::Vector3(0,-15,0), 
                    Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(-1,0,0)),
                    PS_Trimesh);

    checkerplane->attach(ComponentPtr(
            new GraphicsSkyComponent("CoAJnR/SkyBox/miramar")));

    // checkerplane->attach(ComponentPtr(
    //         new GraphicsSkyComponent("CoAJnR/SkyDome/Night", ST_Dome)));
    ContentManager::get().addToGroup(checkerplane, "protected");


    // -------------------------------------------------------------------------

    ComponentContainerPtr staticObject = 
            ContentManager::get().createStaticObject(
                    "bumpMapTest.mesh", Ogre::Vector3(0,-6,0));
//    staticObject->attach(GraphicsParticleComponentPtr(
//            new GraphicsParticleComponent("Examples/Smoke")));
    // staticObject->attach(ComponentPtr(
    //         new GraphicsLightComponent()));



    // staticObject->attach(ComponentPtr(new CollisionHandlerComponentTest));

    // -------------------------------------------------------------------------

    // TimeManager::get().wait(4);
    
    // for(int i = 0; i < 10; i++)
    //     ComponentContainerPtr obj = 
    //             ContentManager::get().createDynamicObject(
    //                     "Tube.mesh", Ogre::Vector3(-5 + i*1.9,10,0));
    
    // -------------------------------------------------------------------------

    // TimeManager::get().wait(7);

    // ContentManager::get().destroyObject(obj);
    // ContentManager::get().destroyObject(staticObject);

    // -------------------------------------------------------------------------
}

void
MainApplication::init()
{
    ThreadPool::get();
    UserThreadPool::get();
    
    TimeManager::get();
    IdManager::get();
    DataStorage::get();

    GraphicsManager::get();
    PhysicsManager::get();
    LogicsManager::get();
    
    InputManager::get();
    
    ContentManager::get();
}

#if !defined(__WIN32__) && !defined(__MINGW__)
void
signalHandler(int p_signal)
{
    MainApplication::shutdown();
}
#endif

void
MainApplication::go()
{
#if defined(__MAC__) || defined(__LINUX__)
    signal(SIGHUP,  signalHandler);
    signal(SIGINT,  signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGKILL, signalHandler);
#endif
    
    MainApplication::init();

	boost::thread graphicsThread(GraphicsManager::get().starter());
    boost::thread physicsThread(PhysicsManager::get().starter());
    boost::thread logicsThread(LogicsManager::get().starter());

    UserThreadPool::get().schedule(&createTestScene);
    
    graphicsThread.join();
    physicsThread.join();
    logicsThread.join();
    
    UserThreadPool::get().wait(0);
    ThreadPool::get().wait(0);
}

void
MainApplication::shutdown()
{
    std::cout << "shutting down..." << std::endl;

    ContentManager::get().removeAllGroups();

    /// @todo TODO: groups removal may not be done when 
    /// killing pools and managers

    LogicsManager::get().stop();
    PhysicsManager::get().stop();
    GraphicsManager::get().stop();
}

}
