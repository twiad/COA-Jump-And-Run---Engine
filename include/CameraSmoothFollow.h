
#ifndef COAJNR_CAMERASMOOTHFOLLOW_INCLUDED
#define COAJNR_CAMERASMOOTHFOLLOW_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class CameraSmoothFollow : public Ogre::FrameListener
{
    Ogre::Camera* m_camera;
    Ogre::SceneNode* m_target;
    
    double m_verticalOffset;

public:
    CameraSmoothFollow(Ogre::Camera* camera, Ogre::SceneNode* target);
    virtual ~CameraSmoothFollow();

    bool frameStarted(const Ogre::FrameEvent& event);

    double verticalOffset()
    {
        return m_verticalOffset;
    }
    
    void setVerticalOffset(double offset)
    {
        m_verticalOffset = offset;
    }
};

}

#endif
