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

#pragma once

#include "yuni/core/fwd.h"

#include <antares/array/matrix.h>
#include <antares/inifile/inifile.h>
#include <antares/study/version.h>

namespace Antares::Data
{

class AreaList;

class EnvForLoading final
{
public:
    explicit EnvForLoading(AreaList& l, const StudyVersion& v):
        areaList(l),
        version(v)
    {
    }

    //! INI file
    std::filesystem::path iniFilename;
    //! Current section
    IniFile::Section* section;

    Yuni::Clob buffer;
    Matrix<>::BufferType matrixBuffer;
    Yuni::Clob folder;

    //! List of areas
    AreaList& areaList;

    StudyVersion version;
};

} // namespace Antares::Data
