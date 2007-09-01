#include "InputHandler.h"

#include "InputController.h"
#include "GraphicsManager.h"

using namespace Ogre;
using namespace OIS;

namespace CoABlaster
{
    
InputHandler* InputHandler::m_instance = 0;
    
InputHandler::InputHandler()
{
	ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	GraphicsManager::get()->window()->getCustomAttribute("WINDOW", &windowHnd);
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
InputHandler::update()
{
    m_keyboard->capture();
	m_mouse->capture();
	
	if(m_joystick) 
	    m_joystick->capture();
	
	/// stop render loop on key Q and ESC
	if(m_keyboard->isKeyDown(KC_Q) || m_keyboard->isKeyDown(KC_ESCAPE))
        return false;
        
    /// @todo TODO: no time here
    std::vector<InputController*>::iterator it = m_inputControllers.begin();
    while(it != m_inputControllers.end())
        (*it++)->handleInput();
    
    return true;
}

void
InputHandler::addInputController(InputController* p_controller)
{
    assert(p_controller && "controller must not be null");
    m_inputControllers.push_back(p_controller);
}

void
InputHandler::removeInputController(InputController* p_controller)
{
    assert(p_controller && "controller must not be null");
    std::vector<InputController*>::iterator it;
    for(it = m_inputControllers.begin(); it != m_inputControllers.end(); it++)
        if((*it) == p_controller)
            m_inputControllers.erase(it);
} 

}