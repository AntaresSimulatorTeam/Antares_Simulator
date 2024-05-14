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

#include <yuni/core/preprocessor/capabilities.h>

#include "antares/application/application.h"

#ifdef YUNI_OS_WINDOWS
#include <yuni/core/system/cpu.h>
#include <yuni/core/system/windows.hdr.h>

#include <antares/logs/logs.h>

using namespace Yuni;
using namespace Antares;
#endif

namespace Antares
{
namespace Solver
{
void Application::resetProcessPriority() const
{
#ifdef YUNI_OS_WINDOWS
    if (System::CPU::Count() <= 2)
    {
        if (not SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS))
        {
            logs.info() << "  :: impossible to reset the process priority";
        }
    }
#endif
}
} // namespace Solver
} // namespace Antares
