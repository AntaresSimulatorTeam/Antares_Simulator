/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <yuni/core/system/suspend.h>

#include <antares/antares/fatal-error.h>
#include <antares/args/args_to_utf8.h>
#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/memory/memory.h>
#include "antares/application/application.h"

using namespace Antares;
using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace
{

const char* const MPL_ANNOUNCEMENT
  = "Copyright 2007-2023 RTE  - Authors: The Antares_Simulator Team \n"
    "\n"
    "Antares_Simulator is free software : you can redistribute it and / or modify\n"
    "it under the terms of the Mozilla Public Licence 2.0 as published by\n"
    "the Mozilla Foundation, either version 2 of the License, or\n"
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
    "Mozilla Public Licence 2.0 for more details.\n"
    "\n"
    "You should have received a copy of the Mozilla Public Licence 2.0\n"
    "along with Antares_Simulator.If not, see <https://opensource.org/license/mpl-2-0/>.\n"
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

} // namespace

/*!
** \brief main
*/
int main(int argc, const char** argv)
{
    try{
                         logs.info(ANTARES_LOGO);
        logs.info(MPL_ANNOUNCEMENT);
        // Name of the running application for the logger
        logs.applicationName("solver");

        if (not memory.initializeTemporaryFolder())
        {
            throw FatalError("Could not initialize temporary folder");
        }

        // locale
        InitializeDefaultLocale();

        // Getting real UTF8 arguments
        IntoUTF8ArgsTranslator toUTF8ArgsTranslator(argc, argv);
        std::tie(argc, argv) = toUTF8ArgsTranslator.convert();
        Antares::Solver::Application application;
        application.prepare(argc, argv);
        application.execute();
        




application.writeExectutionInfo();

        return EXIT_SUCCESS;
    }
    catch (const std::bad_alloc& exc)
    {
        logs.fatal() << exc.what();
        logAbortion();
        return ALLOCATION_FAILURE_EXIT_CODE;
    }
    catch (const std::exception& exc)
    {
        logs.fatal() << exc.what();
        logAbortion();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        logs.fatal() << "An unexpected error occurred.";
        logAbortion();return EXIT_FAILURE;
    }
}
