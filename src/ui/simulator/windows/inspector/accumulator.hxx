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
#ifndef ANTARES_WINDOWS_INSPECTOR_ACCUMULATOR_HXX__
#define ANTARES_WINDOWS_INSPECTOR_ACCUMULATOR_HXX__

#include <antares/study/filter.h>
#include <array>
#include "constants.h"

namespace Antares
{
namespace Window
{
namespace Inspector
{
struct Unique
{
    template<class T>
    static bool Apply(T& value, const T& item)
    {
        return (value == item);
    }
};

struct Add
{
    template<class T>
    static bool Apply(T& value, const T& item)
    {
        value += item;
        return true;
    }
};

template<class PredicateT, class TraitsT = Unique>
struct Accumulator
{
public:
    template<class ListT>
    static void Apply(wxPGProperty* property, const ListT& list)
    {
        assert(list.size() != 0);
        assert(property != NULL);
        if (list.size() == 1)
        {
            auto study = *list.begin();
            property->SetValueFromString(PredicateT::ConvertToString(PredicateT::Value(study)));
        }
        else
        {
            auto i = list.cbegin();
            const auto end = list.cend();
            typename PredicateT::Type value = PredicateT::Value(*i);
            ++i;
            for (; i != end; ++i)
            {
                if (!TraitsT::template Apply(value, PredicateT::Value(*i)))
                {
                    property->SetValueToUnspecified();
                    return;
                }
            }
            property->SetValueFromString(PredicateT::ConvertToString(value));
        }
    }
};

template<class PredicateT>
struct AccumulatorCheck
{
public:
    template<class ListT>
    static void ApplyTextColor(wxPGProperty* property, const ListT& list)
    {
        assert(list.size() != 0);
        assert(property != NULL);
        if (list.size() == 1)
        {
            auto study = *list.begin();
            property->GetGrid()->SetPropertyTextColour(property->GetBaseName(),
                                                       PredicateT::TextColor(study));
        }
        else
        {
            auto i = list.cbegin();
            const auto end = list.cend();
            ++i;
            for (; i != end; ++i)
                property->GetGrid()->SetPropertyTextColour(property->GetBaseName(),
                                                           PredicateT::TextColor(*i));
        }
    }
};

struct PAreaColor
{
    struct Color
    {
        Color(const int c[3])
        {
            color[0] = c[0];
            color[1] = c[1];
            color[2] = c[2];
        }
        int color[3];
        bool operator==(const Color& rhs) const
        {
            return rhs.color[0] == color[0] && rhs.color[1] == color[1] && rhs.color[2] == color[2];
        }
    };
    typedef Color Type;
    static Type Value(const Data::Area* area)
    {
        return Color(area->ui->color);
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << wxT("(") << v.color[0] << wxT(',') << v.color[1] << wxT(',')
                          << v.color[2] << wxT(")");
    }
};

struct PStudyMode
{
    typedef Data::StudyMode Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return (!(!study) ? study->parameters.mode : Data::stdmEconomy);
    }
    static wxString ConvertToString(const Type v)
    {
        // NOTE stdmAdequacyDraft: should never happen since it could not be
        //  selected
        switch (v)
        {
        case Data::stdmEconomy:
            return wxT("Economy");
        case Data::stdmAdequacy:
            return wxT("Adequacy");
        case Data::stdmAdequacyDraft:
            return wxT("Draft");
        case Data::stdmExpansion:
            return wxT("Expansion");
        case Data::stdmUnknown:
        case Data::stdmMax:
            return wxEmptyString;
        }
        return wxEmptyString;
    }
};

struct PStudyBuildingMode
{
    typedef uint Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        if (!(!study))
        {
            auto& parameters = study->parameters;
            if (parameters.derated)
                return 2;
            if (parameters.useCustomScenario)
                return 1;
        }
        return 0;
    }
    static wxString ConvertToString(const Type v)
    {
        switch (v)
        {
        case 0:
            return wxT("Automatic");
        case 1:
            return wxT("Custom");
        case 2:
            return wxT("Derated");
        }
        return wxT("Automatic");
    }
};

struct PStudyCalendarMonth
{
    typedef uint Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !study ? 0 : (uint)study->parameters.firstMonthInYear;
    }
    static wxString ConvertToString(const Type v)
    {
        if (v < 12)
            return calendarMonths[v];
        return wxT("invalid");
    }
};

struct PStudyLeapYear
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !study ? false : study->parameters.leapYear;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("true") : wxT("false");
    }
};

struct PStudyCalendarWeek
{
    typedef uint Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !study ? (uint)Antares::monday : (uint)study->parameters.firstWeekday;
    }
    static wxString ConvertToString(const Type v)
    {
        if (v < 7)
            return calendarWeeks[v];
        return wxT("invalid");
    }
};

struct PStudyPlaylist
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.userPlaylist : false;
    }
    static wxString ConvertToString(const Type v)
    {
        switch (v)
        {
        case 0:
            return wxT("Automatic");
        case 1:
            return wxT("Custom");
        }
        return wxT("Automatic");
    }
};

struct PStudyYearByYear
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.yearByYear : false;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PStudySynthesis
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.synthesis : false;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PStudyGeographicTrimming
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.geographicTrimming : false;
    }
    static wxString ConvertToString(const Type v)
    {
        return (!v) ? wxT("None") : wxT("Custom");
    }
};

struct PStudyThematicTrimming
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.thematicTrimming : false;
    }
    static wxString ConvertToString(const Type v)
    {
        return (!v) ? wxT("None") : wxT("Custom");
    }
};

struct PStudyMCScenarios
{
    typedef bool Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.storeTimeseriesNumbers : false;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PStudyCalBegin
{
    typedef wxString Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return wxString() << ((!(!study) ? study->parameters.simulationDays.first : 0) + 1);
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

struct PStudyCalEnd
{
    typedef uint Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.simulationDays.end : 8760;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << v;
    }
};

struct PStudyYears
{
    typedef uint Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.nbYears : 1;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << v;
    }
};

struct PStudyHorizon
{
    typedef String Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !(!study) ? study->parameters.horizon : nullptr;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxStringFromUTF8(v);
    }
};

struct PStudy1stJanuary
{
    typedef Antares::DayOfTheWeek Type;
    static Type Value(const Data::Study::Ptr& study)
    {
        return !study ? Antares::monday : study->parameters.dayOfThe1stJanuary;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxStringFromUTF8(Antares::Date::DayOfTheWeekToString(v));
    }
};

template<bool Orientation>
struct PLinkArea
{
    typedef wxString Type;
    static Type Value(const Data::AreaLink* link)
    {
        if (Orientation)
            return wxStringFromUTF8(link->from->name);
        else
            return wxStringFromUTF8(link->with->name);
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

struct PLinkCopperPlate
{
    typedef Data::TransmissionCapacities Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->transmissionCapacities;
    }
    static wxString ConvertToString(const Type v)
    {
        switch (v)
        {
        case Data::tncEnabled:
            return wxT("enabled");
        case Data::tncIgnore:
            return wxT("set to zero");
        case Data::tncInfinite:
            return wxT("set to infinite");
        }
        return wxT("set to zero");
    }
};

struct PLinkAssetType
{
    typedef Data::AssetType Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->assetType;
    }
    static wxString ConvertToString(const Type v)
    {
        switch (v)
        {
        case Data::atAC:
            return wxT("ac");
        case Data::atDC:
            return wxT("dc");
        case Data::atGas:
            return wxT("gas");
        case Data::atVirt:
            return wxT("virt");
        case Data::atOther:
            return wxT("other");
        }
        return wxT("ac");
    }
};

struct PLinkHurdlesCost
{
    typedef bool Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->useHurdlesCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PLinkPhaseShift
{
    typedef bool Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->usePST;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PLinkLoopFlow
{
    typedef bool Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->useLoopFlow;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PLinkDisplayComments
{
    typedef bool Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->displayComments;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PLinkComments
{
    typedef wxString Type;
    static Type Value(const Data::AreaLink* link)
    {
        return wxStringFromUTF8(link->comments);
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

/*struct PLinkStyle
{
        typedef wxString Type;
        static Type Value(const Data::AreaLink* link)
        {
                return wxStringFromUTF8(link->style);
        }
        static wxString ConvertToString(const Type v)
        {
                return v;
        }
};*/
struct PLinkStyle
{
    typedef int Type;
    static Type Value(const Data::AreaLink* link)
    {
        return link->style;
    }
    static wxString ConvertToString(const Type v)
    {
        wxString ret;
        switch (v)
        {
        case 0:
            ret = "Plain";
            break;
        case 1:
            ret = "Dot";
            break;
        case 2:
            ret = "Dash";
            break;
        case 3:
            ret = "Dot & Dash";
            break;
        default:
            ret = "Plain";
            break;
        }
        return ret;
    }
};
struct PLinkWidth
{
    typedef int Type;
    static Type Value(const Data::AreaLink* link)
    {
        return Math::MinMax(link->linkWidth, 1, 6);
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PLinkColor
{
    struct Color
    {
        Color(const int c[3])
        {
            color[0] = c[0];
            color[1] = c[1];
            color[2] = c[2];
        }
        int color[3];
        bool operator==(const Color& rhs) const
        {
            return rhs.color[0] == color[0] && rhs.color[1] == color[1] && rhs.color[2] == color[2];
        }
    };
    typedef Color Type;
    static Type Value(const Data::AreaLink* link)
    {
        return Color(link->color);
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << wxT("(") << v.color[0] << wxT(',') << v.color[1] << wxT(',')
                          << v.color[2] << wxT(")");
    }
};

struct PAreaUnsuppliedEnergyCost
{
    typedef double Type;
    static Type Value(const Data::Area* area)
    {
        return area->thermal.unsuppliedEnergyCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PAdequacyPatchMode
{
    typedef Data::AdequacyPatch::AdequacyPatchMode Type;
    static Type Value(const Data::Area* area)
    {
        return area->adequacyPatchMode;
    }
    static wxString ConvertToString(const Type v)
    {
        switch (v)
        {
        case Data::AdequacyPatch::adqmVirtualArea:
            return wxT("virtual area");
        case Data::AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch:
            return wxT("physical area outside patch");
        case Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch:
            return wxT("physical area inside patch");
        }
        return wxEmptyString;
    }
};

template<enum Data::AreaNodalOptimization O>
struct PAreaResortStatus
{
    typedef bool Type;
    static Type Value(const Data::Area* area)
    {
        return (0 != (area->nodalOptimization & O));
    }
    static wxString ConvertToString(const Type v)
    {
        return (v) ? wxT("True") : wxT("False");
    }
};

template<bool SynthesisT, enum Data::FilterFlag F>
struct PAreaFiltering
{
    typedef bool Type;
    static Type Value(const Data::Area* area)
    {
        if (SynthesisT)
            return (0 != (area->filterSynthesis & F));
        else
            return (0 != (area->filterYearByYear & F));
    }
    static wxString ConvertToString(const Type v)
    {
        return (v) ? wxT("True") : wxT("False");
    }
};

template<bool SynthesisT, enum Data::FilterFlag F>
struct PLinkFiltering
{
    typedef bool Type;
    static Type Value(const Data::AreaLink* link)
    {
        if (SynthesisT)
            return (0 != (link->filterSynthesis & F));
        else
            return (0 != (link->filterYearByYear & F));
    }
    static wxString ConvertToString(const Type v)
    {
        return (v) ? wxT("True") : wxT("False");
    }
};

struct PAreaSpilledEnergyCost
{
    typedef double Type;
    static Type Value(const Data::Area* area)
    {
        return area->thermal.spilledEnergyCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

// ----------------
// THERMAL/RENEWABLE CLUSTERS
// ----------------
struct PClusterEnabled
{
    typedef bool Type;
    static Type Value(const Data::Cluster* cluster)
    {
        return cluster->enabled;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PClusterUnitCount
{
    typedef uint Type;
    static Type Value(const Data::Cluster* cluster)
    {
        return cluster->unitCount;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << v;
    }
};

struct PClusterNomCapacity
{
    typedef double Type;
    static Type Value(const Data::Cluster* cluster)
    {
        return cluster->nominalCapacity;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterInstalled
{
    typedef double Type;
    static Type Value(const Data::Cluster* cluster)
    {
        return cluster->nominalCapacity * cluster->unitCount;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterGroup
{
    typedef wxString Type;
    static Type Value(const Data::Cluster* cluster)
    {
        return wxStringFromUTF8(cluster->group());
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

struct PClusterArea
{
    typedef wxString Type;
    static Type Value(const Data::Cluster* cluster)
    {
        return wxStringFromUTF8(cluster->parentArea->name);
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

// ----------------
// THERMAL CLUSTERS
// ----------------
struct PClusterNomCapacityColor
{
    static wxColor TextColor(Data::ThermalCluster* cluster)
    {
        if (not cluster->checkMinStablePower())
            return wxColor(255, 0, 0);

        if (cluster->minStablePower > cluster->nominalCapacity * (1 - cluster->spinning / 100.))
            return wxColor(255, 0, 0);

        return wxColour(86, 98, 115);
    }

    static bool IsValid(Data::ThermalCluster* cluster)
    {
        return cluster->checkMinStablePower();
    }
};

struct PClusterMustRun
{
    typedef bool Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->mustrun;
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PClusterCO2
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->co2;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterVolatilityPlanned
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->plannedVolatility;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterVolatilityForced
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->forcedVolatility;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterSpinning
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->spinning;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterSpinningColor
{
    static wxColor TextColor(Data::ThermalCluster* cluster)
    {
        if (not cluster->checkMinStablePower())
            return wxColor(255, 0, 0);

        if (cluster->minStablePower > cluster->nominalCapacity * (1 - cluster->spinning / 100.))
            return wxColor(255, 0, 0);

        return wxColour(86, 98, 115);
    }

    static bool IsValid(Data::ThermalCluster* cluster)
    {
        return cluster->checkMinStablePower();
    }
};

struct PClusterReference
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->marketBidCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterRandomSpread
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->spreadCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterMarginalCost
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->marginalCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterStartupCost
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->startupCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterFixedCost
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->fixedCost;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterMinStablePower
{
    typedef double Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->minStablePower;
    }
    static wxString ConvertToString(const Type v)
    {
        return DoubleToWxString(v);
    }
};

struct PClusterMinStablePowerColor
{
    static wxColor TextColor(Data::ThermalCluster* cluster)
    {
        if (not cluster->checkMinStablePower())
            return wxColor(255, 0, 0);

        if (cluster->minStablePower > cluster->nominalCapacity * (1 - cluster->spinning / 100.))
            return wxColor(255, 0, 0);

        return wxColour(86, 98, 115);
    }

    static bool IsValid(Data::ThermalCluster* cluster)
    {
        return cluster->checkMinStablePower();
    }
};

struct PClusterMinUpTime
{
    typedef uint Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->minUpTime;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << v;
    }
};

struct PClusterMinDownTime
{
    typedef uint Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->minDownTime;
    }
    static wxString ConvertToString(const Type v)
    {
        return wxString() << v;
    }
};

struct PClusterLawForced
{
    typedef uint Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return (uint)cluster->forcedLaw;
    }
    static wxString ConvertToString(const Type v)
    {
        return (v < thermalLawCount) ? thermalLaws[v] : nullptr;
    }
};

struct PClusterLawPlanned
{
    typedef uint Type;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return cluster->plannedLaw;
    }
    static wxString ConvertToString(const Type v)
    {
        return (v < thermalLawCount) ? thermalLaws[v] : nullptr;
    }
};

struct PClusterDoGenerateTS
{
    using Type = uint;
    static Type Value(const Data::ThermalCluster* cluster)
    {
        return (uint)cluster->tsGenBehavior;
    }
    static wxString ConvertToString(const Type v)
    {
        return (v < localGenTSCount) ? localGenTS[v] : nullptr;
    }
};

struct PRnClusterTSMode
{
    typedef uint Type;
    static Type Value(const Data::RenewableCluster* cluster)
    {
        return cluster->tsMode;
    }
    static wxString ConvertToString(const Type v)
    {
        return (v < renewableTSModeCount) ? renewableTSMode[v] : nullptr;
    }
};

// -------------------
// BINDING CONSTRAINTS
// -------------------
struct PConstraintName
{
    typedef wxString Type;
    static Type Value(const Data::BindingConstraint* constraint)
    {
        return wxStringFromUTF8(constraint->name());
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

struct PConstraintComments
{
    typedef wxString Type;
    static Type Value(const Data::BindingConstraint* constraint)
    {
        return wxStringFromUTF8(constraint->comments());
    }
    static wxString ConvertToString(const Type v)
    {
        return v;
    }
};

struct PConstraintEnabled
{
    typedef bool Type;
    static Type Value(const Data::BindingConstraint* constraint)
    {
        return constraint->enabled();
    }
    static wxString ConvertToString(const Type v)
    {
        return v ? wxT("True") : wxT("False");
    }
};

struct PConstraintType
{
    typedef Data::BindingConstraint::Type Type;
    static Type Value(const Data::BindingConstraint* constraint)
    {
        return constraint->type();
    }
    static wxString ConvertToString(const Type v)
    {
        return wxStringFromUTF8(Data::BindingConstraint::TypeToCString(v));
    }
};
} // namespace Inspector
} // namespace Window
} // namespace Antares

#endif // ANTARES_WINDOWS_INSPECTOR_ACCUMULATOR_HXX__
