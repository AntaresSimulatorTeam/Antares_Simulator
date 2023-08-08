#pragma once

#include <memory>
#include <writer/i_writer.h>

namespace Antares::Solver
{
void initializeSignalHandlers(std::weak_ptr<IResultWriter> writer);
}
