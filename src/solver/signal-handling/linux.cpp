#ifndef YUNI_OS_WINDOWS
#include <csignal>

#include "common.h"
#include "public.h"

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
