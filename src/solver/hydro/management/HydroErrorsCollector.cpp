#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <yuni/core/logs.h>

#include "antares/antares/fatal-error.h"

namespace Antares
{

void HydroErrorsCollector::CheckForErrors() const
{
    if (!areasErrorMap_.empty())
    {
        for (const auto& [area_name, area_msg]: areasErrorMap_)
        {
            logs.error() << "In Area " << area_name << area_msg.message;
        }
        throw FatalError("Hydro validation has failed !");
    }
}

HydroErrorsCollector::AreaReference::AreaReference(HydroErrorsCollector* collector,
                                                   const std::string& name):
    areaSingleErrorMessage_(
      collector->areasErrorMap_
        .insert(

          {name,
           {.message = "", .message_number = (unsigned int)collector->areasErrorMap_.count(name)}})
        ->second)
{
}

HydroErrorsCollector::AreaReference HydroErrorsCollector::operator()(const std::string& name)
{
    return AreaReference(this, name);
}

} // namespace Antares
