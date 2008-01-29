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


#include "LogicsManager.h"

#include "InputManager.h"
#include "BufferedInputHandlerComponentTest.h"
#include "UnbufferedInputHandlerComponentTest.h"
#include "TimeManager.h"

namespace CoAJnR
{

LogicsManager::LogicsManager()
{
    setMaxFrameTime(COAJNR_LOGICS_FRAME_TIME);
}

LogicsManager::~LogicsManager()
{

}

void
LogicsManager::init()
{
    IdManager::get().setThreadName("Logics Thread");
}

void
LogicsManager::fastUpdate()
{
    /// @todo TODO: not needed, but should work :)
    // InputManager::get().update();

    TimeManager::get().update();
}

bool
LogicsManager::update(TimeValue p_elapsed)
{
    InputManager::get().update();

    return true;
}

void
LogicsManager::release()
{

}

}

