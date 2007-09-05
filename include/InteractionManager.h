
#ifndef COAJNR_INTERACTIONMANAGER_INCLUDED
#define COAJNR_INTERACTIONMANAGER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class InputHandler;
class Character;

class InteractionManager
{
    static InteractionManager* m_instance;
    
    InputHandler* m_inputHandler;

    /// list of characters to make movement corrections on
    std::vector<Character*> m_characters;

public:
    /**
     * get access to the singleton instance.
     */
    static InteractionManager* get()
    {
        if(!m_instance)
            m_instance = new InteractionManager;

        assert(m_instance);
        return m_instance;
    }
    
    InteractionManager();

    virtual ~InteractionManager();

    InputHandler* inputHandler()
    {
        assert(m_inputHandler);
        return m_inputHandler;
    }

    bool update();

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

};
    
}

#endif
