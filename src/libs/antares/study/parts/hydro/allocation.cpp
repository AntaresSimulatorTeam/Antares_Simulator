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

#include "antares/study/parts/hydro/allocation.h"
#include <antares/study/area/capacityReservation.h>
#include <antares/utils/utils.h>
#include "antares/study/study.h"
#include <yuni/core/math.h>
#include <yuni/io/file.h>

using namespace Yuni;

namespace Antares
{
namespace Data
{
static const Area* FindMappedAreaName(const AreaName& id,
                                      const Study& study,
                                      const AreaNameMapping& mapping)
{
    auto i = mapping.find(id);
    if (i != mapping.end())
        return study.areas.findFromName(i->second);
    return study.areas.findFromName(id);
}

HydroAllocation::HydroAllocation()
{
#ifndef NDEBUG
    pMustUseValuesFromAreaID = false;
#endif
}

HydroAllocation::~HydroAllocation()
{
}

void HydroAllocation::remove(const AreaName& areaid)
{
    assert(!pMustUseValuesFromAreaID);
    auto i = pValues.find(areaid);
    if (i != pValues.end())
        pValues.erase(i);
}

void HydroAllocation::rename(const AreaName& oldid, const AreaName& newid)
{
    assert(!pMustUseValuesFromAreaID);
    auto i = pValues.find(oldid);
    if (i != pValues.end())
    {
        double coeff = i->second;
        pValues.erase(i);
        pValues[newid] = coeff;
    }
}

double HydroAllocation::fromArea(const Area& area) const
{
    return fromArea(area.id);
}

double HydroAllocation::fromArea(const Area* area) const
{
    return area ? fromArea(area->id) : 0.;
}

double HydroAllocation::operator[](const AreaName& areaid) const
{
    assert(!pMustUseValuesFromAreaID);
    auto i = pValues.find(areaid);
    return (i != pValues.end()) ? i->second : 0.;
}

double HydroAllocation::operator[](const Area& area) const
{
    assert(!pMustUseValuesFromAreaID);
    auto i = pValues.find(area.id);
    return (i != pValues.end()) ? i->second : 0.;
}

void HydroAllocation::fromArea(const Area& area, double value)
{
    fromArea(area.id, value);
}

void HydroAllocation::fromArea(const Area* area, double value)
{
    if (area)
        fromArea(area->id, value);
}

double HydroAllocation::fromArea(const AreaName& areaid) const
{
    assert(!pMustUseValuesFromAreaID);
    auto i = pValues.find(areaid);
    return (i != pValues.end()) ? i->second : 0.;
}

void HydroAllocation::fromArea(const AreaName& areaid, double value)
{
    assert(!pMustUseValuesFromAreaID);
    if (Utils::isZero(value))
    {
        auto i = pValues.find(areaid);
        if (i != pValues.end())
            pValues.erase(i);
    }
    else
        pValues[areaid] = value;
}

void HydroAllocation::prepareForSolver(const AreaList& list)
{
    pValuesFromAreaID.clear();
    auto end = pValues.end();
    for (auto i = pValues.begin(); i != end; ++i)
    {
        auto* targetarea = list.find(i->first);
        if (targetarea)
            pValuesFromAreaID[targetarea->index] = i->second;
    }

    pValues.clear();
#ifndef NDEBUG
    pMustUseValuesFromAreaID = true;
#endif
}

void HydroAllocation::clear()
{
    pValues.clear();
    pValuesFromAreaID.clear();
#ifndef NDEBUG
    pMustUseValuesFromAreaID = false;
#endif
}

bool HydroAllocation::loadFromFile(const AreaName& referencearea, const AnyString& filename)
{
    clear();

    IniFile ini;
    if (IO::File::Exists(filename) && ini.open(filename))
    {
        if (!ini.empty())
        {
            AreaName areaname;
            ini.each([&](const IniFile::Section& section) {
                for (auto* p = section.firstProperty; p; p = p->next)
                {
                    double coeff = p->value.to<double>();
                    if (!Utils::isZero(coeff))
                    {
                        areaname = p->key;
                        areaname.toLower();
                        pValues[areaname] = coeff;
                    }
                }
            });
        }
    }
    else
    {
        pValues[referencearea] = 1.0;
    }
    return true;
}

bool HydroAllocation::saveToFile(const AnyString& filename) const
{
    if (pValues.empty())
    {
        IO::File::CreateEmptyFile(filename);
        return true;
    }
    else
    {
        IniFile ini;
        auto* s = ini.addSection("[allocation]");
        auto end = pValues.end();
        CString<64, false> str;
        for (auto i = pValues.begin(); i != end; ++i)
        {
            double v = i->second;
            if (!Utils::isZero(v))
            {
                str = v;
                if (str.contains('.'))
                {
                    str.trimRight('0');
                    str.trimRight('.');
                }
                s->add(i->first, str);
            }
        }
        return ini.save(filename);
    }
}

void HydroAllocation::copyFrom(const HydroAllocation& source,
                               const Study& studySource,
                               const AreaNameMapping& mapping,
                               const Study& study)
{
    // clear all coefficients first
    clear();

    // copying from source
    auto end = source.coefficients().end();
    for (auto i = source.coefficients().begin(); i != end; ++i)
    {
        auto& areaid = i->first;
        double value = i->second;

        auto* areaCoeffSource = studySource.areas.findFromName(areaid);
        if (areaCoeffSource)
        {
            auto* targetarea = FindMappedAreaName(areaCoeffSource->name, study, mapping);
            if (targetarea)
            {
                pValues[targetarea->id] = value;
                pValuesFromAreaID[targetarea->index] = value;
            }
        }
    }
}

const HydroAllocation::Coefficients& HydroAllocation::coefficients() const
{
    return pValues;
}

} // namespace Data
} // namespace Antares
