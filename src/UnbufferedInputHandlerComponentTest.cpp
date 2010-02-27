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


#include "UnbufferedInputHandlerComponentTest.h"

#include "ContentManager.h"
#include "GraphicsParticleComponent.h"
#include "TimeManager.h"
#include "CharacterControl3DComponent.h"
#include "PhysicsConstraintsComponent.h"
#include "GraphicsLightComponent.h"
#include "GraphicsCameraComponent.h"
#include "PhysicsManager.h"
#include "GraphicsAnimationController.h"

namespace CoAJnR
{
    
unsigned int g_tubes = 0;

UnbufferedInputHandlerComponentTest::UnbufferedInputHandlerComponentTest()
{
}

void 
UnbufferedInputHandlerComponentTest::update(
        TimeValue timeSinceLastUpdate,
        OIS::Keyboard* keyboard,
        OIS::MouseState mouseState, 
        OIS::JoyStickState joystickState)
{
    if(keyboard->isKeyDown(OIS::KC_SPACE))
    {
        // if(m_obj)
        //     m_obj->detach("CharacterControl3D");
        
        
        m_obj = ContentManager::get().createDynamicObject(
                // "QuestionCube.mesh",
                "GlonMesh.mesh",
                Ogre::Vector3::ZERO,
                Ogre::Quaternion::IDENTITY,
                1.0f,
                PS_Sphere);
        PhysicsConstraintsComponent* constr = new PhysicsConstraintsComponent; 
        m_obj->attach(ComponentPtr(constr));
//        m_obj->attach(GraphicsParticleComponentPtr(
//                new GraphicsParticleComponent("Examples/Flare")), "p2");
        m_obj->attach(ComponentPtr(
                new CharacterControl3DComponent()));
        m_obj->attach(GraphicsLightComponentPtr(
                new GraphicsLightComponent(LT_Point, Ogre::Vector3(0,10,15))));
        m_obj->attach(ComponentPtr(
                new GraphicsCameraComponent(true, Ogre::Vector3(0,1.8,9))));
        m_obj->attach(ComponentPtr(new GraphicsAnimationController("Walk2")));
        
        
        // m_obj->component<GraphicsLightComponent>("Light");
                
        TimeManager::get().wait(0.5f);
    }
    else 
    { 
        if(m_obj && keyboard->isKeyDown(OIS::KC_W))
            boost::dynamic_pointer_cast<CharacterControl3DComponent>(
                    m_obj->component("CharacterControl3D"))->forward(
                            timeSinceLastUpdate);
        if(m_obj && keyboard->isKeyDown(OIS::KC_S))
            boost::dynamic_pointer_cast<CharacterControl3DComponent>(
                    m_obj->component("CharacterControl3D"))->backward(
                            timeSinceLastUpdate);
        if(m_obj && keyboard->isKeyDown(OIS::KC_A))
            boost::dynamic_pointer_cast<CharacterControl3DComponent>(
                    m_obj->component("CharacterControl3D"))->turnLeft(
                            timeSinceLastUpdate * 2.0f);
        if(m_obj && keyboard->isKeyDown(OIS::KC_D))
            boost::dynamic_pointer_cast<CharacterControl3DComponent>(
                    m_obj->component("CharacterControl3D"))->turnRight(
                            timeSinceLastUpdate * 2.0f);
        if(m_obj && keyboard->isKeyDown(OIS::KC_E))
            boost::dynamic_pointer_cast<CharacterControl3DComponent>(
                    m_obj->component("CharacterControl3D"))->jump(
                            timeSinceLastUpdate);
    }   
    
    if(keyboard->isKeyDown(OIS::KC_X))
    {
        std::cout << "X DOWN" << std::endl;
        ContentManager::get().removeGroup("Default");
    }
    
    if(keyboard->isKeyDown(OIS::KC_G))
    {
        PhysicsManager::get().setAccelerationFactor(2.0);
    }
    if(keyboard->isKeyDown(OIS::KC_H))
    {
        PhysicsManager::get().setAccelerationFactor(0.5);
    }
    
    else if(m_obj && keyboard->isKeyDown(OIS::KC_K))
    {
        boost::dynamic_pointer_cast<PhysicsComponent>(
                m_obj->component("PhysicsMain"))->applyTorqueImpulse(
                        Ogre::Vector3(0,10,0));
    }

    else if(m_obj && keyboard->isKeyDown(OIS::KC_J))
    {
        ContentManager::get().destroyObject(m_obj);
        m_obj = ComponentContainerPtr();
    }

    if(keyboard->isKeyDown(OIS::KC_T))
    {
        for(int i = 0; i < 10; i++)
            ContentManager::get().createDynamicObject(
                    "Tube.mesh", 
                    Ogre::Vector3(
                            rand()%20 - 10,
                            rand()%200,
                            rand()%20 - 10));
    
        g_tubes += 10;
        std::cout << "tubes: " << g_tubes << std::endl;
        TimeManager::get().wait(0.5);
    }


    if(keyboard->isKeyDown(OIS::KC_P))
    {
        ComponentContainerPtr pfx = 
                ContentManager::get().createEmptyContainer();
        pfx->attach(ComponentPtr(new PositionComponent(Ogre::Vector3(0,5,0))));
        pfx->attach(ComponentPtr(
                new PhysicsComponent(Ogre::Vector3(0.1,0,0), PS_Sphere, 1.0)));
        pfx->attach(GraphicsParticleComponentPtr(
                new GraphicsParticleComponent("Examples/Flare")));
        // pfx->attach(GraphicsLightComponentPtr(
        //         new GraphicsLightComponent(LT_Point)));
    }
}

std::string
UnbufferedInputHandlerComponentTest::defaultName() const
{
    return std::string("UnbufferedInputHandlerComponentTest");
}

}
