/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#include <yuni/yuni.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/core/process/rename.h>
#include <yuni/core/system/suspend.h>
#include <stdarg.h>
#include <new>

#include "config.h"
#include <antares/study/study.h>
#include <antares/logs.h>
#include <antares/exception/LoadingError.hpp>
#include "application.h"
#include "../ui/common/winmain.hxx"

#include <time.h>
#include <antares/hostinfo.h>

#include <antares/resources/resources.h>
#include "../config.h"
#include <antares/emergency.h>
#include <antares/memory/memory.h>
#include <antares/sys/policy.h>
#include <antares/locale.h>
#include "misc/system-memory.h"

#ifdef YUNI_OS_WINDOWS
#include <conio.h>
#else
#include <unistd.h>
#endif

using namespace Antares;
using namespace Yuni;

#define SEP Yuni::IO::Separator

#define GPL_ANNOUNCEMENT \
    "Copyright 2007-2018 RTE  - Authors: The Antares_Simulator Team \n\
\n\
Antares_Simulator is free software : you can redistribute it and / or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
There are special exceptions to the terms and conditions of the\n\
license as they are applied to this software.View the full text of\n\
the exceptions in file COPYING.txt in the directory of a distribution\n\
of this software in source form.\n\
\n\
Antares_Simulator is distributed in the hope that it will be useful, \n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with Antares_Simulator.If not, see <http://www.gnu.org/licenses/>.\n\
\n\n"

static void NotEnoughMemory()
{
    logs.fatal() << "Not enough memory. aborting.";
    exit(42);
}

/*!
** \brief main
*/
int main(int argc, char** argv)
{
    logs.info(GPL_ANNOUNCEMENT);
    // Name of the running application for the logger
    logs.applicationName("solver");

    // Dealing with the lack of memory
    std::set_new_handler(&NotEnoughMemory);

    // Antares SWAP
    if (not memory.initialize())
        return EXIT_FAILURE;

    // locale
    InitializeDefaultLocale();

    // Getting real UTF8 arguments
    argv = AntaresGetUTF8Arguments(argc, argv);

    // Disabling the log notice about disk space reservation
    Antares::Memory::InformAboutDiskSpaceReservation = false;

    // TODO It would be nice if it were removed...
    // This jump is only required by the internal solver
    CompteRendu.AnomalieDetectee = NON_ANTARES;
    setjmp(CompteRendu.Env);
    if (CompteRendu.AnomalieDetectee == OUI_ANTARES)
    {
        logs.error() << "Error...";
        AntaresSolverEmergencyShutdown(); // will never return
        return 42;
    }

    int ret = EXIT_FAILURE;

    Antares::Solver::Application application;
    try
    {
        ret = application.prepare(argc, argv);
    }
    // Catch errors
    catch (const Error::LoadingError& e)
    {
        logs.error() << e.what();
        AntaresSolverEmergencyShutdown();
    }
    // User asks for version
    if (ret != 0)
    {
        FreeUTF8Arguments(argc, argv);
        return EXIT_SUCCESS;
    }

    ret = application.execute();

    FreeUTF8Arguments(argc, argv);

    // to avoid a bug from wxExecute, we should wait a little before returning
    SuspendMilliSeconds(200 /*ms*/);

    return ret;
}
