/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "allocation.h"
#include "../../study.h"
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
    if (Math::Zero(value))
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
                    if (!Math::Zero(coeff))
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
            if (not Math::Zero(v))
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
