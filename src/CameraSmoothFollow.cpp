#include "CameraSmoothFollow.h"

namespace CoABlaster
{

CameraSmoothFollow::CameraSmoothFollow(
        Ogre::Camera* p_camera, Ogre::SceneNode* p_target)
{
    assert(p_camera && "camera must not be null");
    assert(p_target && "target must not be null");
    
    m_camera = p_camera;
    m_target = p_target;
    
    m_camera->setAutoTracking(true, m_target);
}

CameraSmoothFollow::~CameraSmoothFollow()
{
    
}

bool 
CameraSmoothFollow::frameStarted(const Ogre::FrameEvent& p_event)
{
    Ogre::Vector3 cameraPos = m_camera->getPosition();
    cameraPos.y -= 3;
    Ogre::Vector3 targetPos = m_target->getPosition();

    m_camera->setPosition(
            cameraPos.x + ((targetPos.x - cameraPos.x) * 0.6 * 
            p_event.timeSinceLastFrame),
            cameraPos.y + ((targetPos.y - cameraPos.y) * 0.6 * 
            p_event.timeSinceLastFrame) + 3,
            cameraPos.z);

    return true;
}

    
}
