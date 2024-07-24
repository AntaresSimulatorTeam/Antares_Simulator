#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <ranges>
#include <set>

#include <yuni/core/logs.h>

#include "antares/antares/fatal-error.h"

namespace Antares
{

void HydroErrorsCollector::CheckForErrors() const
{
    if (!areasErrorMap_.empty())
    {
        for (const auto& [key, values]: areasErrorMap_)
        {
            for (const auto& value: values | std::views::take(10))
            {
                logs.error() << "In Area " << key << ": " << value << " ";
            }
        }

        throw FatalError("Hydro validation has failed !");
    }
}

HydroErrorsCollector::AreaReference::AreaReference(HydroErrorsCollector* collector,
                                                   const std::string& name):
    areaSingleErrorMessage_(collector->CurrentMessage(name))
{
}

HydroErrorsCollector::AreaReference HydroErrorsCollector::operator()(const std::string& name)
{
    return AreaReference(this, name);
}

std::string& HydroErrorsCollector::CurrentMessage(const std::string& name)
{
    auto& msgs = areasErrorMap_[name];
    return *msgs.insert(msgs.end(), "");
}
} // namespace Antares
