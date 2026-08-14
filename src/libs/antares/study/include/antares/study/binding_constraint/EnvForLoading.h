// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

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
    EnvForLoading& operator=(const EnvForLoading&) = delete;
    EnvForLoading(EnvForLoading&&) noexcept = delete;
    EnvForLoading& operator=(EnvForLoading&&) noexcept = delete;

    //! INI file
    std::filesystem::path iniFilename;
    //! Current section
    IniFile::Section* section{nullptr};

    std::string buffer;
    Matrix<>::BufferType matrixBuffer;
    std::string folder;

    //! List of areas
    AreaList& areaList;

    StudyVersion version;
};

} // namespace Antares::Data
