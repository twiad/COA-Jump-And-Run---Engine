
#ifndef COABLASTER_GRAPHICSMANAGER_INCLUDED
#define COABLASTER_GRAPHICSMANAGER_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

/**
 * this class is rensponsable for ogre configuration and runtime
 */
class GraphicsManager
{
    /// whether the renderer is initialized
    bool m_initialized;

    /// the root of the ogre engine
    Ogre::Root* m_root;
    
    /// a default camera
    Ogre::Camera* m_camera;
    
    /// the scenemanager
    Ogre::SceneManager* m_sceneManager;

    /// the render window
    Ogre::RenderWindow* m_window;

    /// the path to the resources, mainly to handle .app bundles on mac os x
    std::string m_resourcePath;

    /// setup renderer resources, like textures etc
    void setupResources();

public:
    
    /**
     * ctor.
     */
    GraphicsManager();
    
    /**
     * dtor.
     */
    virtual ~GraphicsManager();
    
    /**
     * brings up the ogre subsystem.
     * 
     * including window creation and viewport setup.
     */
    bool init();
    
    /**
     * renders one frame and returns.
     */
    bool renderOneFrame();
    
    /**
     * starts the rendering loop.
     *
     * this functions returns as soon as the render loop gets interupted.
     */
    bool startRendering();
    
};

}



#endif
