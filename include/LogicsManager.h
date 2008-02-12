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


#ifndef COAJNR_LOGICSMANAGER_INCLUDED
#define COAJNR_LOGICSMANAGER_INCLUDED

#include "Dependencies.h"

#include "FreeRunner.h"

namespace CoAJnR
{

class InputManager;
class TimeManager;

/**
 * base of all logics
 */
class LogicsManager : public FreeRunner,
                      public rtils::Singleton<LogicsManager>
{

    InputManager& m_inputManager;
    TimeManager& m_timeManager;

public:
    /**
     * ctor.
     */
    LogicsManager();
         
    /**
     * dtor.
     */
    virtual ~LogicsManager();

protected:
    void init();

    void fastUpdate();

    bool update(TimeValue elapsed);
    
    void release();
};

}

#endif

