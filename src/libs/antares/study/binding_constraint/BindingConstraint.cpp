// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/binding_constraint/BindingConstraint.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "antares/study/binding_constraint/BindingConstraintLoader.h"
#include "antares/study/study.h"
#include "antares/utils/utils.h"

using namespace Antares;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

namespace Antares::Data
{

BindingConstraint::Operator BindingConstraint::StringToOperator(const std::string& text)
{
    const std::string l = Antares::stringToLower(text);

    if (l == "both" || l == "<>" || l == "><" || l == "< and >")
    {
        return opBoth;
    }
    if (l == "less" || l == "<" || l == "<=")
    {
        return opLess;
    }
    if (l == "greater" || l == ">" || l == ">=")
    {
        return opGreater;
    }
    if (l == "equal" || l == "=" || l == "==")
    {
        return opEquality;
    }
    return opUnknown;
}

BindingConstraint::Type BindingConstraint::StringToType(const std::string& text)
{
    if (!text.empty())
    {
        const std::string l = Antares::stringToLower(text);
        switch (l.front())
        {
        case 'h':
        {
            if (l == "hourly" || l == "hour" || l == "h")
            {
                return typeHourly;
            }
            break;
        }
        case 'd':
        {
            if (l == "daily" || l == "day" || l == "d")
            {
                return typeDaily;
            }
            break;
        }
        case 'w':
        {
            if (l == "weekly" || l == "week" || l == "w")
            {
                return typeWeekly;
            }
            break;
        }
        default:
            break;
        }
    }
    logs.error() << "invalid type for binding constraint (got '" << text << "')";
    return typeUnknown;
}

const char* BindingConstraint::TypeToCString(const BindingConstraint::Type type)
{
    static const char* const names[typeMax + 1] = {"", "hourly", "daily", "weekly", ""};
    assert((unsigned int)type < (unsigned int)(typeMax + 1));
    return names[type];
}

const char* BindingConstraint::OperatorToCString(BindingConstraint::Operator o)
{
    static const char* const names[opMax + 1] = {"", "equal", "less", "greater", "both", ""};
    assert((unsigned int)o < (unsigned int)(opMax + 1));
    return names[o];
}

const char* BindingConstraint::OperatorToShortCString(BindingConstraint::Operator o)
{
    static const char* const names[opMax + 1] = {"",
                                                 "equality",
                                                 "bounded above",
                                                 "bounded below",
                                                 "bounded on both sides",
                                                 ""};
    assert((unsigned int)o < (unsigned int)(opMax + 1));
    return names[o];
}

const char* BindingConstraint::MathOperatorToCString(BindingConstraint::Operator o)
{
    static const char* const names[opMax + 1] = {"", "=", "<", ">", "< and >", ""};
    assert((unsigned int)o < (unsigned int)(opMax + 1));
    return names[o];
}

void BindingConstraint::name(const std::string& newname)
{
    pName = newname;
}

void BindingConstraint::pId(const std::string& name)
{
    pID = transformNameIntoID(name);
}

void BindingConstraint::weight(const AreaLink* lnk, double w)
{
    if (lnk)
    {
        if (Utils::isZero(w))
        {
            auto i = pLinkWeights.find(lnk);
            if (i != pLinkWeights.end())
            {
                pLinkWeights.erase(i);
            }
        }
        else
        {
            pLinkWeights[lnk] = w;
        }
    }
}

void BindingConstraint::weight(const ThermalCluster* cluster, double w)
{
    if (cluster)
    {
        if (Utils::isZero(w))
        {
            auto i = pClusterWeights.find(cluster);
            if (i != pClusterWeights.end())
            {
                pClusterWeights.erase(i);
            }
        }
        else
        {
            pClusterWeights[cluster] = w;
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
        if (Utils::isZero(o))
        {
            auto i = pLinkOffsets.find(lnk);
            if (i != pLinkOffsets.end())
            {
                pLinkOffsets.erase(i);
            }
        }
        else
        {
            pLinkOffsets[lnk] = o;
        }
    }
}

void BindingConstraint::offset(const ThermalCluster* cluster, int o)
{
    if (cluster)
    {
        if (Utils::isZero(o))
        {
            auto i = pClusterOffsets.find(cluster);
            if (i != pClusterOffsets.end())
            {
                pClusterOffsets.erase(i);
            }
        }
        else
        {
            pClusterOffsets[cluster] = o;
        }
    }
}

void BindingConstraint::resetToDefaultValues()
{
    pEnabled = true;
    RHSTimeSeries_.reset();
}

void BindingConstraint::copyWeights(
  const Study& study,
  const BindingConstraint& rhs,
  bool emptyBefore,
  const std::function<void(AreaName&, const AreaName&)>& translate)
{
    if (emptyBefore)
    {
        pLinkWeights.clear();
        pClusterWeights.clear();
    }

    if (rhs.pLinkWeights.empty())
    {
        return;
    }

    AreaName fromID;
    AreaName withID;

    for (const auto& [sourceLink, weight]: rhs.pLinkWeights)
    {
        assert(sourceLink and "Invalid link in binding constraint");
        assert(sourceLink->from and "Invalid area pointer 'from' within link");
        assert(sourceLink->with and "Invalid area pointer 'with' within link");

        // Translatiob
        translate(fromID, sourceLink->from->id);
        translate(withID, sourceLink->with->id);

        const AreaLink* localLink = study.areas.findLink(fromID, withID);
        if (localLink)
        {
            pLinkWeights[localLink] = weight;
        }
    }

    if (!rhs.pClusterWeights.empty())
    {
        for (const auto& [thermalCluster, weight]: rhs.pClusterWeights)
        {
            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            AreaName parentID;
            translate(parentID, thermalCluster->parentArea->id);

            const Area* localParent = study.areas.find(parentID);
            if (localParent)
            {
                const ThermalCluster* localTC = localParent->thermal.list.findInAll(
                  thermalCluster->id());
                if (localTC)
                {
                    pClusterWeights[localTC] = weight;
                }
            }
        }
    }
}

void BindingConstraint::copyOffsets(
  const Study& study,
  const BindingConstraint& rhs,
  bool emptyBefore,
  const std::function<void(AreaName&, const AreaName&)>& translate)
{
    if (emptyBefore)
    {
        pLinkOffsets.clear();
    }
    if (rhs.pLinkOffsets.empty())
    {
        return;
    }

    AreaName fromID;
    AreaName withID;

    for (const auto& [sourceLink, offset]: rhs.pLinkOffsets)
    {
        assert(sourceLink and "Invalid link in binding constraint");
        assert(sourceLink->from and "Invalid area pointer 'from' within link");
        assert(sourceLink->with and "Invalid area pointer 'with' within link");

        // Translatiob
        translate(fromID, sourceLink->from->id);
        translate(withID, sourceLink->with->id);

        const AreaLink* localLink = study.areas.findLink(fromID, withID);
        if (localLink)
        {
            pLinkOffsets[localLink] = offset;
        }
    }

    if (!rhs.pClusterOffsets.empty())
    {
        for (const auto& [thermalCluster, offset]: rhs.pClusterOffsets)
        {
            assert(thermalCluster and "Invalid thermal cluster in binding constraint");

            AreaName parentID;
            translate(parentID, thermalCluster->parentArea->id);

            const Area* localParent = study.areas.find(parentID);
            if (localParent)
            {
                const ThermalCluster* localTC = localParent->thermal.list.findInAll(
                  thermalCluster->id());
                if (localTC)
                {
                    pClusterOffsets[localTC] = offset;
                }
            }
        }
    }
}

void BindingConstraint::clear()
{
    // Name / ID
    this->pName.clear();
    this->pID.clear();
    // The type must be `hourly` by default for studies <=3.1, which was the only
    // type of binding constraints supported.
    this->pType = typeUnknown;
    // The operator is `<` by default, which was the only option for studies <= 3.1
    this->pOperator = opUnknown;
    // Enabled: True by default to automatically allow the use of bindingconstraint
    // from old studies (<= 3.1)
    this->pEnabled = true;
}

void BindingConstraint::enabled(bool v)
{
    pEnabled = v;
}

void BindingConstraint::operatorType(BindingConstraint::Operator o)
{
    pOperator = o;
}

unsigned int BindingConstraint::yearByYearFilter() const
{
    return pFilterYearByYear;
}

unsigned int BindingConstraint::synthesisFilter() const
{
    return pFilterSynthesis;
}

double BindingConstraint::weight(const AreaLink* lnk) const
{
    auto i = pLinkWeights.find(lnk);
    return (i != pLinkWeights.end()) ? i->second : 0.;
}

double BindingConstraint::weight(const ThermalCluster* cluster) const
{
    auto i = pClusterWeights.find(cluster);
    return (i != pClusterWeights.end()) ? i->second : 0.;
}

int BindingConstraint::offset(const AreaLink* lnk) const
{
    auto i = pLinkOffsets.find(lnk);
    return (i != pLinkOffsets.end()) ? i->second : 0;
}

int BindingConstraint::offset(const ThermalCluster* cluster) const
{
    auto i = pClusterOffsets.find(cluster);
    return (i != pClusterOffsets.end()) ? i->second : 0;
}

BindingConstraintStructures BindingConstraint::initLinkArrays() const
{
    std::vector<long> linkIndex;
    std::vector<double> linkWeight;
    std::vector<double> clusterWeight;
    std::vector<int> linkOffset;
    std::vector<int> clusterOffset;
    std::vector<long> clusterIndex;
    std::vector<long> clustersAreaIndex;

    linkWeight.resize(linkCount());
    linkOffset.resize(linkCount());
    linkIndex.resize(linkCount());

    clusterWeight.resize(clusterCount());
    clusterOffset.resize(clusterCount());
    clusterIndex.resize(clusterCount());
    clustersAreaIndex.resize(clusterCount());

    unsigned int off = 0;
    auto end = pLinkWeights.end();
    for (auto i = pLinkWeights.begin(); i != end; ++i, ++off)
    {
        linkIndex[off] = (i->first)->index;
        linkWeight[off] = i->second;

        auto offsetIt = pLinkOffsets.find(i->first);
        if (offsetIt != pLinkOffsets.end())
        {
            linkOffset[off] = offsetIt->second;
        }
        else
        {
            linkOffset[off] = 0;
        }
    }

    off = 0;
    auto cEnd = pClusterWeights.end();
    for (auto i = pClusterWeights.begin(); i != cEnd; ++i)
    {
        if (i->first->isActive())
        {
            clusterIndex[off] = (i->first)->index;
            clustersAreaIndex[off] = (i->first)->parentArea->index;
            clusterWeight[off] = i->second;

            if (auto offsetIt = pClusterOffsets.find(i->first); offsetIt != pClusterOffsets.end())
            {
                clusterOffset[off] = offsetIt->second;
            }
            else
            {
                clusterOffset[off] = 0;
            }

            ++off;
        }
    }

    return {
      linkIndex,
      linkWeight,
      clusterWeight,
      linkOffset,
      clusterOffset,
      clusterIndex,
      clustersAreaIndex,
    };
}

const BindingConstraint::clusterWeightMap& BindingConstraint::clustersAndWeights() const
{
    return pClusterWeights;
}

void BindingConstraint::clearAndReset(const std::string& name,
                                      BindingConstraint::Type newType,
                                      BindingConstraint::Operator op)
{
    // Name / ID
    pName = name;
    pID = transformNameIntoID(name);
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
        RHSTimeSeries_.reset(columnMax, 8784);
        break;
    }
    case typeDaily:
    {
        RHSTimeSeries_.reset(columnMax, 366);
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
}

std::string BindingConstraint::group() const
{
    return group_;
}

void BindingConstraint::group(std::string group_name)
{
    group_ = std::move(group_name);
}

const Matrix<>& BindingConstraint::RHSTimeSeries() const
{
    return RHSTimeSeries_;
}

Matrix<>& BindingConstraint::RHSTimeSeries()
{
    return RHSTimeSeries_;
}

void BindingConstraint::copyFrom(const BindingConstraint* original)
{
    clearAndReset(original->name(), original->type(), original->operatorType());
    pLinkWeights = original->pLinkWeights;
    pClusterWeights = original->pClusterWeights;
    pLinkOffsets = original->pLinkOffsets;
    pClusterOffsets = original->pClusterOffsets;
    pFilterYearByYear = original->pFilterYearByYear;
    pFilterSynthesis = original->pFilterSynthesis;
    pEnabled = original->pEnabled;
    group_ = original->group_;
    RHSTimeSeries_.copyFrom(original->RHSTimeSeries_);
}

} // namespace Antares::Data
