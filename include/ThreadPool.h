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


#ifndef COAJNR_THREADPOOL_INCLUDED
#define COAJNR_THREADPOOL_INCLUDED

#include "Dependencies.h"

namespace CoAJnR
{

/**
 * globally available thread pool for delegating tasks.
 * 
 * the size of the thread pool is defined by COAJNR_THREAD_POOL_SIZE.
 */
class ThreadPool : public boost::threadpool::pool,
                   public rtils::Singleton<ThreadPool>
{

public:
    /**
     * ctor.
     */
    ThreadPool() : boost::threadpool::pool(COAJNR_THREAD_POOL_SIZE) { };

    /**
     * dtor.
     */
    virtual ~ThreadPool() { };
};

}

#endif

