#pragma once

#include <stdexcept>

namespace Antares::Solver::Initialization::Error
{
class NoQueueService : public std::runtime_error
{
public:
    NoQueueService(): std::runtime_error("No queue service") {}
};
} // namespace Antares::Solver::Initialization::Error
