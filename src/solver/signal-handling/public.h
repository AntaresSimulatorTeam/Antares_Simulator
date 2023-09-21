#pragma once

#include <memory>

namespace Antares::Solver
{
void initializeSignalHandlers(std::weak_ptr<IResultWriter> writer);
}
