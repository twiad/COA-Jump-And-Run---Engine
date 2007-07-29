
#ifndef COABLASTER_INPUTHANDLER_INCLUDED
#define COABLASTER_INPUTHANDLER_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

/**
 * checks for input between frames
 */
class InputHandler : public Ogre::FrameListener
{
    /// the ois input sub system
    OIS::InputManager* m_inputManager;
    
    /// the mouse input device
	OIS::Mouse* m_mouse;
	
	/// the keyboard input device
	OIS::Keyboard* m_keyboard;
	
	/// the joystick input device
	OIS::JoyStick* m_joystick;

public:
    InputHandler(Ogre::RenderWindow*);
    
    virtual ~InputHandler();
    
    bool frameStarted(const Ogre::FrameEvent& event);
};

}

#endif
