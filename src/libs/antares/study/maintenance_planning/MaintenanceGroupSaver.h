//
// Created by milos on 1/11/23.
//

#pragma once

#include <antares/inifile/inifile.h>
#include "antares/study/fwd.h"
#include "MaintenanceGroup.h"

namespace Antares::Data
{
class MaintenanceGroupSaver
{
public:
    class EnvForSaving final
    {
    public:
        EnvForSaving() = default;

        //! Current section
        IniFile::Section* section = nullptr;
        Yuni::Clob folder;
        Yuni::CString<(ant_k_area_name_max_length + 15), false> key;
    };

    /*!
    ** \brief Save the Maintenance Group into a folder and an INI file
    **
    ** \param env All information needed to perform the task
    ** \return True if the operation succeeded, false otherwise
    */
    static bool saveToEnv(EnvForSaving& env, const MaintenanceGroup* maintenanceGroup);
};
} // namespace Antares::Data