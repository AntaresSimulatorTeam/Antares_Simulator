//
// Created by milos on 1/11/23.
//

#pragma once

#include <memory>
#include "antares/study/area/area.h"
#include "EnvForLoading.h"
#include "MaintenanceGroup.h"

namespace Antares::Data
{

class MaintenanceGroup;
class MaintenanceGroupLoader
{
public:
    std::vector<std::shared_ptr<MaintenanceGroup>> load(EnvForLoading env);

private:
    static bool SeparateValue(const EnvForLoading& env,
                              const IniFile::Property* p,
                              MaintenanceGroup::Weights& w);

    std::string toLower(const std::string& str);
};

} // namespace Antares::Data
