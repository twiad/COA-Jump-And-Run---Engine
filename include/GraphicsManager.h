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


#ifndef COAJNR_GRAPHICSMANAGER_INCLUDED
#define COAJNR_GRAPHICSMANAGER_INCLUDED

#include "Dependencies.h"

#include "FreeRunner.h"

namespace CoAJnR
{
    
class DebugOverlayFrameListener;

/**
 * handles visual representation of objects.
 *
 * this class encapsulates the ogre rendering subsystem and updates it 
 * continuously in its own thread.
 *
 * thats why do not access the GraphicsManager directly, use DeferredCall
 * objects instead. See DeferredCall, FreeRunner and FreeRunner::enqueueCallDroppable()
 * for more informations.
 */
class GraphicsManager : public FreeRunner, 
                        public rtils::Singleton<GraphicsManager>
{
    /**
     * handles window close events
     */
    class WindowCloseListener : public Ogre::WindowEventListener
    {
        virtual void windowClosed(Ogre::RenderWindow*);
    };
    
    WindowCloseListener m_windowEventListener;
    
    /// whether this instance was initialized
    bool m_initialized;

    /// the root of the ogre engine
    Ogre::Root* m_root;
    
    /// the scenemanager
    Ogre::SceneManager* m_sceneManager;

    /// the render window
    Ogre::RenderWindow* m_window;

    /// the part of the window to render in, connected to the camera    
    Ogre::Viewport* m_viewport;

    /// the path to the resources, mainly to handle .app bundles on mac os x
    std::string m_resourcePath;

    /// a little window showing statistics and a ogre logo
    DebugOverlayFrameListener* m_debugOverlayFrameListener;

    /**
     * sets up ogre resources.
     *
     * this loads and parses script for materials, particle systems etc.
     * and scans for available data like meshes, textures and so on.
     */
    void setupResources();

    /**
     * registers common listeners.
     */
    void registerListeners();

    /**
     * renders one frame and returns.
     */
    bool renderOneFrame();

    void initInputSystem();
    void releaseInputSystem();
    
protected:
    /**
     * brings up the ogre subsystem.
     * 
     * including window creation and viewport setup.
     */
    void init();
    
    /**
     * pops window messages like input events and similar.
     */
    void fastUpdate();
    
    /**
     * updates the graphics manager (renders one frame)
     */
    bool update(double elapsed);
    
    /**
     * cleanup.
     */
    void release();

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
     * returns the scenemanager.
     *
     * @note never use the scene manager directly from outside graphics thread.
     */
    Ogre::SceneManager* sceneManager() 
    { 
        assert(m_sceneManager); 
        return m_sceneManager; 
    }

    /**
     * returns the ogre root.
     *
     * @note never use the root directly from outside graphics thread.
     */
    Ogre::Root* root()
    { 
        assert(m_root); 
        return m_root; 
    }

    /**
     * returns the render window.
     *
     * @note never use the window directly from outside graphics thread.
     */
    Ogre::RenderWindow* window()
    { 
        assert(m_window); 
        return m_window; 
    }

    /**
     * return the view port.
     *
     * @note never use the viewport directly from outside graphics thread.
     */
    Ogre::Viewport* viewport()
    {
        assert(m_viewport);
        return m_viewport;
    }

    /**
     * returns the debug overlay
     *
     * @note never use the ogre objects directly from outside graphics thread.
     */
    DebugOverlayFrameListener* debugOverlay()
    {
        assert(m_debugOverlayFrameListener);
        return m_debugOverlayFrameListener;
    }
};

}

#endif

