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
#include "BindingConstraintLoader.h"
#include "BindingConstraintSaver.h"

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

    if (l == "both" || l == "<>" || l == "><" || l == "< and >")
        return opBoth;
    if (l == "less" || l == "<" || l == "<=")
        return opLess;
    if (l == "greater" || l == ">" || l == ">=")
        return opGreater;
    if (l == "equal" || l == "=" || l == "==")
        return opEquality;
    return opUnknown;
}

BindingConstraint::Type BindingConstraint::StringToType(const AnyString& text)
{
    if (! text.empty())
    {
        ShortString16 l(text);
        l.toLower();
        switch (l.first())
        {
        case 'h':
        {
                if (l == "hourly" || l == "hour" || l == "h")
                    return typeHourly;
                break;
            }
        case 'd':
        {
                if (l == "daily" || l == "day" || l == "d")
                    return typeDaily;
                break;
            }
        case 'w':
        {
                if (l == "weekly" || l == "week" || l == "w")
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

    uint Antares::Data::BindingConstraint::enabledClusterCount() const
{
    return static_cast<uint>(std::count_if(
            pClusterWeights.begin(), pClusterWeights.end(), [](const clusterWeightMap::value_type &i) {
                return i.first->enabled && !i.first->mustrun;
            }));
}

void BindingConstraint::resetToDefaultValues()
{
    pEnabled = true;
    pComments.clear();
    RHSTimeSeries_.reset();
    markAsModified();
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

    if (!rhs.pClusterWeights.empty())
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

    if (!rhs.pClusterOffsets.empty())
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

bool BindingConstraint::contains(const Area* area) const
{
    const auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i)
    {
        if ((i->first)->from == area || (i->first)->with == area)
            return true;
    }

    const auto tEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != tEnd; ++i)
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

        if (auto at = pLinkOffsets.find(i->first); at != pLinkOffsets.end())
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

        if (auto at = pClusterOffsets.find(i->first); at != pClusterOffsets.end())
        {
            int o = at->second;
            if (o > 0)
                s << " x (t + " << pClusterOffsets.find(i->first)->second << ')';
            if (o < 0)
                s << " x (t - " << Math::Abs(pClusterOffsets.find(i->first)->second) << ')';
        }

        if (!(i->first)->enabled || (i->first)->mustrun)
            s << " x N/A";

        s << ')';
        first = false;
    }
}

Yuni::uint64 BindingConstraint::memoryUsage() const
{
    return sizeof(BindingConstraint)
           // comments
           + pComments.capacity()
           // Values
           + RHSTimeSeries().memoryUsage()
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
    const auto i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end());
}

bool BindingConstraint::contains(const ThermalCluster* clstr) const
{
    const auto i = pClusterWeights.find(clstr);
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

    auto endWeights = this->end();

    for (auto j = this->begin(); j != endWeights; ++j)
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
    auto i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end()) ? i->second : 0.;
}

double BindingConstraint::weight(const ThermalCluster* clstr) const
{
    auto i = pClusterWeights.find(clstr);
    return (i != pClusterWeights.end()) ? i->second : 0.;
}

int BindingConstraint::offset(const AreaLink* lnk) const
{
    auto i = pLinkOffsets.find(lnk);
    return (i != pLinkOffsets.end()) ? i->second : 0;
}

int BindingConstraint::offset(const ThermalCluster* lnk) const
{
    auto i = pClusterOffsets.find(lnk);
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

            if (auto offsetIt = pClusterOffsets.find(i->first); offsetIt != pClusterOffsets.end())
                cO[off] = offsetIt->second;
            else
                cO[off] = 0;

            ++off;
        }
    }
}

bool BindingConstraint::forceReload(bool reload) const
{
    return RHSTimeSeries().forceReload(reload);
}

void BindingConstraintsList::forceReload(bool reload) const
{
    if (!pList.empty())
    {
        for (const auto & i : pList)
            i->forceReload(reload);
    }
}

void BindingConstraint::markAsModified() const
{
    RHSTimeSeries().markAsModified();
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
            RHSTimeSeries_.reset();
            logs.error() << "invalid type for " << name << " (got 'unknown')";
            assert(false);
            break;
        }
    case typeHourly:
    {
            RHSTimeSeries_.reset(columnMax, 8784, true);
            break;
        }
    case typeDaily:
    {
            RHSTimeSeries_.reset(columnMax, 366, true);
            break;
        }
    case typeWeekly:
    {
            RHSTimeSeries_.reset(columnMax, 366);
            break;
        }
    case typeMax:
    {
            RHSTimeSeries_.reset(0, 0);
            logs.error() << "invalid type for " << name;
            break;
        }
    }
    RHSTimeSeries_.markAsModified();
}

std::string BindingConstraint::group() const {
    return group_;
}

void BindingConstraint::group(std::string group_name) {
    group_ = std::move(group_name);
    markAsModified();
}

const Matrix<>& BindingConstraint::RHSTimeSeries() const {
    return RHSTimeSeries_;
}

Matrix<>& BindingConstraint::RHSTimeSeries() {
    return RHSTimeSeries_;
}

void BindingConstraint::copyFrom(BindingConstraint const* original) {
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
    RHSTimeSeries_.copyFrom(original->RHSTimeSeries_);
}

} // namespace Antares
