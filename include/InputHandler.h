
#ifndef COABLASTER_INPUTHANDLER_INCLUDED
#define COABLASTER_INPUTHANDLER_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

class InputController;

/**
 * checks for input between frames
 */
class InputHandler
{
    static InputHandler* m_instance;
    
    /// the ois input sub system
    OIS::InputManager* m_inputManager;
    
    /// the mouse input device
	OIS::Mouse* m_mouse;
	
	/// the keyboard input device
	OIS::Keyboard* m_keyboard;
	
	/// the joystick input device
	OIS::JoyStick* m_joystick;

    /// input controllers
    std::vector<InputController*> m_inputControllers;
    
public:
    InputHandler();
    
    virtual ~InputHandler();
    
    static InputHandler* get()
    {
        if(!m_instance)
            m_instance = new InputHandler();
        
        return m_instance;
    }
    
    bool update();

    void addInputController(InputController* controller);

    void removeInputController(InputController* controller);    

    OIS::Keyboard* keyboard() 
    { 
        assert(m_keyboard);
        return m_keyboard; 
    };

    OIS::Mouse* mouse() 
    { 
        assert(m_mouse);
        return m_mouse; 
    };

    OIS::JoyStick* joystick() 
    { 
        assert(m_joystick);
        return m_joystick; 
    };
};

}

#endif
