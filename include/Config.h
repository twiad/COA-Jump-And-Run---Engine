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


#ifndef COAJNR_CONFIG_INCLUDED
#define COAJNR_CONFIG_INCLUDED

/// define this to allow more than 16384 physical objects
/// disable it to get a speedup in physics calculation
// #define COAJNR_LOADS_OF_OBJECTS

/// the max amount of threads used in this application 
/// if the actual amount of threads used in this application will exeed this 
/// value, undefined behaviour will occure.
#define COAJNR_MAX_THREAD_COUNT             12

/// sets the size of the applications thread pool
#define COAJNR_THREAD_POOL_SIZE             2
#define COAJNR_USER_THREAD_POOL_SIZE        6

/// sets the length of queues used by FreeRunner's to queue DeferredCall's
#define COAJNR_CALL_QUEUE_SIZE              4096

/// default FreeRunner target frame time.
#define COAJNR_FREERUNNER_FRAME_TIME        0.02

/// graphics manager target frame time
// #define COAJNR_GRAPHICS_FRAME_TIME          0.04
// #define COAJNR_GRAPHICS_FRAME_TIME          0.03333333
#define COAJNR_GRAPHICS_FRAME_TIME          0.0166666667

/// physics manager target frame time
// #define COAJNR_PHYSICS_FRAME_TIME           0.04
// #define COAJNR_PHYSICS_FRAME_TIME           0.03333333
#define COAJNR_PHYSICS_FRAME_TIME           0.0166666667

/// logics manager target frame rate
// #define COAJNR_LOGICS_FRAME_TIME            0.04
// #define COAJNR_LOGICS_FRAME_TIME            0.03333333
#define COAJNR_LOGICS_FRAME_TIME           0.0166666667

/// calls are dropped after this execution count
#define COAJNR_CALL_MAX_EXECUTION_COUNT     ((uint)0) - 1

#define COAJNR_PHYSICS_ACCELERATION_FACTOR      1.6

#endif

