#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <yuni/core/logs.h>

#include "antares/antares/fatal-error.h"

namespace Antares
{
void HydroErrorsCollector::Collect(const std::string& area_name, const std::string& message)
{
    logs.error() << "In Area " << area_name << " " << message;
    auto error_count = area_errors_counter_[area_name]++;
    errors_limit_reached_ = error_count > TRHESHOLD_NUMBER_OF_ERRORS_FOR_ONE_AREA;
    stop_ = true;

    if (errors_limit_reached_)
    {
        logs.error() << "Hydro validation has failed !";
        logs.error() << error_count << " errors found in Area " << area_name;
    }
}

void HydroErrorsCollector::Collect(const std::string& message)
{
    logs.error() << message;
    stop_ = true;
}

void HydroErrorsCollector::CheckForFatalErrors() const
{
    if (stop_)
    {
        throw FatalError("Hydro validation has failed !");
    }
}

} // namespace Antares
