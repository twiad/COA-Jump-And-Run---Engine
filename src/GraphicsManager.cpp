
#include "GraphicsManager.h"

#include "InputHandler.h"
// #include "PhysicsManager.h"
#include "Scenery.h"

using namespace Ogre;

namespace CoABlaster
{

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>
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

GraphicsManager* GraphicsManager::m_instance = 0;

GraphicsManager::GraphicsManager()
{
    m_initialized = false;
    m_root = 0;
    m_camera = 0;
    m_sceneManager = 0;
    m_scenery = 0;
    m_window = 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
 m_resourcePath = macBundlePath() + "/Contents/Resources/";
#else
 m_resourcePath = "";
#endif

}

GraphicsManager::~GraphicsManager()
{
    if(m_initialized)
    {
        if(m_scenery)
            delete m_scenery;
        if(m_root)
            delete m_root;
    }
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

bool 
GraphicsManager::init(Scenery* p_scenery)
{
    m_root = new Root(
            m_resourcePath + "plugins.cfg", 
            m_resourcePath + "ogre.cfg", 
            m_resourcePath + "Ogre.log");

    setupResources();

    if(m_root->showConfigDialog())
          m_window = m_root->initialise(true);

    if(!m_window)
        return false;

    m_sceneManager = m_root->
            createSceneManager(ST_GENERIC, "ExampleSMInstance");
            
    m_camera = m_sceneManager->createCamera("camera");
    m_camera->setPosition(Vector3(0, 0, -50));
    m_camera->lookAt(Vector3(0, 0, 0));
    m_camera->setNearClipDistance(1);

    m_viewport = m_window->addViewport(m_camera);
    m_viewport->setBackgroundColour(ColourValue( 0, 0, 0));

    m_camera->setAspectRatio(
            Real(m_viewport->getActualWidth()) /
            Real(m_viewport->getActualHeight()));

    TextureManager::getSingleton().setDefaultNumMipmaps(5);

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_initialized = true;

    registerFrameListeners();
    
    m_sceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    if(p_scenery)
        setScenery(p_scenery);

    return true;
}

void
GraphicsManager::registerFrameListeners()
{
    m_root->addFrameListener(new InputHandler(m_window));
    // m_root->addFrameListener(PhysicsManager::get());
}

bool
GraphicsManager::update(double p_elapsed)
{
    WindowEventUtilities::messagePump();
    return renderOneFrame();
}

bool 
GraphicsManager::renderOneFrame()
{
    if(!m_initialized)
        return false;

    return m_root->renderOneFrame();
}

bool 
GraphicsManager::startRendering()
{
    if(!m_initialized)
        return false;

    m_root->startRendering();

    return true;
}

void
GraphicsManager::setScenery(Scenery* p_scenery)
{
    assert(p_scenery);

    if(m_scenery)
    {
        m_scenery->cleanup();
        delete m_scenery;
    }

    m_scenery = p_scenery;
    m_scenery->setup();
}

}