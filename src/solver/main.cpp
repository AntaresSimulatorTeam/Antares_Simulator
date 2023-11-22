/*
** Copyright 2007-2023 RTE
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
#include <yuni/core/system/suspend.h>

#include <antares/logs/logs.h>
#include "antares/application/application.h"
#include <antares/args/args_to_utf8.h>

#include <antares/fatal-error.h>
#include <antares/memory/memory.h>
#include <antares/locale/locale.h>

using namespace Antares;
using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace {

const char* const GPL_ANNOUNCEMENT
  = "Copyright 2007-2023 RTE  - Authors: The Antares_Simulator Team \n"
    "\n"
    "Antares_Simulator is free software : you can redistribute it and / or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "There are special exceptions to the terms and conditions of the\n"
    "license as they are applied to this software.View the full text of\n"
    "the exceptions in file COPYING.txt in the directory of a distribution\n"
    "of this software in source form.\n"
    "\n"
    "Antares_Simulator is distributed in the hope that it will be useful, \n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with Antares_Simulator.If not, see <http://www.gnu.org/licenses/>.\n"
    "\n";

const char* const ANTARES_LOGO = "\n\n"
                                 "                  . = - .                                   \n"
                                 "                  - % % :                                   \n"
                                 "              . . - % # :                   . .             \n"
                                 "            . - : - + : .         . . . . : + = .           \n"
                                 "          . = : . = - .     : = - - = = + * % % -           \n"
                                 "        . = -   . + :   . : * % * = - : : + # * .           \n"
                                 "        - * .   . + : : = + + * - .   . - + : .             \n"
                                 "      . * -     - # * + = : . . .     : + :                 \n"
                                 "      = # :   . # % % = .           . = = .                 \n"
                                 "    . * * .   : # % % :             - + .                   \n"
                                 "    : # * . : + = * + .           . = - .                   \n"
                                 "    : % + . = = . : + :           : + :                     \n"
                                 "    - % * = = .   . = = .       . - = .                     \n"
                                 "    - % # + :       : + = .     . = -             . . .     \n"
                                 "    - % % = .         : + = : . : + :           . - + :     \n"
                                 "    : % % -             . = + = # % = . . . . . : # @ + .   \n"
                                 "    . # % + .             . : * % % # = = = = + + * # -     \n"
                                 "      + % % -                 : # % + - = = - : . . . .     \n"
                                 "      : # % * .               . = + . . . . .               \n"
                                 "      . + % % + .             . = + .         . . .         \n"
                                 "        . * % % + .           . # % :       . . .           \n"
                                 "          : # % % * - .       . + # :     . : : .           \n"
                                 "            : * % % % * - : . . : + : . - = - .             \n"
                                 "              . = # % % % # * * * # # # + : .               \n"
                                 "                . : = * # % % % % # + - .                   \n"
                                 "                    . . : : : : : . .                       \n\n";

constexpr int ALLOCATION_FAILURE_EXIT_CODE = 42;

void logAbortion()
{
    if (!logs.logfile())
    {
        logs.fatal() << "Aborting now. (warning: no file log available)";
        logs.warning() << "No log file available";
    }
    else
    {
        logs.error() << "Aborting now. See logs for more details";
    }
}

}

/*!
** \brief main
*/
int main(int argc, char** argv)
{
    try {

        logs.info(ANTARES_LOGO);
        logs.info(GPL_ANNOUNCEMENT);
        // Name of the running application for the logger
        logs.applicationName("solver");

        if (not memory.initializeTemporaryFolder())
            throw FatalError("Could not initialize temporary folder");

        // locale
        InitializeDefaultLocale();

        // Getting real UTF8 arguments
        IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
        std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
        Antares::Solver::Application application;
        application.prepare(argc, argv);
        application.execute();
        application.writeExectutionInfo();

        // to avoid a bug from wxExecute, we should wait a little before returning
        SuspendMilliSeconds(200 /*ms*/);

        return EXIT_SUCCESS;
    }
    catch (const std::bad_alloc& exc) {
        logs.fatal() << exc.what();
        logAbortion();
        return ALLOCATION_FAILURE_EXIT_CODE;
    }
    catch (const std::exception& exc) {
        logs.fatal() << exc.what();
        logAbortion();
        return EXIT_FAILURE;
    }
    catch (...) {
        logs.fatal() << "An unexpected error occurred.";
        logAbortion();
        return EXIT_FAILURE;
    }
}
