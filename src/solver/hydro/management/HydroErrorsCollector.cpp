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
        std::set<std::string> uniqueKeys;
        std::transform(areasErrorMap_.begin(),
                       areasErrorMap_.end(),
                       std::inserter(uniqueKeys, uniqueKeys.begin()),
                       [](const auto& pair) { return pair.first; });

        for (const auto& key: uniqueKeys)
        {
            for (const auto& value:
                 areasErrorMap_
                   | std::views::filter([&key](const auto& p) { return p.first == key; })
                   | std::views::take(10))
            {
                logs.error() << "In Area " << value.first << ": " << value.second << " ";
            }
        }

        // for (auto value = areasErrorMap_.begin(); value != areasErrorMap_.end();)
        // {
        //     const auto& key = value->first;
        //     const auto& rangeEnd = areasErrorMap_.upper_bound(key);

        //     const auto& limit = std::min(value + 10, rangeEnd);

        //     for (; value != limit; ++value)
        //     {
        //         logs.error() << "In Area " << value.first << ": " << value.second << " ";
        //     }

        //     // Move iterator to the next key
        //     value = rangeEnd;
        // }

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
