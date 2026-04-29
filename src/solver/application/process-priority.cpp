// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/core/preprocessor/capabilities.h>

#include "antares/application/application.h"

#ifdef YUNI_OS_WINDOWS
#include <yuni/core/system/cpu.h>
#include <yuni/core/system/windows.hdr.h>

#include <antares/logs/logs.h>

using namespace Yuni;
using namespace Antares;
#endif

namespace Antares::Solver
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
} // namespace Antares::Solver
