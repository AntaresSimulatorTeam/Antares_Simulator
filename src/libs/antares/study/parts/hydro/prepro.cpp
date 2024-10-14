/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/study/parts/hydro/prepro.h"

#include <yuni/yuni.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Antares;
using namespace Yuni;

namespace fs = std::filesystem;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
static bool PreproHydroSaveSettings(PreproHydro* h, const char* filename)
{
    IniFile ini;
    auto* s = ini.addSection("prepro");
    s->add("intermonthly-correlation", h->intermonthlyCorrelation);
    return ini.save(filename);
}

static bool PreproHydroLoadSettings(PreproHydro* h, const fs::path& filename)
{
    IniFile ini;
    IniFile::Section* s;
    IniFile::Property* p;

    // Reset
    h->intermonthlyCorrelation = 0.5f;

    // Try to load the file
    if (ini.open(filename))
    {
        bool ret = false;
        if ((s = ini.find("prepro")))
        {
            p = s->find("intermonthly-correlation");
            if (p || (p = s->find("intermonthlycorrelationvalue")))
            {
                const CString<20, false> t = p->value;
                ret = t.to(h->intermonthlyCorrelation);
            }
            else
            {
                logs.error() << filename
                             << ": prepro/intermonthlyCorrelation: The property is missing";
            }
        }
        else
        {
            logs.error() << filename << ": [prepro]: The section is missing";
        }

        return ret;
    }
    else
    {
        logs.error() << filename << ": Impossible to open the file";
    }

    return false;
}

PreproHydro::PreproHydro()
{
}

uint64_t PreproHydroMemoryUsage(PreproHydro* h)
{
    return (h) ? sizeof(double) : 0;
}

void PreproHydro::reset()
{
    intermonthlyCorrelation = 0.5;
    // data
    data.reset(hydroPreproMax, 12, true);
    // Expectation
    auto& col = data[expectation];
    for (uint i = 0; i != 12; ++i)
    {
        col[i] = 0.;
    }
}

void PreproHydro::copyFrom(const PreproHydro& rhs)
{
    intermonthlyCorrelation = rhs.intermonthlyCorrelation;
    data = rhs.data;
    rhs.data.unloadFromMemory();
}

bool PreproHydro::saveToFolder(const AreaName& areaID, const char* folder)
{
    assert(folder);
    assert('\0' != *folder);

    Clob buffer;
    // Make sure the folder is created
    buffer.clear() << folder << SEP << areaID;
    if (IO::Directory::Create(buffer))
    {
        // Writing the ini file
        buffer.clear() << folder << SEP << areaID << SEP << "prepro.ini";
        bool ret = (PreproHydroSaveSettings(this, buffer.c_str()) ? true : false);

        buffer.clear() << folder << SEP << areaID << SEP << "energy.txt";
        if (data.jit)
        {
            assert(0 != (data.jit->options & Matrix<>::optFixedSize));
            assert(data.jit->minWidth == hydroPreproMax);
        }

        ret = data.saveToCSVFile(buffer, /*decimal*/ 3) && ret;

        return ret;
    }
    return false;
}

bool PreproHydro::loadFromFolder(Study& s, const std::string& areaID, const fs::path& folder)
{
    enum
    {
        mtrxOption = Matrix<>::optFixedSize | Matrix<>::optImmediate,
    };

    constexpr int maxNbOfLineToLoad = 12;

    data.resize(hydroPreproMax, 12, true);
    String& buffer = s.bufferLoadingTS;

    fs::path preproPath = folder / areaID / "prepro.ini";
    bool ret = PreproHydroLoadSettings(this, preproPath);

    fs::path energyPath = folder / areaID / "energy.txt";
    ret = data.loadFromCSVFile(energyPath.string(), hydroPreproMax, maxNbOfLineToLoad, mtrxOption, &s.dataBuffer)
          && ret;

    return ret;
}

bool PreproHydro::validate(const std::string& areaID)
{
    bool ret = true;

    if (intermonthlyCorrelation < 0. || intermonthlyCorrelation > 1.)
    {
        logs.error() << "Hydro: Prepro: `" << areaID
                     << "`: Intermonthly correlation value: The value must be between 0 and 1";
        if (intermonthlyCorrelation < 0.)
        {
            intermonthlyCorrelation = 0.;
        }
        else
        {
            intermonthlyCorrelation = 1.;
        }
    }

    const auto& col = data[powerOverWater];
    for (unsigned i = 0; i != data.height; ++i)
    {
        const double d = col[i];
        if (d < 0. || d > 1.)
        {
            logs.error() << "Hydro: Prepro: " << areaID
                         << ": invalid value for ROR (line: " << (i + 1) << ")";
        }
    }

    const auto& colMin = data[minimumEnergy];
    const auto& colMax = data[maximumEnergy];

    for (unsigned i = 0; i != data.height; ++i)
    {
        if (colMin[i] < 0.)
        {
            ret = false;
            logs.error() << "Hydro: Prepro: `" << areaID
                         << "`: minimum energy: At least one value is negative (line: " << (i + 1)
                         << ')';
            continue;
        }
        if (colMin[i] > colMax[i])
        {
            ret = false;
            logs.error() << "Hydro: Prepro: `" << areaID
                         << "`: the minimum energy must be less than the maximum energy (line: "
                         << (i + 1) << ')';
        }
    }

    const auto& colExp = data[expectation];
    for (unsigned i = 0; i != data.height; i++)
    {
        if (colExp[i] < 0.)
        {
            ret = false;
            logs.error() << "Hydro: Prepro: `" << areaID
                         << "`: invalid value for expectation (line: " << (i + 1) << ")";
        }
    }

    const auto& colStdDev = data[stdDeviation];
    for (unsigned i = 0; i != data.height; i++)
    {
        if (colStdDev[i] < 0.)
        {
            ret = false;
            logs.error() << "Hydro: Prepro: `" << areaID
                         << "`: invalid value for standard deviation (line: " << (i + 1) << ")";
        }
    }

    return ret;
}

bool PreproHydro::forceReload(bool reload) const
{
    return data.forceReload(reload);
}

void PreproHydro::markAsModified() const
{
    return data.markAsModified();
}

} // namespace Data
} // namespace Antares
