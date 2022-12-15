#ifndef YUNI_OS_WINDOWS
#include <csignal>

#include "antares/signal-handling/common.h"
#include "antares/signal-handling/public.h"

namespace Antares::Solver
{
void initializeSignalHandlers(std::weak_ptr<IResultWriter> writer)
{
    setApplicationResultWriter(writer);
    std::signal(SIGTERM, &signalCtrl_term);
    std::signal(SIGINT, signalCtrl_int);
}

} // namespace Antares::Solver

#endif
