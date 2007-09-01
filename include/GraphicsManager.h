
#ifndef COABLASTER_GRAPHICSMANAGER_INCLUDED
#define COABLASTER_GRAPHICSMANAGER_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{
    
class Scenery;
class Character;

/**
 * this class is rensponsable for ogre configuration and runtime
 */
class GraphicsManager
{
    /// singleton pattern
    static GraphicsManager* m_instance;
    
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
    
    Ogre::Viewport* m_viewport;

    /// the path to the resources, mainly to handle .app bundles on mac os x
    std::string m_resourcePath;

    /// the current scenery
    Scenery* m_scenery;

    /// list of characters to make movement corrections
    std::vector<Character*> m_characters;

    /**
     * setup renderer resources, like textures etc
     */
    void setupResources();

    /**
     * registers common frame listeners.
     */
    void registerFrameListeners();


public:
    /**
     * get access to the singleton instance.
     */
    static GraphicsManager* get()
    {
        if(!m_instance)
            m_instance = new GraphicsManager;

        assert(m_instance);
        return m_instance;
    }
    
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
    bool init(Scenery* scenery = 0);
    
    /**
     * updates the graphics manager (renders a frame)
     */
    bool update(double elapsed);
    
    /**
     * renders one frame and returns.
     */
    bool renderOneFrame();
    
    /**
     * sets the current scenery and deactivated the active one before.
     */
    void setScenery(Scenery* scenery);

    /**
     * adds a character to the list of objects which get movment corrections.
     */
    void addCharacter(Character* character);

    /**
     * removes a character from the graphics manager.
     */
    void removeCharacter(Character* character);

    /**
     * applies movement corrections for all registered characters.
     */
    void applyMovementCorrections(double p_elapsed);

    /**
     * returns the scenemanager.
     */
    Ogre::SceneManager* sceneManager() 
    { 
        assert(m_sceneManager); 
        return m_sceneManager; 
    }

    /**
     * returns the ogre root.
     */
    Ogre::Root* root()
    { 
        assert(m_root); 
        return m_root; 
    }

    /**
     * returns the main camera.
     */
    Ogre::Camera* camera()
    { 
        assert(m_camera); 
        return m_camera; 
    }
    
    /**
     * returns the render window.
     */
    Ogre::RenderWindow* window()
    { 
        assert(m_window); 
        return m_window; 
    }

    /**
     * return the view port.
     */
    Ogre::Viewport* viewport()
    {
        assert(m_viewport);
        return m_viewport;
    }
};

}



#endif
