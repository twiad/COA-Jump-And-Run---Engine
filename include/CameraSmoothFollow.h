
#ifndef COABLASTER_CAMERASMOOTHFOLLOW_INCLUDED
#define COABLASTER_CAMERASMOOTHFOLLOW_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

class CameraSmoothFollow : public Ogre::FrameListener
{
    Ogre::Camera* m_camera;
    Ogre::SceneNode* m_target;

public:
    CameraSmoothFollow(Ogre::Camera* camera, Ogre::SceneNode* target);
    virtual ~CameraSmoothFollow();

    bool frameStarted(const Ogre::FrameEvent& event);
};

}

#endif
