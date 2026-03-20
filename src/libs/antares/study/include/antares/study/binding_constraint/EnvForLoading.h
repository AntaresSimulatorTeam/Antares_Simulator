// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

    EnvForLoading(const EnvForLoading&) = default;
    EnvForLoading& operator=(const EnvForLoading&) = default;
    EnvForLoading(EnvForLoading&&) noexcept = default;
    EnvForLoading& operator=(EnvForLoading&&) noexcept = default;

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
