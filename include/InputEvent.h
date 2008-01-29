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

#ifndef COAJNR_INPUTEVENT_INCLUDED
#define COAJNR_INPUTEVENT_INCLUDED

namespace CoAJnR 
{

/**
 * enumeration of input events that can occur, can get or'ed together
 */
enum InputEvent
{
    KEY_PRESS = 1,
    KEY_RELEASE = 2,
    MOUSE_MOVE = 4,
    MOUSE_PRESS = 8,
    MOUSE_RELEASE = 16,
    JOYSTICK_PRESS = 32,
    JOYSTICK_RELEASE = 64,
    JOYSTICK_AXIS_MOVE = 128,
    JOYSTICK_SLIDER_MOVE = 256,
    JOYSTICK_POV_MOVE = 512
};

}

#endif
