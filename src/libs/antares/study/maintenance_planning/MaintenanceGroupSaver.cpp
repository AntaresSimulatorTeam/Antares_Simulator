//
// Created by milos on 1/11/23.
//
#include "MaintenanceGroupSaver.h"
#include "MaintenanceGroup.h"
#include <yuni/yuni.h>
#include <string>
#include "antares/study/area/area.h"
#include "antares/study/fwd.h"

namespace Antares::Data
{

using namespace Yuni;
bool MaintenanceGroupSaver::saveToEnv(EnvForSaving& env, const MaintenanceGroup* maintenanceGroup)
{
    env.section->add("name", maintenanceGroup->name_);
    env.section->add("id", maintenanceGroup->ID_);
    env.section->add("enabled", maintenanceGroup->enabled_);
    env.section->add(
      "residual-load-definition",
      MaintenanceGroup::ResidualLoadDefinitionTypeToCString(maintenanceGroup->type_));

    if (!maintenanceGroup->weights_.empty())
    {
        auto end = maintenanceGroup->weights_.end();
        for (auto i = maintenanceGroup->weights_.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid area");
            String value;
            const Area& area = *(i->first);

            // add weight-load+renewable+ror
            env.key.clear() << area.name << '.weights';
            value << i->second.load << '%' << i->second.renewable << '%' << i->second.ror;
            env.section->add(env.key, value);
        }
    }
    return true;
}
} // namespace Antares::Data