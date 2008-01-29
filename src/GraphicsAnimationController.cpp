/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2007  Adrian Jäkel                       *
 *                     Copyright (C) 2007  Franz Köhler                       *
 *                     Copyright (C) 2007  Robert Timm                        *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/

#include "GraphicsAnimationController.h"

namespace CoAJnR
{

    
GraphicsAnimationController::GraphicsAnimationController(
        std::string p_animationName) :
    TimerComponent(0.03), m_animationName(p_animationName)
{

}

void 
GraphicsAnimationController::attachedCallback()
{
    /// @todo TODO: this is considered an antipattern, 
    /// do not have to call overwritten method from base
    TimerComponent::attachedCallback();

    m_meshComponent = parent()->component<GraphicsMeshComponent>(
            "GraphicsMeshMain");

    DEFERRED_CALL_VARS(GetAnimationCall)
        std::string animationName;
        boost::shared_ptr<GraphicsAnimationController> component;
        GraphicsMeshComponentPtr graphicsComponent;

    DEFERRED_CALL_CODE()
        if(!graphicsComponent->ready())
            return false;

        Ogre::AnimationState* animState = 
                graphicsComponent->entity()->
                        getAnimationState(animationName);

        animState->setLoop(true);
        animState->setEnabled(true);

        std::cout << "anim state should be enabled" << std::endl;
        
        component->m_animationState = animState;


        /// @todo TODO: already called by super!!!!!!!!!!!!!!!!!!111one
        ThreadPool::get().schedule(boost::bind(
                &ComponentContainer::componentAttachedCallback,
                component->parent(), component));

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(GetAnimationCall)
        call->animationName = m_animationName;
        call->graphicsComponent = m_meshComponent;

        /// @todo TODO: only one per component container supported here. FIXME
        call->component = parent()->component<GraphicsAnimationController>(
            defaultName());

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void 
GraphicsAnimationController::detachedCallback()
{
    
    DEFERRED_CALL_VARS(GetAnimationCall)
        boost::shared_ptr<GraphicsAnimationController> component;
        GraphicsMeshComponentPtr graphicsComponent;

    DEFERRED_CALL_CODE()
        if(!graphicsComponent->ready())
            return false;

        Ogre::AnimationState* animState = component->m_animationState;

        animState->setEnabled(false);

        component->m_animationState = 0;

        /// @todo TODO: ???
        // delete animState;

        // ThreadPool::get().schedule(boost::bind(
        //         &ComponentContainer::componentDetachedCallback,
        //         component->parent(), component));

        component->TimerComponent::detachedCallback();

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(GetAnimationCall)
        call->graphicsComponent = m_meshComponent;
        /// @todo TODO: only one per component container supported here. FIXME
        call->component = boost::dynamic_pointer_cast<
                GraphicsAnimationController>(shared_from_this());
        
        std::cout << call->component << std::endl;
        
        // parent()->component<GraphicsAnimationController>(
        //     defaultName());

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

void 
GraphicsAnimationController::update(TimeValue p_time)
{
    DEFERRED_CALL_VARS(UpdateAnimationCall)
        boost::shared_ptr<GraphicsAnimationController> component;
        TimeValue time;

    DEFERRED_CALL_CODE()
        if(!component->m_meshComponent->ready())
            return true;

        component->m_animationState->addTime(time);

        return true;

    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(UpdateAnimationCall)
        call->time = p_time;
        /// @todo TODO: only one per component container supported here. FIXME
        call->component = parent()->component<GraphicsAnimationController>(
            defaultName());

    DEFERRED_CALL_ENQUEUE(GraphicsManager)
}

std::vector<std::string> 
GraphicsAnimationController::dependencies()
{
    std::vector<std::string> deps;
    deps.push_back("GraphicsMeshMain");
    return deps;
}

std::string 
GraphicsAnimationController::defaultName() const
{
    return std::string("GraphicsAnimationController");
}

}
