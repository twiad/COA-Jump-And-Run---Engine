#include "InputHandler.h"

using namespace Ogre;
using namespace OIS;

namespace CoABlaster
{
    
InputHandler::InputHandler(RenderWindow* p_window)
{
	ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	p_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	m_inputManager = InputManager::createInputSystem( pl );

	m_keyboard = static_cast<Keyboard*>(
	        m_inputManager->createInputObject(OISKeyboard, false));
	m_mouse = static_cast<Mouse*>(
	        m_inputManager->createInputObject(OISMouse, false));

	try {
		m_joystick = static_cast<JoyStick*>(
		        m_inputManager->createInputObject(OISJoyStick, false));
	}
	catch(...) {
		m_joystick = 0;
	}

}

InputHandler::~InputHandler()
{
    
}

bool
InputHandler::frameStarted(const Ogre::FrameEvent& p_event)
{
    m_keyboard->capture();
	m_mouse->capture();
	
	if(m_joystick) 
	    m_joystick->capture();
	
	/// stop render loop on key Q and ESC
	if(m_keyboard->isKeyDown(KC_Q) || m_keyboard->isKeyDown(KC_ESCAPE))
        return false;
    
    return true;
}
 
}