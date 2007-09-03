#include "InteractionManager.h"

#include "Character.h"
#include "InputHandler.h"
#include "MainApplication.h"
#include "PhysicsManager.h"

namespace CoAJnR
{

InteractionManager* InteractionManager::m_instance = 0;

InteractionManager::InteractionManager()
{
    m_inputHandler = InputHandler::get();
}

InteractionManager::~InteractionManager()
{

}

bool
InteractionManager::update()
{
    bool inputResult = m_inputHandler->update();

    PhysicsManager::get()->world()->publishCollisions();

    applyMovementCorrections();

    return inputResult;
}

void
InteractionManager::applyMovementCorrections()
{
    std::vector<Character*>::iterator it;
    for(it = m_characters.begin(); it != m_characters.end(); it++)
        (*it)->applyMovementCorrections();
}

void
InteractionManager::addCharacter(Character* p_character)
{
    assert(p_character && "character must not be null");
    m_characters.push_back(p_character);
}

void
InteractionManager::removeCharacter(Character* p_character)
{
    assert(p_character && "character must not be null");
    std::vector<Character*>::iterator it;
    for(it = m_characters.begin(); it != m_characters.end(); it++)
        if(*it == p_character)
            m_characters.erase(it);
}



}
