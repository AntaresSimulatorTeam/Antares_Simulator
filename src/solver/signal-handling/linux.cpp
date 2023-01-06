#include <yuni/core/preprocessor/capabilities.h>
#ifndef YUNI_OS_WINDOWS
#include <csignal>
#include "common.h"
#include "application/application.h"

namespace Antares::Solver
{
void initializeSignalHandlers()
{
    std::signal(SIGTERM, signalCtrl_term);
    std::signal(SIGINT, signalCtrl_int);
}
} // namespace Antares::Solver

#endif
