// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

namespace Antares::Data
{

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

bool PreproHydro::loadFromFolder(Study& s, const std::string& areaID, const fs::path& folder)
{
    enum
    {
        mtrxOption = Matrix<>::optFixedSize | Matrix<>::optImmediate,
    };

    constexpr int maxNbOfLineToLoad = 12;
    data.resize(hydroPreproMax, 12, true);

    fs::path preproPath = folder / areaID / "prepro.ini";
    bool ret = PreproHydroLoadSettings(this, preproPath);

    fs::path energyPath = folder / areaID / "energy.txt";
    ret = data.loadFromCSVFile(energyPath.string(),
                               hydroPreproMax,
                               maxNbOfLineToLoad,
                               mtrxOption,
                               &s.dataBuffer)
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

} // namespace Antares::Data
