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


#ifndef COAJNR_MAINAPPLICATION_INCLUDED
#define COAJNR_MAINAPPLICATION_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

/**
 * central program entry point
 */
class MainApplication
{
    /// window message queue lock
    static boost::mutex m_windowMessageQueueMutex;

    /**
     * stuff done before running.
     */
    static void init();

public:
    /**
     * makes the application run.
     */
    static void go();

    /**
     * shutdown the application cleanly
     */
    static void shutdown();
    
    /**
     * returns a reference to the window message queue lock.
     *
     * this lock is needed to synchronize the access to the window managers
     * message queue for the application window. 
     * GraphicsManager and InputManager both access this queue.
     */
    static boost::mutex& windowMessageQueueMutex()
    {
        return m_windowMessageQueueMutex;
    }

};

}

#endif

