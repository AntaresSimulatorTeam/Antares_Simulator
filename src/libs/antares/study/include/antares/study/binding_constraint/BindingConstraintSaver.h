/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

//
// Created by marechaljas on 16/05/23.
//

#pragma once

#include <antares/inifile/inifile.h>
#include "antares/study/fwd.h"

namespace Antares::Data
{
class BindingConstraintSaver
{
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
    static bool saveToEnv(EnvForSaving& env, const BindingConstraint* bindingConstraint);
};
} // namespace Antares::Data
