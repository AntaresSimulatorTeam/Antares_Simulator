#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <yuni/core/logs.h>

AreaErrors::AreaErrors(const std::string& name):
    name_(name)
{
}

void AreaErrors::PrintErrors() const
{
    if (!messages_.empty())
    {
        logs.error() << "In Area " << name_;
        for (const auto& msg: messages)
        {
            logs.error() << msg;
        }
    }
}

bool HydroErrorsCollector::ReadyToFlush() const
{
    return flush_;
}

bool HydroErrorsCollector::ExceptionHasToBeThrown() const
{
    return !error_counter_per_area_.empty();
}

