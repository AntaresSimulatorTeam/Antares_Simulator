//
// Created by milos on 1/11/23.
//

#pragma once

#include <memory>
#include "antares/study/area/area.h"
#include "../binding_constraint/EnvForLoading.h"
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


/*
TODO CR27:
    for II phase:
        * check if same area is inside more than 1 Maintenance Group - and throw error
        * what happens when area name is changed - UI code
            * we need to change the name in INI file in Maintenance Group
        * what happens if the area is deleted - UI code
            * we need to remove that area from all Maintenance Group-s      


*/ 

