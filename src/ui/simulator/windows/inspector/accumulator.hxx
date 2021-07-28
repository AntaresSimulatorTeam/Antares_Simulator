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

namespace Antares
{
namespace Window
{
namespace Inspector
{
enum
{
    arrayMinUpDownTimeCount = 168 + 3 + 1
};

static const wxChar* arrayMinUpDownTime[] = {wxT("1"),
                                             wxT("24"),
                                             wxT("168"),
                                             // --
                                             wxT(" -- "),
                                             // --
                                             wxT("1"),
                                             wxT("2"),
                                             wxT("3"),
                                             wxT("4"),
                                             wxT("5"),
                                             wxT("6"),
                                             wxT("7"),
                                             wxT("8"),
                                             wxT("9"),
                                             wxT("10"),
                                             wxT("11"),
                                             wxT("12"),
                                             wxT("13"),
                                             wxT("14"),
                                             wxT("15"),
                                             wxT("16"),
                                             wxT("17"),
                                             wxT("18"),
                                             wxT("19"),
                                             wxT("20"),
                                             wxT("21"),
                                             wxT("22"),
                                             wxT("23"),
                                             wxT("24"),
                                             wxT("25"),
                                             wxT("26"),
                                             wxT("27"),
                                             wxT("28"),
                                             wxT("29"),
                                             wxT("30"),
                                             wxT("31"),
                                             wxT("32"),
                                             wxT("33"),
                                             wxT("34"),
                                             wxT("35"),
                                             wxT("36"),
                                             wxT("37"),
                                             wxT("38"),
                                             wxT("39"),
                                             wxT("40"),
                                             wxT("41"),
                                             wxT("42"),
                                             wxT("43"),
                                             wxT("44"),
                                             wxT("45"),
                                             wxT("46"),
                                             wxT("47"),
                                             wxT("48"),
                                             wxT("49"),
                                             wxT("50"),
                                             wxT("51"),
                                             wxT("52"),
                                             wxT("53"),
                                             wxT("54"),
                                             wxT("55"),
                                             wxT("56"),
                                             wxT("57"),
                                             wxT("58"),
                                             wxT("59"),
                                             wxT("60"),
                                             wxT("61"),
                                             wxT("62"),
                                             wxT("63"),
                                             wxT("64"),
                                             wxT("65"),
                                             wxT("66"),
                                             wxT("67"),
                                             wxT("68"),
                                             wxT("69"),
                                             wxT("70"),
                                             wxT("71"),
                                             wxT("72"),
                                             wxT("73"),
                                             wxT("74"),
                                             wxT("75"),
                                             wxT("76"),
                                             wxT("77"),
                                             wxT("78"),
                                             wxT("79"),
                                             wxT("80"),
                                             wxT("81"),
                                             wxT("82"),
                                             wxT("83"),
                                             wxT("84"),
                                             wxT("85"),
                                             wxT("86"),
                                             wxT("87"),
                                             wxT("88"),
                                             wxT("89"),
                                             wxT("90"),
                                             wxT("91"),
                                             wxT("92"),
                                             wxT("93"),
                                             wxT("94"),
                                             wxT("95"),
                                             wxT("96"),
                                             wxT("97"),
                                             wxT("98"),
                                             wxT("99"),
                                             wxT("100"),
                                             wxT("101"),
                                             wxT("102"),
                                             wxT("103"),
                                             wxT("104"),
                                             wxT("105"),
                                             wxT("106"),
                                             wxT("107"),
                                             wxT("108"),
                                             wxT("109"),
                                             wxT("110"),
                                             wxT("111"),
                                             wxT("112"),
                                             wxT("113"),
                                             wxT("114"),
                                             wxT("115"),
                                             wxT("116"),
                                             wxT("117"),
                                             wxT("118"),
                                             wxT("119"),
                                             wxT("120"),
                                             wxT("121"),
                                             wxT("122"),
                                             wxT("123"),
                                             wxT("124"),
                                             wxT("125"),
                                             wxT("126"),
                                             wxT("127"),
                                             wxT("128"),
                                             wxT("129"),
                                             wxT("130"),
                                             wxT("131"),
                                             wxT("132"),
                                             wxT("133"),
                                             wxT("134"),
                                             wxT("135"),
                                             wxT("136"),
                                             wxT("137"),
                                             wxT("138"),
                                             wxT("139"),
                                             wxT("140"),
                                             wxT("141"),
                                             wxT("142"),
                                             wxT("143"),
                                             wxT("144"),
                                             wxT("145"),
                                             wxT("146"),
                                             wxT("147"),
                                             wxT("148"),
                                             wxT("149"),
                                             wxT("150"),
                                             wxT("151"),
                                             wxT("152"),
                                             wxT("153"),
                                             wxT("154"),
                                             wxT("155"),
                                             wxT("156"),
                                             wxT("157"),
                                             wxT("158"),
                                             wxT("159"),
                                             wxT("160"),
                                             wxT("161"),
                                             wxT("162"),
                                             wxT("163"),
                                             wxT("164"),
                                             wxT("165"),
                                             wxT("166"),
                                             wxT("167"),
                                             wxT("168"),
                                             // --
                                             nullptr};

static const uint arrayMinUpDownTimeValue[] = {1, 24, 168, 0};

enum
{
    arrayClusterGroupCount = 10
};
static const wxChar* arrayClusterGroup[] = {wxT("Gas"),
                                            wxT("Hard coal"),
                                            wxT("Lignite"),
                                            wxT("Mixed fuel"),
                                            wxT("Nuclear"),
                                            wxT("Oil"),
                                            wxT("Other"),
                                            wxT("Other 2"),
                                            wxT("Other 3"),
                                            wxT("Other 4"),
                                            nullptr};

enum
{
    arrayRnClusterGroupCount = 9
};
static const wxChar* arrayRnClusterGroup[] = { wxT("Wind Onshore"),
                                               wxT("Wind Offshore"),
                                               wxT("Solar Thermal"),
                                               wxT("Solar PV"),
                                               wxT("Solar Rooftop"),
                                               wxT("Other RES 1"),
                                               wxT("Other RES 2"),
                                               wxT("Other RES 3"),
                                               wxT("Other RES 4"),
                                               nullptr };

enum
{
    renewableTSModeCount = 2
};

static const wxChar* renewableTSMode[]
  = {wxT("power generation"), wxT("production factor"), nullptr};

enum
{
    thermalLawCount = 2
};
static const wxChar* thermalLaws[] = {wxT("uniform"), wxT("geometric"), nullptr};

static const wxChar* weekday[] = {wxT("Monday"),
                                  wxT("Tuesday"),
                                  wxT("Wednesday"),
                                  wxT("Thursday"),
                                  wxT("Friday"),
                                  wxT("Saturday"),
                                  wxT("Sunday"),
                                  nullptr};

static const wxChar* buildingMode[] = {wxT("Automatic"), wxT("Custom"), wxT("Derated"), nullptr};

static const wxChar* playlist[] = {wxT("Automatic"), wxT("Custom"), nullptr};

static const wxChar* geographicTrimming[] = {wxT("None"), wxT("Custom"), nullptr};

static const wxChar* thematicTrimming[] = {wxT("None"), wxT("Custom"), nullptr};

static const wxChar* calendarMonths[] = {wxT("JAN  -  DEC"),
                                         wxT("FEB  -  JAN"),
                                         wxT("MAR  -  FEB"),
                                         wxT("APR  -  MAR"),
                                         wxT("MAY  -  APR"),
                                         wxT("JUN  -  MAY"),
                                         wxT("JUL  -  JUN"),
                                         wxT("AUG  -  JUL"),
                                         wxT("SEP  -  AUG"),
                                         wxT("OCT  -  SEP"),
                                         wxT("NOV  -  OCT"),
                                         wxT("DEC  -  NOV"),
                                         nullptr};

static const wxChar* calendarWeeks[] = {wxT("MON  -  SUN"),
                                        wxT("TUE  -  MON"),
                                        wxT("WED  -  TUE"),
                                        wxT("THU  -  WED"),
                                        wxT("FRI  -  THU"),
                                        wxT("SAT  -  FRI"),
                                        wxT("SUN  -  SAT"),
                                        nullptr};

static const wxChar* studyMode[] = {wxT("Economy"),
                                    wxT("Adequacy"),
                                    wxT("draft"),
#if STUDY_MODE_EXPANSION == 1
                                    wxT("Expansion"),
#endif
                                    nullptr};

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
