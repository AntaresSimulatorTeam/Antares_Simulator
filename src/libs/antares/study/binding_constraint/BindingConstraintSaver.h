//
// Created by marechaljas on 16/05/23.
//

#pragma once

#include <inifile/inifile.h>
#include "antares/study/fwd.h"

namespace Antares::Data {
class BindingConstraintSaver {
public:
    class EnvForSaving final
    {
    public:
        EnvForSaving() = default;

        //! Current section
        IniFile::Section* section = nullptr;

        Yuni::Clob folder;
        Yuni::Clob matrixFilename;
        Yuni::CString<2 * (ant_k_area_name_max_length + 8), false> key;
    };

    /*!
    ** \brief Save the binding constraint into a folder and an INI file
    **
    ** \param env All information needed to perform the task
    ** \return True if the operation succeeded, false otherwise
    */
    static bool saveToEnv(EnvForSaving& env, const BindingConstraint *bindingConstraint) ;
};
}