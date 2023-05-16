/*
** Copyright 2007-2023 RTE
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
#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <algorithm>
#include <vector>
#include "BindingConstraint.h"
#include "../study.h"
#include "../../logs.h"
#include "../../utils.h"
#include "../runtime.h"
#include "../memory-usage.h"
#include "BindingConstraintTimeSeries.h"
#include "BindingConstraintTimeSeriesNumbers.h"
#include "BindingConstraintLoader.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

namespace Antares::Data {

BindingConstraint::Operator BindingConstraint::StringToOperator(const AnyString& text)
{
    ShortString16 l(text);
    l.toLower();

    if (l == "both" or l == "<>" or l == "><" or l == "< and >")
        return opBoth;
    if (l == "less" or l == "<" or l == "<=")
        return opLess;
    if (l == "greater" or l == ">" or l == ">=")
        return opGreater;
    if (l == "equal" or l == "=" or l == "==")
        return opEquality;
    return opUnknown;
}

BindingConstraint::Type BindingConstraint::StringToType(const AnyString& text)
{
    if (not text.empty())
    {
        ShortString16 l(text);
        l.toLower();
        switch (l.first())
        {
        case 'h':
        {
                if (l == "hourly" or l == "hour" or l == "h")
                    return typeHourly;
                break;
            }
        case 'd':
        {
                if (l == "daily" or l == "day" or l == "d")
                    return typeDaily;
                break;
            }
        case 'w':
        {
                if (l == "weekly" or l == "week" or l == "w")
                    return typeWeekly;
                break;
            }
        }
    }
    return typeUnknown;
}

const char* BindingConstraint::TypeToCString(const BindingConstraint::Type type)
{
    static const char *const names[typeMax + 1] = {"", "hourly", "daily", "weekly", ""};
    assert((uint) type < (uint) (typeMax + 1));
    return names[type];
}

const char* BindingConstraint::OperatorToCString(BindingConstraint::Operator o)
{
    static const char *const names[opMax + 1] = {"", "equal", "less", "greater", "both", ""};
    assert((uint) o < (uint) (opMax + 1));
    return names[o];
}

const char* BindingConstraint::OperatorToShortCString(BindingConstraint::Operator o)
{
    static const char *const names[opMax + 1]
            = {"", "equality", "bounded above", "bounded below", "bounded on both sides", ""};
    assert((uint) o < (uint) (opMax + 1));
    return names[o];
}

const char* BindingConstraint::MathOperatorToCString(BindingConstraint::Operator o)
{
    static const char *const names[opMax + 1] = {"", "=", "<", ">", "< and >", ""};
    assert((uint) o < (uint) (opMax + 1));
    return names[o];
}

BindingConstraint::BindingConstraint() : pEnabled(false)
{
}

BindingConstraint::~BindingConstraint()
{
#ifndef NDEBUG
    pName = "<INVALID>";
    pID = "<INVALID>";
#endif
}

void BindingConstraint::name(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    Antares::TransformNameIntoID(pName, pID);
}

void BindingConstraint::weight(const AreaLink* lnk, double w)
{
    if (lnk)
    {
        if (Math::Zero(w))
        {
            auto i = pLinkWeights.find(lnk);
            if (i != pLinkWeights.end())
                pLinkWeights.erase(i);
        }
        else {
            pLinkWeights[lnk] = w;
        }
    }
}

void BindingConstraint::weight(const ThermalCluster* clstr, double w)
{
    if (clstr)
    {
        if (Math::Zero(w))
        {
            auto i = pClusterWeights.find(clstr);
            if (i != pClusterWeights.end())
                pClusterWeights.erase(i);
        } else {
            pClusterWeights[clstr] = w;
        }
    }
}

void BindingConstraint::removeAllWeights()
{
    pLinkWeights.clear();
    pClusterWeights.clear();
}

void BindingConstraint::offset(const AreaLink* lnk, int o)
{
    if (lnk)
    {
        if (Math::Zero(o))
        {
            auto i = pLinkOffsets.find(lnk);
            if (i != pLinkOffsets.end())
                pLinkOffsets.erase(i);
        }
        else
            pLinkOffsets[lnk] = o;
    }
}

void BindingConstraint::offset(const ThermalCluster* clstr, int o)
{
    if (clstr)
    {
        if (Math::Zero(o))
        {
            auto i = pClusterOffsets.find(clstr);
            if (i != pClusterOffsets.end())
                pClusterOffsets.erase(i);
        }
        else
            pClusterOffsets[clstr] = o;
    }
}

void BindingConstraint::removeAllOffsets()
{
    pLinkOffsets.clear();
    pClusterOffsets.clear();
}

uint Antares::Data::BindingConstraint::enabledClusterCount() const
{
    return static_cast<uint>(std::count_if(
            pClusterWeights.begin(), pClusterWeights.end(), [](const clusterWeightMap::value_type &i) {
                return i.first->enabled && !i.first->mustrun;
            }));
}

bool BindingConstraint::removeLink(const AreaLink* lnk)
{
    auto iw = pLinkWeights.find(lnk);
    if (iw != pLinkWeights.end())
    {
        pLinkWeights.erase(iw);
        return true;
    }

    auto io = pLinkOffsets.find(lnk);
    if (io != pLinkOffsets.end())
    {
        pLinkOffsets.erase(io);
        return true;
    }
    return false;
}

bool BindingConstraint::removeCluster(const ThermalCluster* clstr)
{
    auto iw = pClusterWeights.find(clstr);
    if (iw != pClusterWeights.end())
    {
        pClusterWeights.erase(iw);
        return true;
    }

    auto io = pClusterOffsets.find(clstr);
    if (io != pClusterOffsets.end())
    {
        pClusterOffsets.erase(io);
        return true;
    }
    return false;
}

void BindingConstraint::resetToDefaultValues()
{
    pEnabled = true;
    pComments.clear();
    time_series.reset();
    markAsModified();
    //pValues.zero();
    //pValues.markAsModified();
}

void BindingConstraint::copyWeights(const Study &study,
                                    const BindingConstraint &rhs,
                                    bool emptyBefore)
{
    if (emptyBefore)
    {
        pLinkWeights.clear();
        pClusterWeights.clear();
    }

    if (not rhs.pLinkWeights.empty())
    {
        auto end = rhs.pLinkWeights.end();
        for (auto i = rhs.pLinkWeights.begin(); i != end; ++i)
        {
            // Alias to the current link
            const AreaLink *sourceLink = i->first;
            // weight
            const double weight = i->second;

            assert(sourceLink and "Invalid link in binding constraint");
            assert(sourceLink->from and "Invalid area pointer 'from' within link");
            assert(sourceLink->with and "Invalid area pointer 'with' within link");
            const AreaLink *localLink
                    = study.areas.findLink(sourceLink->from->id, sourceLink->with->id);
            if (localLink)
                pLinkWeights[localLink] = weight;
        }
    }

    if (not rhs.pClusterWeights.empty())
    {
        auto end = rhs.pClusterWeights.end();
        for (auto i = rhs.pClusterWeights.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster *thermalCluster = i->first;
            // weight
            const double weight = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            const Area *localParent = study.areas.findFromName(thermalCluster->parentArea->name);
            if (localParent)
            {
                const ThermalCluster *localTC
                        = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterWeights[localTC] = weight;
            }
        }
    }
}

void BindingConstraint::copyWeights(const Study &study,
                                    const BindingConstraint &rhs,
                                    bool emptyBefore,
                                    Yuni::Bind<void(AreaName&, const AreaName&)>& translate)
{
    if (emptyBefore)
    {
        pLinkWeights.clear();
        pClusterWeights.clear();
    }

    if (rhs.pLinkWeights.empty())
        return;

    AreaName fromID;
    AreaName withID;

    auto end = rhs.pLinkWeights.end();
    for (auto i = rhs.pLinkWeights.begin(); i != end; ++i)
    {
        // Alias to the current link
        const AreaLink *sourceLink = i->first;
        // weight
        const double weight = i->second;

        assert(sourceLink and "Invalid link in binding constraint");
        assert(sourceLink->from and "Invalid area pointer 'from' within link");
        assert(sourceLink->with and "Invalid area pointer 'with' within link");

        // Translatiob
        translate(fromID, sourceLink->from->id);
        translate(withID, sourceLink->with->id);

        const AreaLink *localLink = study.areas.findLink(fromID, withID);
        if (localLink)
            pLinkWeights[localLink] = weight;
    }

    if (not rhs.pClusterWeights.empty())
    {
        auto end = rhs.pClusterWeights.end();
        for (auto i = rhs.pClusterWeights.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster *thermalCluster = i->first;
            // weight
            const double weight = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            AreaName parentID;
            translate(parentID, thermalCluster->parentArea->id);

            const Area *localParent = study.areas.find(parentID);
            if (localParent)
            {
                const ThermalCluster *localTC
                        = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterWeights[localTC] = weight;
            }
        }
    }
}

void BindingConstraint::copyOffsets(const Study &study,
                                    const BindingConstraint &rhs,
                                    bool emptyBefore)
{
    if (emptyBefore)
    {
        pLinkOffsets.clear();
        pClusterOffsets.clear();
    }

    if (not rhs.pLinkOffsets.empty())
    {
        auto end = rhs.pLinkOffsets.end();
        for (auto i = rhs.pLinkOffsets.begin(); i != end; ++i)
        {
            // Alias to the current link
            const AreaLink *sourceLink = i->first;
            // offset
            const int offset = i->second;

            assert(sourceLink and "Invalid link in binding constraint");
            assert(sourceLink->from and "Invalid area pointer 'from' within link");
            assert(sourceLink->with and "Invalid area pointer 'with' within link");
            const AreaLink *localLink
                    = study.areas.findLink(sourceLink->from->id, sourceLink->with->id);
            if (localLink)
                pLinkOffsets[localLink] = offset;
        }
    }

    if (not rhs.pClusterOffsets.empty())
    {
        auto end = rhs.pClusterOffsets.end();
        for (auto i = rhs.pClusterOffsets.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster *thermalCluster = i->first;
            // weight
            const int offset = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            const Area *localParent = study.areas.findFromName(thermalCluster->parentArea->name);
            if (localParent)
            {
                const ThermalCluster *localTC
                        = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterOffsets[localTC] = offset;
            }
        }
    }
}

void BindingConstraint::copyOffsets(const Study &study,
                                    const BindingConstraint &rhs,
                                    bool emptyBefore,
                                    Yuni::Bind<void(AreaName&, const AreaName&)>& translate)
{
    if (emptyBefore)
        pLinkOffsets.clear();
    if (rhs.pLinkOffsets.empty())
        return;

    AreaName fromID;
    AreaName withID;

    auto end = rhs.pLinkOffsets.end();
    for (auto i = rhs.pLinkOffsets.begin(); i != end; ++i)
    {
        // Alias to the current link
        const AreaLink *sourceLink = i->first;
        // offset
        const int offset = i->second;

        assert(sourceLink and "Invalid link in binding constraint");
        assert(sourceLink->from and "Invalid area pointer 'from' within link");
        assert(sourceLink->with and "Invalid area pointer 'with' within link");

        // Translatiob
        translate(fromID, sourceLink->from->id);
        translate(withID, sourceLink->with->id);

        const AreaLink *localLink = study.areas.findLink(fromID, withID);
        if (localLink)
            pLinkOffsets[localLink] = offset;
    }

    if (not rhs.pClusterOffsets.empty())
    {
        auto end = rhs.pClusterOffsets.end();
        for (auto i = rhs.pClusterOffsets.begin(); i != end; ++i)
        {
            // Alias to the current thermalCluster
            const ThermalCluster *thermalCluster = i->first;
            // weight
            const int offset = i->second;

            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            AreaName parentID;
            translate(parentID, thermalCluster->parentArea->id);

            const Area *localParent = study.areas.find(parentID);
            if (localParent)
            {
                const ThermalCluster *localTC
                        = localParent->thermal.list.find(thermalCluster->id());
                if (localTC)
                    pClusterOffsets[localTC] = offset;
            }
        }
    }
}

bool BindingConstraint::loadFromEnv(EnvForLoading& env, unsigned nb_years)
{
    return true;
}

void BindingConstraint::clear() {
    // Name / ID
    this->pName.clear();
    this->pID.clear();
    // No comments
    this->pComments.clear();
    // The type must be `hourly` by default for studies <=3.1, which was the only
// type of binding constraints supported.
    this->pType = typeUnknown;
    // The operator is `<` by default, which was the only option for studies <= 3.1
    this->pOperator = opUnknown;
    // Enabled: True by default to automatically allow the use of bindingconstraint
// from old studies (<= 3.1)
    this->pEnabled = true;
}

bool BindingConstraint::saveToEnv(BindingConstraint::EnvForSaving& env)
{
    env.section->add("name", pName);
    env.section->add("id", pID);
    env.section->add("enabled", pEnabled);
    env.section->add("type", TypeToCString(pType));
    env.section->add("operator", OperatorToCString(pOperator));
    env.section->add("filter-year-by-year", datePrecisionIntoString(pFilterYearByYear));
    env.section->add("filter-synthesis", datePrecisionIntoString(pFilterSynthesis));

    if (not pComments.empty())
        env.section->add("comments", pComments);

    if (not pLinkWeights.empty())
    {
        auto end = pLinkWeights.end();
        for (auto i = pLinkWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid link");
            assert(i->first->from and "Invalid area name");
            assert(i->first->with and "Invalid area name");

            const AreaLink &lnk = *(i->first);
            env.key.clear() << lnk.from->id << '%' << lnk.with->id;
            String value;
            value << i->second;
            if (pLinkOffsets.find(i->first) != pLinkOffsets.end())
                value << '%' << pLinkOffsets[i->first];
            // env.section->add(env.key, i->second);
            env.section->add(env.key, value);
        }
    }

    if (not pClusterWeights.empty())
    {
        auto end = pClusterWeights.end();
        for (auto i = pClusterWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid thermal cluster");

            const ThermalCluster &clstr = *(i->first);
            env.key.clear() << clstr.getFullName();
            String value;
            value << i->second;
            if (pClusterOffsets.find(i->first) != pClusterOffsets.end())
                value << '%' << pClusterOffsets[i->first];
            // env.section->add(env.key, i->second);
            env.section->add(env.key, value);
        }
    }

    // Exporting the matrix
    env.matrixFilename.clear() << env.folder << SEP << pID << ".txt";
    return time_series.saveToCSVFile(env.matrixFilename.c_str());
}

void BindingConstraintsList::clear()
{
    pList.clear();
}


void BindingConstraint::reverseWeightSign(const AreaLink* lnk)
{
    auto i = pLinkWeights.find(lnk);
    if (i != pLinkWeights.end())
    {
        i->second *= -1.;
        logs.info() << "Updated the binding constraint `" << pName << '`';
    }
}

void BindingConstraint::reverseWeightSign(const ThermalCluster* clstr)
{
    auto i = pClusterWeights.find(clstr);
    if (i != pClusterWeights.end())
    {
        i->second *= -1.;
        logs.info() << "Updated the binding constraint `" << pName << '`';
    }
}

bool BindingConstraint::contains(const Area* area) const
{
    const linkWeightMap::const_iterator end = pLinkWeights.end();
    for (linkWeightMap::const_iterator i = pLinkWeights.begin(); i != end; ++i)
    {
        if ((i->first)->from == area or (i->first)->with == area)
            return true;
    }

    const clusterWeightMap::const_iterator tEnd = pClusterWeights.end();
    for (clusterWeightMap::const_iterator i = pClusterWeights.begin(); i != tEnd; ++i)
    {
        if ((i->first)->parentArea == area)
            return true;
    }

    return false;
}

void BindingConstraint::buildFormula(String& s) const
{
    char tmp[42];
    bool first = true;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i)
    {
        if (!first)
            s << " + ";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);

        s << '(' << (const char *) tmp << " x " << (i->first)->getName();

        auto at = pLinkOffsets.find(i->first);
        if (at != pLinkOffsets.end())
        {
            int o = at->second;
            if (o > 0)
                s << " x (t + " << pLinkOffsets.find(i->first)->second << ')';
            if (o < 0)
                s << " x (t - " << Math::Abs(pLinkOffsets.find(i->first)->second) << ')';
        }

        s << ')';
        first = false;
    }

    auto tEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != tEnd; ++i)
    {
        if (!first)
            s << " + ";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);

        s << '(' << (const char *) tmp << " x " << (i->first)->getFullName();

        auto at = pClusterOffsets.find(i->first);
        if (at != pClusterOffsets.end())
        {
            int o = at->second;
            if (o > 0)
                s << " x (t + " << pClusterOffsets.find(i->first)->second << ')';
            if (o < 0)
                s << " x (t - " << Math::Abs(pClusterOffsets.find(i->first)->second) << ')';
        }

        if (not(i->first)->enabled || (i->first)->mustrun)
            s << " x N/A";

        s << ')';
        first = false;
    }
}

void BindingConstraint::buildHTMLFormula(String& s) const
{
    char tmp[42];
    s.clear();
    bool first = true;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i)
    {
        if (!first)
            s << " <font color=\"black\">+</font> ";
        s << "<font color=\"#AAAAAA\">(</font><font color=\"#FF781E\">";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);
        s << (const char *) tmp;
        s << "</font><font color=\"#FF2222\">x</font> <font color=\"#4F5B81\">"
          << (i->first)->from->name << '.' << (i->first)->with->name
          << "</font><font color=\"#AAAAAA\">)</font>";
        first = false;
    }

    auto tEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != tEnd; ++i)
    {
        if (!first)
            s << " <font color=\"black\">+</font> ";
        s << "<font color=\"#AAAAAA\">(</font><font color=\"#FF781E\">";
        SNPRINTF(tmp, sizeof(tmp), "%.2f", i->second);
        s << (const char *) tmp;
        s << "</font><font color=\"#FF2222\">x</font> <font color=\"#4F5B81\">"
          << (i->first)->name() << "</font><font color=\"#AAAAAA\">)</font>";
        first = false;
    }
}

Yuni::uint64 BindingConstraint::memoryUsage() const
{
    return sizeof(BindingConstraint)
           // comments
           + pComments.capacity()
           // Values
           + TimeSeries().memoryUsage()
           // Estimation
           + pLinkWeights.size() * (sizeof(double) + 3 * sizeof(void *))
           // Estimation
           + pLinkOffsets.size() * (sizeof(int) + 3 * sizeof(void *))
           // Estimation
           + pClusterWeights.size() * (sizeof(double) + 3 * sizeof(void *))
           // Estimation
           + pClusterOffsets.size() * (sizeof(int) + 3 * sizeof(void *));
}

bool BindingConstraint::contains(const BindingConstraint* bc) const
{
    return (this == bc);
}

bool BindingConstraint::contains(const AreaLink* lnk) const
{
    const linkWeightMap::const_iterator i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end());
}

bool BindingConstraint::contains(const ThermalCluster* clstr) const
{
    const clusterWeightMap::const_iterator i = pClusterWeights.find(clstr);
    return (i != pClusterWeights.end());
}

void BindingConstraint::enabled(bool v)
{
    pEnabled = v;
}

void BindingConstraint::operatorType(BindingConstraint::Operator o)
{
    pOperator = o;
}

uint BindingConstraint::yearByYearFilter() const
{
    return pFilterYearByYear;
}

uint BindingConstraint::synthesisFilter() const
{
    return pFilterSynthesis;
}

bool BindingConstraint::hasAllWeightedLinksOnLayer(size_t layerID)
{
    if (layerID == 0 || (linkCount() == 0 && clusterCount() == 0))
        return true;

    BindingConstraint::iterator endWeights = this->end();

    for (Data::BindingConstraint::iterator j = this->begin(); j != endWeights; ++j)
    {
        auto *areaLink = j->first;
        if (!areaLink)
            continue;

        if (!areaLink->isVisibleOnLayer(layerID) || j->second == 0)
        {
            return false;
        }
    }
    return true;
}

bool BindingConstraint::hasAllWeightedClustersOnLayer(size_t layerID)
{
    if (layerID == 0 || (linkCount() == 0 && clusterCount() == 0))
        return true;

    auto endWeights = pClusterWeights.end();

    for (auto j = pClusterWeights.begin(); j != endWeights; ++j)
    {
        auto *clstr = j->first;
        if (!clstr)
            continue;

        if (!clstr->isVisibleOnLayer(layerID) || j->second == 0)
        {
            return false;
        }
    }
    return true;
}

double BindingConstraint::weight(const AreaLink* lnk) const
{
    linkWeightMap::const_iterator i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end()) ? i->second : 0.;
}

double BindingConstraint::weight(const ThermalCluster* clstr) const
{
    clusterWeightMap::const_iterator i = pClusterWeights.find(clstr);
    return (i != pClusterWeights.end()) ? i->second : 0.;
}

int BindingConstraint::offset(const AreaLink* lnk) const
{
    linkOffsetMap::const_iterator i = pLinkOffsets.find(lnk);
    return (i != pLinkOffsets.end()) ? i->second : 0;
}

int BindingConstraint::offset(const ThermalCluster* lnk) const
{
    clusterOffsetMap::const_iterator i = pClusterOffsets.find(lnk);
    return (i != pClusterOffsets.end()) ? i->second : 0;
}

void BindingConstraint::initLinkArrays(std::vector<double>& w,
                                       std::vector<double>& cW,
                                       std::vector<int>& o,
                                       std::vector<int>& cO,
                                       std::vector<long>& linkIndex,
                                       std::vector<long>& clusterIndex,
                                       std::vector<long>& clustersAreaIndex) const
{
    uint off = 0;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i, ++off)
    {
        linkIndex[off] = (i->first)->index;
        w[off] = i->second;

        auto offsetIt = pLinkOffsets.find(i->first);
        if (offsetIt != pLinkOffsets.end())
            o[off] = offsetIt->second;
        else
            o[off] = 0;
    }

    off = 0;
    auto cEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != cEnd; ++i) {
        if (i->first->enabled && !i->first->mustrun) {
            clusterIndex[off] = (i->first)->index;
            clustersAreaIndex[off] = (i->first)->parentArea->index;
            cW[off] = i->second;

            auto offsetIt = pClusterOffsets.find(i->first);
            if (offsetIt != pClusterOffsets.end())
                cO[off] = offsetIt->second;
            else
                cO[off] = 0;

            ++off;
        }
    }
}

bool BindingConstraint::forceReload(bool reload) const
{
    return TimeSeries().forceReload(reload);
}

bool BindingConstraintsList::forceReload(bool reload) const
{
    if (not pList.empty())
    {
        bool ret = true;
        for (uint i = 0; i != pList.size(); ++i)
            ret = pList[i]->forceReload(reload) and ret;
        return ret;
    }
    return true;
}

void BindingConstraint::markAsModified() const
{
    TimeSeries().markAsModified();
}

void BindingConstraint::clearAndReset(const AnyString &name,
                                      BindingConstraint::Type newType,
                                      BindingConstraint::Operator op)
{
    // Name / ID
    pName = name;
    pID.clear();
    TransformNameIntoID(name, pID);
    // New type
    pType = newType;
    // Operator
    pOperator = op;
    // Resetting the weights
    pLinkWeights.clear();
    // Resetting the offsets
    pLinkOffsets.clear();
    // Resetting the weights
    pClusterWeights.clear();
    // Resetting the offsets
    pClusterOffsets.clear();

    switch (pType)
    {
    case typeUnknown:
    {
            time_series.reset();
            logs.error() << "invalid type for " << name << " (got 'unknown')";
            assert(false);
            break;
        }
    case typeHourly:
    {
            time_series.reset(columnMax, 8784, true);
            break;
        }
    case typeDaily:
    {
            time_series.reset(columnMax, 366, true);
            break;
        }
    case typeWeekly:
    {
            time_series.reset(columnMax, 366);
            break;
        }
    case typeMax:
    {
            time_series.reset(0, 0);
            logs.error() << "invalid type for " << name;
            break;
        }
    }
    time_series.markAsModified();
}

bool BindingConstraint::loadTimeSeries(EnvForLoading &env)
{
    if (env.version >= version860)
        return loadBoundedTimeSeries(env, operatorType());

    return loadTimeSeriesBefore860(env);
}

bool
BindingConstraint::loadBoundedTimeSeries(EnvForLoading &env, BindingConstraint::Operator operatorType) {
    bool load_ok = false;

    switch (operatorType) {
        case BindingConstraint::opLess:
            env.buffer.clear() << env.folder << SEP << name() << "_lt" << ".txt";
            break;
        case BindingConstraint::opGreater:
            env.buffer.clear() << env.folder << SEP << name() << "_gt" << ".txt";
            break;
        case BindingConstraint::opEquality:
            env.buffer.clear() << env.folder << SEP << name() << "_eq" << ".txt";
            break;
        default:
            assert(false);
    }
    load_ok = time_series.loadFromCSVFile(env.buffer,
                                          1,
                                          (type() == BindingConstraint::typeHourly) ? 8784 : 366,
                                          Matrix<>::optImmediate,
                                          &env.matrixBuffer);
    if (load_ok)
    {
        logs.info() << " loaded time series for `" << name() << "` (" << BindingConstraint::TypeToCString(type()) << ", "
                    << BindingConstraint::OperatorToShortCString(operatorType) << ')';
        return true;
    } else {
        return false;
    }
}

bool BindingConstraint::loadTimeSeriesBefore860(EnvForLoading &env)
{
    env.buffer.clear() << env.folder << SEP << pID << ".txt";
    Matrix<> intermediate;
    if (intermediate.loadFromCSVFile(env.buffer,
                columnMax,
                (pType == typeHourly) ? 8784 : 366,
                Matrix<>::optImmediate | Matrix<>::optFixedSize,
                &env.matrixBuffer))
    {
        if (pComments.empty())
            logs.info() << " added `" << pName << "` (" << TypeToCString(pType) << ", "
                << OperatorToShortCString(pOperator) << ')';
        else
            logs.info() << " added `" << pName << "` (" << TypeToCString(pType) << ", "
                << OperatorToShortCString(pOperator) << ") " << pComments;

        // 0 is BindingConstraint::opLess
        int columnNumber = Column::columnInferior;
        if (operatorType() == BindingConstraint::opGreater)
            columnNumber = Column::columnSuperior;
        else if (operatorType() == BindingConstraint::opEquality)
            columnNumber = Column::columnEquality;

        time_series.pasteToColumn(0, intermediate[columnNumber]);
        return true;
    }

    return false;
}

void BindingConstraint::matrix(const double one_value)
{
    //TODO (only type?)
    //pValues.fill(onevalue);
    //pValues.markAsModified();
}

std::string BindingConstraint::group() const {
    return group_;
}

void BindingConstraint::group(std::string group_name) {
    group_ = group_name;
    markAsModified();
}

const Matrix<>& BindingConstraint::TimeSeries() const {
    return time_series;
}

Matrix<>& BindingConstraint::TimeSeries() {
    return time_series;
}

void BindingConstraint::copyFrom(BindingConstraint *original) {
    clearAndReset(original->name(), original->type(), original->operatorType());
    pLinkWeights = original->pLinkWeights;
    pClusterWeights = original->pClusterWeights;
    pLinkOffsets = original->pLinkOffsets;
    pClusterOffsets = original->pClusterOffsets;
    pFilterYearByYear = original->pFilterYearByYear;
    pFilterSynthesis = original->pFilterSynthesis;
    pEnabled = original->pEnabled;
    pComments = original->pComments;
    group_ = original->group_;
    time_series.copyFrom(original->time_series);
}

} // namespace Antares
