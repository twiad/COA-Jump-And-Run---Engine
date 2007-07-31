
#ifndef COABLASTER_PHYSICSMANAGER_INCLUDED
#define COABLASTER_PHYSICSMANAGER_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

/**
 * this class is rensponsable for bullet configuration and runtime
 */
class PhysicsManager : public Ogre::FrameListener
{
    /// singleton pattern
    static PhysicsManager* m_instance;
    
    OgreBulletDynamics::DynamicsWorld* m_world;

    Ogre::SceneNode* m_debugNode;


public:
    /**
     * get access to the singleton instance.
     */
    static PhysicsManager* get()
    {
        if(!m_instance)
            m_instance = new PhysicsManager;

        return m_instance;
    }
    
    /**
     * ctor.
     */
    PhysicsManager();
    
    /**
     * dtor.
     */
    virtual ~PhysicsManager();
    
    /**
     * framelistener callback for physics update.
     */
    bool frameStarted(const Ogre::FrameEvent& event);

    /**
     * returns the ogre bullet dynamics world.
     */
    OgreBulletDynamics::DynamicsWorld* world()
    {
        assert(m_world);
        return m_world;
    }
};

}



#endif
