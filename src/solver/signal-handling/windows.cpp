#ifdef YUNI_OS_WINDOWS

#include <windows.h>
#include "common.h"
#include "../application.h"

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
    }
    return TRUE;
}

namespace Antares
{
namespace Solver
{
void Application::installSignalHandlers() const
{
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
}
} // namespace Solver
} // namespace Antares
#endif
