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
        * what happens when area name is changed - UI code  -- tested area name will be changed in INI file once study is saved - OK
        * what happens if the area is deleted - UI code - tested all mnt groups containing area name will be removed from INI file once study is saved 
            - OK?! or not OK! - maybe it is better to remove this area from mnt group? 
            This will actually happen if you try to load the mnt groups from ini file and the area from INI file do not exist in the model. 
            That area,with all weights, will be removed from INI file. Mnt Group-s will not be removed - just missing area will be removed from the mnt group.     


*/ 

