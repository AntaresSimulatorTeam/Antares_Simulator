#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <ranges>

#include <yuni/core/logs.h>

#include "antares/antares/fatal-error.h"

namespace Antares
{

void HydroErrorsCollector::CheckForErrors() const
{
    if (!areasErrorMap_.empty())
    {
        for (const auto& key: areasErrorMap_ | std::views::keys)
        {
            auto first_elements = areasErrorMap_
                                  | std::views::filter([&key](const auto& p)
                                                       { return p.first == key; })
                                  | std::views::take(10);

            for (const auto& value: first_elements)
            {
                logs.error() << "In Area " << value.first << ": " << value.second << " ";
            }
        }
        throw FatalError("Hydro validation has failed !");
    }
}

HydroErrorsCollector::AreaReference::AreaReference(HydroErrorsCollector* collector,
                                                   const std::string& name):
    areaSingleErrorMessage_(collector->areasErrorMap_.insert({name, ""})->second)
{
}

HydroErrorsCollector::AreaReference HydroErrorsCollector::operator()(const std::string& name)
{
    return AreaReference(this, name);
}

} // namespace Antares
