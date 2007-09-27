
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

};
    
}

#endif
