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


#include "GraphicsManager.h"

#include "DebugOverlayFrameListener.h"
#include "InputManager.h"
#include "MainApplication.h"

using namespace Ogre;

namespace CoAJnR
{

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>
/**
 * returns the path of the .app bundle on MacOS X
 *
 * this is needed to handle resource locations and config file pathes
 * propperly on Mac OS X.
 */
std::string macBundlePath()
{
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath(
            mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return std::string(path);
}
#endif

GraphicsManager::GraphicsManager()
{
    m_initialized = false;
    m_root = 0;
    m_sceneManager = 0;
    m_window = 0;
    m_debugOverlayFrameListener = 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
 m_resourcePath = macBundlePath() + "/Contents/Resources/";
#else
 m_resourcePath = "";
#endif

    setMaxFrameTime(COAJNR_GRAPHICS_FRAME_TIME);
}

GraphicsManager::~GraphicsManager()
{
}

void
GraphicsManager::setupResources()
{
    ConfigFile configFile;
    configFile.load(m_resourcePath + "resources.cfg");

    ConfigFile::SectionIterator secItr = configFile.getSectionIterator();

    String secName, typeName, archName;
    while (secItr.hasMoreElements())
    {
        secName = secItr.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = secItr.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), typeName, secName);
#else
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
#endif
        }
    }
}

void
GraphicsManager::init()
{
    IdManager::get().setThreadName("Graphics Thread");

    m_root = new Root(
            m_resourcePath + "plugins.cfg", 
            m_resourcePath + "ogre.cfg", 
            m_resourcePath + "ogre.log"
            );

    setupResources();

    if(!m_root->restoreConfig())
        m_root->showConfigDialog();
    
    m_window = m_root->initialise(true, "CoA JnR");

    assert(m_window && "window not created propperly");

    m_sceneManager = m_root->createSceneManager(ST_GENERIC);
    //m_sceneManager = m_root->createSceneManager(ST_EXTERIOR_CLOSE);
    //m_sceneManager = m_root->createSceneManager("TerrainSceneManager");

    m_viewport = m_window->addViewport(0);
    m_viewport->setBackgroundColour(ColourValue( 0, 0, 0));

    TextureManager::getSingleton().setDefaultNumMipmaps(5);

    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_debugOverlayFrameListener = new DebugOverlayFrameListener(m_window);

    m_initialized = true;

    
    m_sceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
    // m_sceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
    // m_sceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
    
    m_sceneManager->setAmbientLight(ColourValue(0.1,0.1,0.1));

    registerListeners();
    initInputSystem();
}

void
GraphicsManager::initInputSystem()
{
    size_t windowHnd = 0;
    m_window->getCustomAttribute("WINDOW", &windowHnd);
    InputManager::get().init(
            windowHnd, 
            m_window->getWidth(), 
            m_window->getHeight());
}

void
GraphicsManager::releaseInputSystem()
{
    InputManager::get().release();
}

void
GraphicsManager::registerListeners()
{
    m_root->addFrameListener(m_debugOverlayFrameListener);
    Ogre::WindowEventUtilities::addWindowEventListener(
            m_window, &m_windowEventListener);
}

void
GraphicsManager::fastUpdate()
{
    boost::mutex::scoped_lock scopedAutoLock(
            MainApplication::windowMessageQueueMutex());

    Ogre::WindowEventUtilities::messagePump();
}

bool
GraphicsManager::update(double p_elapsed)
{
    return renderOneFrame();
}

void
GraphicsManager::release()
{
    if(m_initialized)
    {
        releaseInputSystem();
        
        m_root->removeFrameListener(m_debugOverlayFrameListener);
        
        delete m_debugOverlayFrameListener;
        m_debugOverlayFrameListener = 0;
            
        if(m_root)
            delete m_root;
    }
}

bool 
GraphicsManager::renderOneFrame()
{
    if(!m_initialized)
        return false;

    return m_root->renderOneFrame();
}

void
GraphicsManager::WindowCloseListener::windowClosed(Ogre::RenderWindow*)
{
    MainApplication::shutdown();
}

}
