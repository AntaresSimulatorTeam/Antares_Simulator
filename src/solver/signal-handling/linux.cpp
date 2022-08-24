#ifndef YUNI_OS_WINDOWS
#include <csignal>
#include "common.h"
#include "../application.h"

namespace Antares
{
namespace Solver
{
void Application::installSignalHandlers() const
{
    std::signal(SIGTERM, signalCtrl_term);
    std::signal(SIGINT, signalCtrl_int);
}
} // namespace Solver
} // namespace Antares
#endif
