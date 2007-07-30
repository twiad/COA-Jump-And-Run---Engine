/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

Copyright (c) 2007 tuan.kuranes@gmail.com



This source file is not LGPL, it's public source code that you can reuse.
-----------------------------------------------------------------------------*/
#include "OgreBulletApplication.h"
#include "OgreBulletListener.h"


#include "OgreResourceGroupManager.h"

using namespace Ogre;

#if !(OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))
    using namespace OIS;
#endif //OGRE_VERSION not Eihort

using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;
//using namespace OgreBulletLoader;

// -------------------------------------------------------------------------
OgreBulletApplication::OgreBulletApplication(std::vector <OgreBulletListener *> *bulletListeners) : 
    ExampleApplication(),
    FrameListener(),
    mInputSystem(0),
    mInput(0),
    mBulletListener(0)
{
    mBulletListeners = bulletListeners;
    assert (!mBulletListeners->empty());
}
// -------------------------------------------------------------------------
OgreBulletApplication::~OgreBulletApplication()
{ 
    if (mInputSystem || mInput)
    {
        #if (OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))
       
            delete mInputSystem;
        #else
           
            mInputSystem->destroyInputObject(mInput);
            mInputSystem->destroyInputObject(mMouse);
            InputManager::destroyInputSystem(mInputSystem);
        #endif //OGRE_VERSION not Eihort
    }
}
// -------------------------------------------------------------------------
bool OgreBulletApplication::switchListener(OgreBulletListener *newListener)
{

#if (OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))

    if (mBulletListener)
    {
        mInputSystem->removeMouseMotionListener(mBulletListener->getInputListener());
        mInputSystem->removeMouseListener(mBulletListener->getInputListener());
        mInputSystem->removeKeyListener(mBulletListener->getInputListener());
        mBulletListener->shutdown ();
    }

    newListener->init (mRoot, mWindow);

    mInputSystem->addMouseMotionListener(newListener->getInputListener());
    mInputSystem->addMouseListener(newListener->getInputListener());   
    mInputSystem->addKeyListener(newListener->getInputListener());             

#else

    if (mBulletListener)
    {
        mInput->setEventCallback (0);
        mMouse->setEventCallback (0);
        mBulletListener->shutdown ();
    }

    newListener->init (mRoot, mWindow, this);
    mInput->setEventCallback (newListener->getInputListener());
    mMouse->setEventCallback (newListener->getInputListener());

#endif //OGRE_VERSION not Eihort

    mBulletListener = newListener;

    return true;
}
// -------------------------------------------------------------------------
bool OgreBulletApplication::frameStarted(const FrameEvent& evt)
{

#if !(OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))
        mMouse->capture();
        mInput->capture();
#else
    mInput->capture();
#endif 

    std::vector <OgreBulletListener *>::iterator it =  mBulletListeners->begin();
    while (it != mBulletListeners->end())
    {
        if ((*(*it)->getBoolActivator()) == true ||
            mInput->isKeyDown ((*it)->getNextKey ()))
        {
            //if ((*it) !=  mBulletListener)
            {
                switchListener(*it);
            }
            break;
        }
        ++it;
    }	

    assert (mBulletListener);

    if (!mBulletListener->frameStarted(evt.timeSinceLastFrame))
    {
        mBulletListener->shutdown ();
        return false;
    }
    return true;
}

// -------------------------------------------------------------------------
bool OgreBulletApplication::frameEnded(const FrameEvent& evt)
{
    assert (mBulletListener);
    // we're running a scene, tell it that a frame's started 
    ;

    if (!mBulletListener->frameEnded(evt.timeSinceLastFrame))
    {
        mBulletListener->shutdown ();
        return false;
    }
    return true; 
}

// -------------------------------------------------------------------------
void OgreBulletApplication::createFrameListener(void)
{
    mFrameListener = 0;

#if (OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))

    mInput = PlatformManager::getSingleton().createInputReader();
    //mInput->initialise(mWindow, false, false);

    mInputSystem = new EventProcessor();
    mInputSystem->initialise (mWindow);
    mInputSystem->startProcessingEvents();
    mInput = mInputSystem->getInputReader();


#else

    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
	OIS::ParamList pl;

    #if defined OIS_WIN32_PLATFORM
        mWindow->getCustomAttribute("WINDOW", &windowHnd);
    #elif defined OIS_LINUX_PLATFORM
        mWindow->getCustomAttribute( "GLXWINDOW", &windowHnd );
    #endif    

    // Fill parameter list
    windowHndStr << (unsigned int) windowHnd;
    pl.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );

    // Uncomment these two lines to allow users to switch keyboards via the language bar
    //paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND") ));
    //paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE") ));

    mInputSystem  = InputManager::createInputSystem( pl );

    //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
    mInput = static_cast<Keyboard*>(mInputSystem->createInputObject( OISKeyboard, true ));
    mMouse = static_cast<Mouse*>(mInputSystem->createInputObject( OISMouse, true ));

    unsigned int width, height, depth;
    int left, top;
    mWindow->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

#endif //OGRE_VERSION not Eihort

    switchListener (*(mBulletListeners->begin()));
    mRoot->addFrameListener(this);

}
// -------------------------------------------------------------------------
void OgreBulletApplication::setupResources(void)
{
    ExampleApplication::setupResources(); 
    ResourceGroupManager *rsm = ResourceGroupManager::getSingletonPtr();
    StringVector groups = rsm->getResourceGroups();        
    if (std::find(groups.begin(), groups.end(), String("OgreBullet")) == groups.end())
    {
        FileInfoListPtr finfo =  ResourceGroupManager::getSingleton().findResourceFileInfo (
            "Bootstrap", "axes.mesh");
        const bool isSDK =  (!finfo->empty()) && 
            StringUtil::startsWith (finfo->begin()->archive->getName(), "../../media/packs/ogrecore.zip", true);
       
        rsm->createResourceGroup("OgreBullet");

        if (isSDK)
        {
            rsm->addResourceLocation ("../../../ogrebullet/demos/Media","FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../ogrebullet/demos/Media/textures", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../ogrebullet/demos/Media/overlays", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../ogrebullet/demos/Media/materials", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../ogrebullet/demos/Media/models", "FileSystem", "OgreBullet");

			rsm->addResourceLocation ("../../../ogrebullet/demos/Media/gui", "FileSystem", "BetaGui");
        }
        else
        {
            rsm->addResourceLocation ("../../../../../ogreaddons/ogrebullet/demos/Media", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../../../ogreaddons/ogrebullet/demos/Media/textures", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../../../ogreaddons/ogrebullet/demos/Media/overlays", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../../../ogreaddons/ogrebullet/demos/Media/materials", "FileSystem", "OgreBullet");
            rsm->addResourceLocation ("../../../../../ogreaddons/ogrebullet/demos/Media/models", "FileSystem", "OgreBullet");

            rsm->addResourceLocation ("../../../../../ogreaddons/ogrebullet/demos/Media/gui", "FileSystem", "BetaGui");
        }
    }
}
// -------------------------------------------------------------------------