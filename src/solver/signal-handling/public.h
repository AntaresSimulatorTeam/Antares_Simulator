#pragma once

#include <memory>
#include <antares/study.h>

namespace Antares::Solver
{
void initializeSignalHandlers(std::weak_ptr<Antares::Data::Study> study);
}
