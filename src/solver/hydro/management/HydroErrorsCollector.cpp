#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <yuni/core/logs.h>

#include "antares/antares/fatal-error.h"

namespace Antares
{

void HydroErrorsCollector::CheckForErrors() const
{
    if (!aresErrorMap_.empty())
    {
        for (const auto& [area_name, area_msgs]: areasErrorMap_)
        {
            logs.error() << "In Area " << area_name;
            for (const auto& msg: area_msgs)
            {
                logs.error() << msg;
            }
        }
        throw FatalError("Hydro validation has failed !");
    }
}

HydroErrorsCollector::AreaReference::AreaReference(HydroErrorsCollector* collector,
                                                   const std::string& name):
    areasErrorMap_(collector->areasErrorMap_[name])
{
}

HydroErrorsCollector::AreaReference HydroErrorsCollector::operator()(const std::string& name)
{
    return AreaReference(this, name);
}

void operator<<(const HydroErrorsCollector::AreaReference& ref, const std::string& msg)
{
    // TODO what to do with empty msg?
    ref.areasErrorMessages_.push_back(msg);
}
} // namespace Antares
