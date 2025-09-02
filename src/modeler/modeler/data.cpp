#include <antares/solver/modeler/data.h>

namespace Antares::Modeler
{
void Data::bindComponentsAndScenarios()
{
    for (auto& [id, component]: system->Components())
    {
        component.scenario_ = &scenario_group_repository.scenario(component.getScenarioGroupId());
    }
}
} // namespace Antares::Modeler
