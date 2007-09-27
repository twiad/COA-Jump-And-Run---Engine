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

    PhysicsManager::get()->scheduleMovementCorrections();

    // applyMovementCorrections();

    return inputResult;
}

}
