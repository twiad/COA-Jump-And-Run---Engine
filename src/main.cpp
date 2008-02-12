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


#include "Dependencies.h"
#include "MainApplication.h"
#include "TimeManager.h"

using namespace CoAJnR;

#ifdef COAJNR_UNITTEST
namespace tut
{
    test_runner_singleton runner;
}
#endif

int main(int argc, char** argv)
{
#ifdef COAJNR_UNITTEST
    tut::reporter reporter;
    tut::runner.get().set_callback(&reporter);
    tut::runner.get().run_tests();

    if(reporter.warnings_count)
        TimeManager::get().wait(5.0f);

    if(!reporter.all_ok())
        return !reporter.all_ok();
#endif

    MainApplication::go();
    return 0;
}
