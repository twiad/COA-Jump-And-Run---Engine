
#ifndef COAJNR_PHYSICSMANAGER_INCLUDED
#define COAJNR_PHYSICSMANAGER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class Character;
class PhysicsInteractionPacket;

/**
 * this class is rensponsable for bullet configuration and runtime
 */
class PhysicsManager // : public Ogre::FrameListener
{
    /// singleton pattern
    static PhysicsManager* m_instance;
    
    OgreBulletDynamics::DynamicsWorld* m_world;

    Ogre::SceneNode* m_debugNode;

    bool m_doMovementCorrections;

    /// list of characters to make movement corrections on
    std::vector<Character*> m_characters;

public:
    /**
     * get access to the singleton instance.
     */
    static PhysicsManager* get()
    {
        if(!m_instance)
            m_instance = new PhysicsManager;

        assert(m_instance);
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
     * updates the physics manager, steps the world for a certain time amount.
     */
    void update(double elapesd);
    
    /**
     * synchronizes bullet transformations to ogre
     */
    void synchronize();
    
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
    
    /**
     * adds a character to the list of objects which get movment corrections.
     */
    void addCharacter(Character* character);

    /**
     * removes a character from the graphics manager.
     */
    void removeCharacter(Character* character);

    /**
     * applies movement corrections for all registered characters.
     */
    void applyMovementCorrections();


    void scheduleMovementCorrections();
};

}



#endif
