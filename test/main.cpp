/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

Copyright (c) 2007 tuan.kuranes@gmail.com



This source file is not LGPL, it's public source code that you can reuse.
-----------------------------------------------------------------------------*/

#include "OgreBulletApplication.h"

#include "Primitives_Demo.h"
#include "Constraints_Demo.h"
#include "Vehicle_Demo.h"
#include "Trimesh_Demo.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>

// This function will locate the path to our application on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
std::string macBundlePath()
{
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return std::string(path);
}
#endif

#ifdef __APPLE__
  #undef main
#endif

// Windows stuff
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

/*
Windows entry point
*/
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT nCmdShow)
#else
int main(int argc,char* argv[])
#endif

{
    std::vector<OgreBulletListener *> ogrebulletscenes;
    std::vector <OgreBulletListener *>::iterator it;

    ogrebulletscenes.push_back(new Primitives_Demo());
    ogrebulletscenes.push_back(new Trimesh_Demo());
    ogrebulletscenes.push_back(new Vehicles_Demo());

    //ogrebulletscenes.push_back(new Constraints_Demo());

    ogrebulletscenes[0]->setNextKey(KC_F1);
    ogrebulletscenes[1]->setNextKey(KC_F2);
    ogrebulletscenes[2]->setNextKey(KC_F3);

    //ogrebulletscenes[3]->setNextKey(KC_F4);
    //ogrebulletscenes[4]->setNextKey(KC_F5);
    

    // Create the application and try to run it
    OgreBulletApplication app(&ogrebulletscenes);

#if (OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))
    SET_TERM_HANDLER;
#endif 

    try
    {
        app.go();
    } 
    catch(Ogre::Exception& e)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( 0, e.getFullDescription().c_str(), "OgreBullet OgreBulletListener: Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
    }

    it =  ogrebulletscenes.begin();
    while (it != ogrebulletscenes.end())
    {
        delete *it;
        ++it;
    }

    return 0;
}

