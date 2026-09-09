// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/hydro/allocation.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/utils/utils.h>
#include "antares/study/study.h"

namespace fs = std::filesystem;

namespace Antares::Data
{
void HydroAllocation::fromArea(const AreaName& areaid, double value)
{
    if (Utils::isZero(value))
    {
        auto i = pValues.find(areaid);
        if (i != pValues.end())
        {
            pValues.erase(i);
        }
    }
    else
    {
        pValues[areaid] = value;
    }
}

void HydroAllocation::prepareForSolver(const AreaList& list)
{
    pValuesFromAreaID.clear();
    auto end = pValues.end();
    for (auto i = pValues.begin(); i != end; ++i)
    {
        auto* targetarea = list.find(i->first);
        if (targetarea)
        {
            pValuesFromAreaID[targetarea->index] = i->second;
        }
    }

    pValues.clear();
}

void HydroAllocation::clear()
{
    pValues.clear();
    pValuesFromAreaID.clear();
}

bool HydroAllocation::loadFromFile(const AreaName& referencearea, const fs::path& filename)
{
    clear();

    IniFile ini;
    if (!fs::exists(filename) || !ini.open(filename))
    {
        pValues[referencearea] = 1.0;
        return true;
    }

    if (ini.empty())
    {
        return true;
    }

    ini.each(
      [this](const IniFile::Section& section)
      {
          for (auto* p = section.firstProperty; p; p = p->next)
          {
              double coeff = p->value.to<double>();
              if (!Utils::isZero(coeff))
              {
                  AreaName areaname = std::string(p->key);
                  boost::to_lower(areaname);
                  pValues[areaname] = coeff;
              }
          }
      });
    return true;
}

const HydroAllocation::Coefficients& HydroAllocation::coefficients() const
{
    return pValues;
}

} // namespace Antares::Data
