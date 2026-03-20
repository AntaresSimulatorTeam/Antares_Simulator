// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifdef YUNI_OS_WINDOWS

#include <windows.h>

#include "antares/signal-handling/common.h"

BOOL WINAPI ConsoleHandler(DWORD dwType)
{
    switch (dwType)
    {
    case CTRL_C_EVENT:
        signalCtrl_int(0);
        break;
    case CTRL_BREAK_EVENT:
        signalCtrl_term(0);
        break;
    default:
        break;
    }
    return TRUE;
}

namespace Antares::Solver
{
void initializeSignalHandlers(std::weak_ptr<IResultWriter> writer)
{
    setApplicationResultWriter(writer);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
}
} // namespace Antares::Solver
#endif
