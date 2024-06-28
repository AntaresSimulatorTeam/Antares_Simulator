#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <yuni/core/logs.h>

#include "antares/antares/fatal-error.h"

namespace Antares
{
void HydroErrorsCollector::Collect(const std::string& area_name, const std::string& message)
{
    logs.error() << "In Area " << area_name << " " << message;
    area_errors_counter_[area_name]++;
    errors_limit_reached_ = area_errors_counter_[area] > 10;
    stop_ = true;

    if (errors_limit_reached_)
    {
        throw FatalError("Hydro validation has failed !")
    }
}

void HydroErrorsCollector::Collect(const std::string& message)
{
    logs.error() << message;
    stop_ = true;
}

void HydroErrorsCollector::CheckForFatalErrors() const
{
    if (stop_ || errors_limit_reached_)
    {
        throw FatalError("Hydro validation has failed !")
    }
}

} // namespace Antares
