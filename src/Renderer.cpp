
#include "Renderer.h"

#include "InputHandler.h"

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


Renderer::Renderer()
{
    m_initialized = false;
    m_root = 0;
    m_camera = 0;
    m_sceneManager = 0;
    m_window = 0;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
 m_resourcePath = macBundlePath() + "/Contents/Resources/";
#else
 m_resourcePath = "";
#endif

}

Renderer::~Renderer()
{
    if(m_initialized)
        if(m_root)
            delete m_root;
}

void
Renderer::setupResources()
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
Renderer::init()
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
            
    m_camera = m_sceneManager->createCamera("cam");
    m_camera->setPosition(Vector3(0, 0, 500));
    m_camera->lookAt(Vector3(0, 0, -300));
    m_camera->setNearClipDistance(5);

    Viewport* vp = m_window->addViewport(m_camera);
    vp->setBackgroundColour(ColourValue(0,0,0));

    m_camera->setAspectRatio(
            Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

    TextureManager::getSingleton().setDefaultNumMipmaps(5);

    m_root->addFrameListener(new InputHandler(m_window));

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_initialized = true;

    return true;
}

bool 
Renderer::renderOneFrame()
{
    if(!m_initialized)
        return false;
        
    m_root->renderOneFrame();
        
    return true;
}

bool 
Renderer::startRendering()
{
    if(!m_initialized)
        return false;

    m_root->startRendering();

    return true;
}

}