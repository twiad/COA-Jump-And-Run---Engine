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


#ifndef COAJNR_DEPENDENCIES_INCLUDED
#define COAJNR_DEPENDENCIES_INCLUDED

#include <vector>
#include <map>
#include <list>

#include <iostream>
#include <string>

#if !defined(__WIN32__) && !defined(__MINGW__)
#include <signal.h>
#endif


#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/timer.hpp>

#include <threadpool.hpp>

#ifdef __WIN32__
#include <Ogre.h>
#else
#include <OGRE/Ogre.h>
#endif

#include <OIS/OIS.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#ifdef __WIN32__
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  include <windows.h>
#  include <mmsystem.h>
#endif

#ifndef uint
# define uint unsigned int
#endif

#include "Config.h"

#ifdef COAJNR_UNITTEST
#  include <tut/tut.hpp>
#  include <tut/tut_reporter.hpp>
#endif

#endif
