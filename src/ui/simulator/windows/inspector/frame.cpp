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

#include <antares/wx-wrapper.h>
#include "frame.h"
#include "../../application/main.h"
#include <wx/wupdlock.h>
#include <wx/fontdlg.h>
#include <wx/propgrid/editors.h>

#include <antares/study/study.h>
#include <antares/study/area/constants.h>
#include "../../application/study.h"
#include "editor-calendar.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/components/map/component.h"
#include "../../toolbox/components/mainpanel.h"
#include "../../toolbox/components/button.h"
#include <ui/common/lock.h>
#include "grid.h"

using namespace Yuni;

#include "accumulator.hxx"

#define P_FLOAT(CAPTION, ID) page->Append(new wxFloatProperty(wxT(CAPTION), wxT(ID), 0.))

#define P_STRING(CAPTION, ID) \
    page->Append(new wxStringProperty(wxT(CAPTION), wxT(ID), wxEmptyString))

#define P_BOOL(CAPTION, ID) page->Append(new wxBoolProperty(wxT(CAPTION), wxT(ID), false))

#define P_INT(CAPTION, ID) page->Append(new wxIntProperty(wxT(CAPTION), wxT(ID), 0))

#define P_UINT(CAPTION, ID) page->Append(new wxUIntProperty(wxT(CAPTION), wxT(ID), 0))

#define P_ENUM(CAPTION, ID, FILTER) page->Append(new wxEnumProperty(wxT(CAPTION), wxT(ID), FILTER));

namespace Antares
{
namespace Window
{
namespace Inspector
{
// Singleton
// FIXME Singleton should not be used in any programs !
// (assigned in inspector.cpp)
Frame* gInspector = nullptr;

// The global selection for the unique inspector
InspectorData::Ptr gData;

// wx stuff
WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(StudyCalendarBtnEditor,
                                      StudyCalendarBtnEditor,
                                      wxPGTextCtrlEditor)

static inline wxPGProperty* Group(wxPropertyGrid* grid, const wxChar* caption, const wxChar* id)
{
    wxPGProperty* p = grid->Append(new wxPropertyCategory(caption, id));
    grid->SetPropertyTextColour(p, wxColor(238, 107, 0));
    return p;
}

static inline wxPGProperty* Category(wxPropertyGrid* grid, const wxChar* caption, const wxChar* id)
{
    return grid->Append(new wxPropertyCategory(caption, id));
}

void Frame::delayApply()
{
    Dispatcher::GUI::Post(this, &Frame::onDelayApply, 50);
}

void Frame::onDelayApply()
{
    apply(pCurrentSelection);
}

void Frame::delayApplyGlobalSelection()
{
    Dispatcher::GUI::Post(this, &Frame::onDelayApplyGlobalSelection, 50);
}

void Frame::onDelayApplyGlobalSelection()
{
    apply(gData);
}

void Frame::onSelectAllLinks(wxCommandEvent&)
{
    InspectorData::Ptr data = gData;
    if (!(!data) and gInspector)
    {
        data->links.clear();
        auto areaEnd = data->areas.end();
        for (auto i = data->areas.begin(); i != areaEnd; ++i)
        {
            auto& area = *(*i);
            auto end = area.links.end();
            for (auto i = area.links.begin(); i != end; ++i)
                data->links.insert(i->second);
        }
        data->areas.clear();
        data->ThClusters.clear();
        data->RnClusters.clear();
        data->empty = data->links.empty();
        gInspector->delayApplyGlobalSelection();
    }
}

void Frame::onSelectLink(wxCommandEvent& evt)
{
    InspectorData::Ptr data = gData;
    if (!(!data) and gInspector and evt.GetEventObject())
    {
        data->links.clear();
        data->links.insert((Data::AreaLink*)mapIDPointer[evt.GetId()]);
        data->areas.clear();
        data->ThClusters.clear();
        data->RnClusters.clear();
        data->empty = data->links.empty();
        gInspector->delayApplyGlobalSelection();
    }
}

void Frame::onSelectAllLinksFromArea(wxCommandEvent& evt)
{
    InspectorData::Ptr data = gData;
    if (!(!data) and gInspector and evt.GetEventObject())
    {
        data->links.clear();
        auto* area = (Data::Area*)mapIDPointer[evt.GetId()];
        if (!area)
            return;
        const auto end = area->links.end();
        for (auto i = area->links.begin(); i != end; ++i)
            data->links.insert(i->second);
        data->areas.clear();
        data->ThClusters.clear();
        data->RnClusters.clear();
        data->empty = data->links.empty();
        gInspector->delayApplyGlobalSelection();
    }
}

// gp : never used - to be removed
void Frame::onSelectAllPlants(wxCommandEvent&)
{
    InspectorData::Ptr data = gData;
    if (!(!data) and gInspector)
    {
        data->ThClusters.clear();
        auto areaEnd = data->areas.end();
        for (auto i = data->areas.begin(); i != areaEnd; ++i)
        {
            Data::Area& area = *(*i);
            auto end = area.thermal.list.end();
            for (auto i = area.thermal.list.begin(); i != end; ++i)
                data->ThClusters.insert(i->second.get());
        }
        data->areas.clear();
        data->links.clear();
        data->empty = data->ThClusters.empty();
        gInspector->delayApplyGlobalSelection();
    }
}

// gp : never used - to be removed
void Frame::onSelectPlant(wxCommandEvent& evt)
{
    InspectorData::Ptr data = gData;
    if (!(!data) and gInspector and evt.GetEventObject())
    {
        data->ThClusters.clear();
        data->ThClusters.insert((Data::ThermalCluster*)mapIDPointer[evt.GetId()]);
        data->areas.clear();
        data->links.clear();
        data->empty = data->ThClusters.empty();
        gInspector->delayApplyGlobalSelection();
    }
}

// gp : never used - to be removed
void Frame::onSelectAllPlantsFromArea(wxCommandEvent& evt)
{
    InspectorData::Ptr data = gData;
    if (!(!data) and gInspector and evt.GetEventObject())
    {
        Data::Area* area = (Data::Area*)mapIDPointer[evt.GetId()];
        if (!area)
            return;
        data->ThClusters.clear();
        auto end = area->thermal.list.end();
        for (auto i = area->thermal.list.begin(); i != end; ++i)
            data->ThClusters.insert(i->second.get());
        data->areas.clear();
        data->links.clear();
        data->empty = data->ThClusters.empty();
        gInspector->delayApplyGlobalSelection();
    }
}

Frame::Frame(wxWindow* parent, bool allowAnyObject) :
 Antares::Component::Panel(parent),
 pNotes(nullptr),
 pAllowAnyObject(allowAnyObject),
 pBtnInspector(nullptr),
 pAlreadyConnectedToSimulationChangesEvent(false)
{
    // Registering out custom editors for the property grid
    // The registration must only be done once !
    {
        static bool alreadyRegistered = false;
        if (!alreadyRegistered)
        {
            alreadyRegistered = true;
            wxPGEditor_StudyCalendarBtnEditor = wxPropertyGrid::DoRegisterEditorClass(
              new StudyCalendarBtnEditor(), "StudyCalendarBtnEditor");
        }
    }

#ifdef YUNI_OS_WINDOWS
    enum
    {
        margin = 2
    };
#else
    enum
    {
        margin = 0
    };
#endif

    wxSizer* s = new wxBoxSizer(wxVERTICAL);
    pMainPanel = new Antares::Component::Panel(this);
    s->Add(pMainPanel, 1, wxALL | wxEXPAND);

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pMainPanel->SetSizer(sizer);

    if (pAllowAnyObject)
    {
        wxSizer* sizerToolbar = new wxBoxSizer(wxHORIZONTAL);
        sizerToolbar->AddSpacer(5);

        Component::Button* btn;

        // Properties Selector
        btn = new Component::Button(pMainPanel,
                                    wxT("2 items"),
                                    "images/16x16/property.png",
                                    this,
                                    &Frame::onSelectProperties);
        pBtnInspector = btn;
        sizerToolbar->Add(btn, 0, wxALL | wxEXPAND, margin);
        // User's notes Selector
        btn = new Component::Button(
          pMainPanel, wxT("notes"), "images/16x16/notes.png", this, &Frame::onSelectNotes);
        sizerToolbar->Add(btn, 0, wxALL | wxEXPAND, margin);

        sizer->Add(sizerToolbar, 0, wxALL | wxEXPAND, 2);
    }

    if (pAllowAnyObject)
    {
        pNotes = new Window::Notes(pMainPanel, 5);
        sizer->Add(pNotes, 1, wxALL | wxEXPAND);
        sizer->Show(pNotes, false, false);
    }

    InspectorGrid* pg = new InspectorGrid(*this,
                                          pMainPanel,
                                          wxID_ANY,
                                          wxDefaultPosition,
                                          wxDefaultSize,
                                          wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLTIPS | wxPG_TOOLBAR);
    pApplyPropertyGrid.bind(pg, &InspectorGrid::apply);
    pg->InitAllTypeHandlers();
    pPropertyGrid = pg;
    sizer->Add(pg, 1, wxALL | wxEXPAND);

    // pg->Connect(pg->GetId(), wxEVT_PG_CHANGING,
    // wxPropertyGridEventHandler(Frame:onPropertChanging), nullptr, this);

    wxPGProperty* lid;
    // Color default
    pg->SetCellTextColour(wxColour(86, 98, 115));
    // Color caption
    pg->SetCaptionTextColour(wxColour(40, 40, 40));
    pg->SetCaptionBackgroundColour(wxColour(247, 248, 255));
    // Color selection
    pg->SetSelectionBackgroundColour(wxColour(91, 103, 120));
    pg->SetSelectionTextColour(wxColour(255, 255, 255));

    //
    // --- PAGE ---
    //
    wxPropertyGrid* page = pg;

    // --- STUDY ---
    if (pAllowAnyObject)
    {
        Group(pg, wxT("STUDY"), wxT("study.title"));
        Category(pg, wxT("General"), wxT("study.general"));
        pPGCommonStudyName
          = page->Append(new wxStringProperty(wxT("name"), wxT("common.study.name"), wxT("")));
        pPGCommonStudyAuthor
          = page->Append(new wxStringProperty(wxT("author"), wxT("common.study.author"), wxT("")));
    }
    else
    {
        pPGCommonStudyName = nullptr;
        pPGCommonStudyAuthor = nullptr;
    }

    // --- STUDIES ---
    pPGStudyTitle = Group(pg, wxT("GENERAL PARAMETERS"), wxT("study.title"));
    pPGStudyGrpSimulation = Category(pg, wxT("Simulation"), wxT("study.context"));
    pPGStudyMode = P_ENUM("Mode", "study.mode", studyMode);
    pPGStudyCalendarBegin = P_UINT("First day", "study.cal.begin");
    pPGStudyCalendarEnd = P_UINT("Last day", "study.cal.end");
    pg->SetPropertyEditor(pPGStudyCalendarBegin, wxPG_EDITOR(StudyCalendarBtnEditor));
    pg->SetPropertyEditor(pPGStudyCalendarEnd, wxPG_EDITOR(StudyCalendarBtnEditor));
    pPGStudyGrpCalendar = Category(pg, wxT("Calendar"), wxT("study.simu.calendar"));
    pPGStudyHorizon = P_STRING("Horizon", "study.horizon");
    pPGStudyCalendarMonth = P_ENUM("Year", "study.calendarmonth", calendarMonths);
    pPGStudyLeapYear = P_BOOL("Leap year", "study.leap");
    pPGStudyCalendarWeek = P_ENUM("Week", "study.calendarweek", calendarWeeks);
    pPGStudy1rstJanuary = P_ENUM("1st january", "study.1rsjanuary", weekday);

    pPGStudySpace1 = Category(pg, wxEmptyString, wxEmptyString);
    pPGStudyMCScenarios = Category(pg, wxT("Monte-Carlo Scenarios"), wxT("study.mc_scenarios"));

    pPGStudyYears = P_UINT("Number", "study.years");

    pPGStudyBuildMode = P_ENUM("Building mode", "study.buildingmode", buildingMode);
    pPGStudyPlaylist = P_ENUM("Selection mode", "study.playlist", playlist);

    pPGStudySpace2 = Category(pg, wxEmptyString, wxEmptyString);
    pPGStudyOutputProfile = Category(pg, wxT("Output profile"), wxT("study.output_profile"));
    pPGStudySimulationSynthesis = P_BOOL("Simulation synthesis", "study.simsynthesis");
    pPGStudyYearByYear = P_BOOL("Year-by-year", "study.yearbyyear");
    pPGGeographicTrimming
      = P_ENUM("Geographic trimming", "study.geographictrimming", geographicTrimming);
    pPGThematicTrimming = P_ENUM("Thematic trimming", "study.thematictrimming", thematicTrimming);
    pPGStudyUseMCScenarios = P_BOOL("MC Scenarios", "study.scenarios");

    // --- AREA ---
    pPGAreaSeparator = Group(pg, wxEmptyString, wxEmptyString);
    pPGAreaTitle = Group(pg, wxT("AREAS"), wxT("area.title"));
    pPGAreaGeneral = Category(pg, wxT("General"), wxT("area.general"));
    pPGAreaName = P_STRING("Name", "area.name");
    pPGAreaColor
      = page->Append(new wxColourProperty(wxT("color"), wxT("area.color"), wxColour(0, 0, 0)));

    pPGAreaOptimization = Category(pg, wxT("Nodal optimization"), wxT("area.optimization"));
    lid = page->Append(
      new wxStringProperty(wxT("Energy cost (\u20AC/Wh)"), wxT("area.energy"), wxT("<composed>")));
    pPGUnsupplied
      = page->AppendIn(lid, new wxFloatProperty(wxT("unsupplied"), wxT("unsupplied"), 1.));
    pPGSpilled = page->AppendIn(lid, new wxFloatProperty(wxT("spilled"), wxT("spilled"), 1.));

    pPGAreaResort = page->Append(new wxStringProperty(
      wxT("Last resort shedding status"), wxT("area.resort"), wxT("<composed>")));
    pPGAreaResortNon = page->AppendIn(
      pPGAreaResort,
      new wxBoolProperty(wxT("non dispatch. power"), wxT("area.non_dispatch_power"), false));
    pPGAreaResortHydroPower = page->AppendIn(
      pPGAreaResort,
      new wxBoolProperty(wxT("dispatch. hydropower"), wxT("area.dispatch_hydropower"), false));
    pPGAreaResortOther = page->AppendIn(
      pPGAreaResort,
      new wxBoolProperty(wxT("other dispatch. power"), wxT("area.other_dispatch_power"), false));

    pPGAreaAdequacyPatchTitle
      = Category(pg, wxT("Adequacy Patch"), wxT("area.adequacy_patch_title"));
    pPGAreaAdequacyPatchMode = page->Append(new wxEnumProperty(
      wxT("adequacy patch mode"), wxT("area.adequacy_patch_mode"), adequacyPatchMode));

    pPGAreaLocalization = Category(pg, wxT("Localization"), wxT("area.localization"));
    P_INT("x", "area.x");
    P_INT("y", "area.y");

    pPGAreaFilteringStatus = Category(pg, wxT("Output print status"), wxT("area.filtering"));
    lid = page->Append(new wxStringProperty(
      wxT("Output synthesis"), wxT("area.filtering-synthesis"), wxT("<composed>")));
    pPGAreaFilteringSynthesis[0]
      = page->AppendIn(lid, new wxBoolProperty(wxT("hourly"), wxT("hourly"), true));
    pPGAreaFilteringSynthesis[1]
      = page->AppendIn(lid, new wxBoolProperty(wxT("daily"), wxT("daily"), true));
    pPGAreaFilteringSynthesis[2]
      = page->AppendIn(lid, new wxBoolProperty(wxT("weekly"), wxT("weekly"), true));
    pPGAreaFilteringSynthesis[3]
      = page->AppendIn(lid, new wxBoolProperty(wxT("monthly"), wxT("monthly"), true));
    pPGAreaFilteringSynthesis[4]
      = page->AppendIn(lid, new wxBoolProperty(wxT("annual"), wxT("annual"), true));
    lid = page->Append(new wxStringProperty(
      wxT("Output Year-by-year"), wxT("area.filtering-year-by-year"), wxT("<composed>")));
    pPGAreaFilteringYbY[0]
      = page->AppendIn(lid, new wxBoolProperty(wxT("hourly"), wxT("hourly"), true));
    pPGAreaFilteringYbY[1]
      = page->AppendIn(lid, new wxBoolProperty(wxT("daily"), wxT("daily"), true));
    pPGAreaFilteringYbY[2]
      = page->AppendIn(lid, new wxBoolProperty(wxT("weekly"), wxT("weekly"), true));
    pPGAreaFilteringYbY[3]
      = page->AppendIn(lid, new wxBoolProperty(wxT("monthly"), wxT("monthly"), true));
    pPGAreaFilteringYbY[4]
      = page->AppendIn(lid, new wxBoolProperty(wxT("annual"), wxT("annual"), true));

    pPGAreaDeps = Category(pg, wxT("Dependencies"), wxT("area.deps"));
    pPGAreaLinks = P_UINT("links", "area.link_count");
    pPGAreaPlants = P_UINT("thermal clusters", "area.cluster_count");

    // --- LINK ---
    pPGLinkSeparator = Group(pg, wxEmptyString, wxEmptyString);
    Group(pg, wxT("1 LINK"), wxT("link.title"));
    Category(pg, wxT("Connection"), wxT("link.connection"));
    lid = P_STRING("From", "link.from");
    pg->DisableProperty(lid);
    lid = P_STRING("To", "link.to");
    pg->DisableProperty(lid);

    Category(pg, wxT("Parameters"), wxT("link.params"));
    P_BOOL("Hurdles cost", "link.hurdles_cost");
    P_BOOL("Loop flow", "link.use_loop_flow")->Enable(false);
    P_BOOL("Phase shifter", "link.use_phase_shifter")->Enable(false);
    //	P_BOOL("Trans. capacities", "link.transmission-capacities");
    Category(pg, wxT("Caption"), wxT("link.coms"));
    P_BOOL("Caption on map", "link.display_comments");
    P_STRING("Caption", "link.comments");

    Category(pg, wxT("Style"), wxT("link.styles"));
    pPGLinkColor
      = page->Append(new wxColourProperty(wxT("Color"), wxT("link.color"), wxColour(0, 0, 0)));

    wxPGChoices arrStyle;
    arrStyle.Add(wxT("Plain"), Data::stPlain);
    arrStyle.Add(wxT("Dot"), Data::stDot);
    arrStyle.Add(wxT("Dash"), Data::stDash);
    arrStyle.Add(wxT("Dot & Dash"), Data::stDotDash);
    pPGLinkStyle = pg->Append(new wxEnumProperty(wxT("Style"), wxT("link.style"), arrStyle));
    pPGLinkWidth = P_INT("Width", "link.width");
    pPGLinkFilteringStatus = Category(pg, wxT("Output print status"), wxT("link.filtering"));
    lid = page->Append(new wxStringProperty(
      wxT("Output synthesis"), wxT("link.filtering-synthesis"), wxT("<composed>")));
    pPGLinkFilteringSynthesis[0]
      = page->AppendIn(lid, new wxBoolProperty(wxT("hourly"), wxT("hourly"), true));
    pPGLinkFilteringSynthesis[1]
      = page->AppendIn(lid, new wxBoolProperty(wxT("daily"), wxT("daily"), true));
    pPGLinkFilteringSynthesis[2]
      = page->AppendIn(lid, new wxBoolProperty(wxT("weekly"), wxT("weekly"), true));
    pPGLinkFilteringSynthesis[3]
      = page->AppendIn(lid, new wxBoolProperty(wxT("monthly"), wxT("monthly"), true));
    pPGLinkFilteringSynthesis[4]
      = page->AppendIn(lid, new wxBoolProperty(wxT("annual"), wxT("annual"), true));
    lid = page->Append(new wxStringProperty(
      wxT("Output Year-by-year"), wxT("link.filtering-year-by-year"), wxT("<composed>")));
    pPGLinkFilteringYbY[0]
      = page->AppendIn(lid, new wxBoolProperty(wxT("hourly"), wxT("hourly"), true));
    pPGLinkFilteringYbY[1]
      = page->AppendIn(lid, new wxBoolProperty(wxT("daily"), wxT("daily"), true));
    pPGLinkFilteringYbY[2]
      = page->AppendIn(lid, new wxBoolProperty(wxT("weekly"), wxT("weekly"), true));
    pPGLinkFilteringYbY[3]
      = page->AppendIn(lid, new wxBoolProperty(wxT("monthly"), wxT("monthly"), true));
    pPGLinkFilteringYbY[4]
      = page->AppendIn(lid, new wxBoolProperty(wxT("annual"), wxT("annual"), true));

    // --- THERMAL CLUSTERS ---
    // gp : all thermal property names should be renamed "th-cluster.<something>" instead of
    // "cluster.<something>"
    pPGThClusterSeparator = Group(pg, wxEmptyString, wxEmptyString);
    Group(pg, wxT("THERMAL CLUSTERS"), wxT("cluster.title"));
    pPGThClusterGeneral = Category(pg, wxT("General"), wxT("cluster.general"));
    pPGThClusterName = P_STRING("Name", "cluster.name");

    wxPGChoices ThGroupChoices;
    for (uint i = 0; i != arrayClusterGroupCount; ++i)
        ThGroupChoices.Add(arrayClusterGroup[i], i);
    pPGThClusterGroup = page->Append(
      new wxEditEnumProperty(wxT("group"), wxT("cluster.group"), ThGroupChoices, wxEmptyString));

    pPGThClusterArea = P_STRING("area", "cluster.area");
    pg->DisableProperty(pPGThClusterArea);

    pPGThClusterParams = Category(pg, wxT("Operating parameters"), wxT("cluster.params"));
    pPGThClusterEnabled = P_BOOL("Enabled", "cluster.enabled");
    pPGThClusterUnitCount = P_UINT("Unit", "cluster.unit");
    pPGThClusterNominalCapacity = P_FLOAT("Nominal capacity (MW)", "cluster.nominal_capacity");
    pPGThClusterInstalled = P_FLOAT("Installed (MW)", "cluster.installed");
    pg->DisableProperty(pPGThClusterInstalled);
    pPGThClusterMustRun = P_BOOL("Must run", "cluster.must-run");

    pPGThClusterMinStablePower = P_FLOAT("Min Stable Power (MW)", "cluster.minstablepower");
    pPGThClusterMinUpTime = page->Append(
      new wxEnumProperty(wxT("Min. Up Time"), wxT("cluster.minuptime"), arrayMinUpDownTime));
    pPGThClusterMinDownTime = page->Append(
      new wxEnumProperty(wxT("Min. Down Time"), wxT("cluster.mindowntime"), arrayMinUpDownTime));

    pPGThClusterSpinning = P_FLOAT("Spinning (%)", "cluster.spinning");
    pPGThClusterSpinning->SetAttribute(wxPG_ATTR_MAX, 99.99);
    pPGThClusterCO2 = P_FLOAT("CO2 (Tons/MWh)", "cluster.co2");

    pPGThClusterCosts = Category(pg, wxT("Operating costs"), wxT("cluster.costs"));
    pPGThClusterMarginalCost = P_FLOAT("Marginal (\u20AC/MWh)", "cluster.opcost_marginal");
    pPGThClusterFixedCost = P_FLOAT("Fixed (\u20AC/hour)", "cluster.opcost_fixed");
    pPGThClusterStartupCost = P_FLOAT("Startup (\u20AC/startup)", "cluster.opcost_startup");
    pPGThClusterOperatingCost = P_FLOAT("Market bid (\u20AC/MWh)", "cluster.opcost_marketbid");
    pPGThClusterRandomSpread = P_FLOAT("Spread (\u20AC/MWh)", "cluster.opcost_spread");

    pPGThClusterReliabilityModel
      = Category(pg, wxT("Timeseries generation"), wxT("cluster.reliabilitymodel"));
    pPGThClusterDoGenerateTS = P_ENUM("Generate timeseries", "cluster.gen-ts", localGenTS);
    pPGThClusterVolatilityForced = P_FLOAT("Volatility (forced)", "cluster.forcedVolatility");
    pPGThClusterVolatilityPlanned = P_FLOAT("Volatility (planned)", "cluster.plannedVolatility");
    pPGThClusterLawForced = P_ENUM("Law (forced)", "cluster.forcedlaw", thermalLaws);
    pPGThClusterLawPlanned = P_ENUM("Law (planned)", "cluster.plannedlaw", thermalLaws);

    // --- RENEWABLE CLUSTERS ---
    pPGRnClusterSeparator = Group(pg, wxEmptyString, wxEmptyString);
    Group(pg, wxT("RENEWABLE CLUSTERS"), wxT("rn-cluster.title"));
    pPGRnClusterGeneral = Category(pg, wxT("General"), wxT("rn-cluster.general"));
    pPGRnClusterName = P_STRING("Name", "rn-cluster.name");

    wxPGChoices RnGroupChoices;
    for (uint i = 0; i != arrayRnClusterGroupCount; ++i)
        RnGroupChoices.Add(arrayRnClusterGroup[i], i);
    pPGRnClusterGroup = page->Append(
      new wxEditEnumProperty(wxT("group"), wxT("rn-cluster.group"), RnGroupChoices, wxEmptyString));

    pPGRnClusterArea = P_STRING("area", "rn-cluster.area");
    pg->DisableProperty(pPGRnClusterArea);

    pPGRnClusterTSMode = P_ENUM("timeseries mode", "rn-cluster.ts_mode", renewableTSMode);

    pPGRnClusterParams = Category(pg, wxT("Operating parameters"), wxT("rn-cluster.params"));
    pPGRnClusterEnabled = P_BOOL("Enabled", "rn-cluster.enabled");
    pPGRnClusterUnitCount = P_UINT("Unit", "rn-cluster.unit");
    pPGRnClusterNominalCapacity = P_FLOAT("Nominal capacity (MW)", "rn-cluster.nominal_capacity");
    pPGRnClusterInstalled = P_FLOAT("Installed (MW)", "rn-cluster.installed");
    pg->DisableProperty(pPGRnClusterInstalled);

    // --- CONSTRAINT ---
    pPGConstraintSeparator = Group(pg, wxEmptyString, wxEmptyString);
    pPGConstraintTitle = Group(pg, wxT("1 CONSTRAINT"), wxT("constraint.title"));
    pPGConstraintGeneral = Category(pg, wxT("General"), wxT("constraint.general"));
    pPGConstraintName = P_STRING("Name", "constraint.name");
    pPGConstraintComments = P_STRING("Comments", "constraint.comments");
    pPGConstraintEnabled = P_BOOL("Enabled", "constraint.enabled");
    pPGConstraintType = P_STRING("Type", "constraint.type");

    // Sizer
    SetSizer(s);
    // User's notes
    if (pAllowAnyObject)
        Dispatcher::GUI::Post(this, &Frame::onLoadUserNotes);

    OnInspectorRefresh.connect(this, &Frame::onInternalRefresh);
}

Frame::~Frame()
{
    // detach all events connected to this frame
    destroyBoundEvents();

    // If the global inspector is this object, we must unlink it from the
    // main from
    if (this == gInspector)
    {
        detachFromTheMainForm();
        gInspector = nullptr;
    }
}

void Frame::onInternalRefresh(const void* sender)
{
    if (sender != this)
        delayApply();
}

void Frame::detachFromTheMainForm()
{
    auto* mainFrm = Antares::Forms::ApplWnd::Instance();
    assert(mainFrm and "Invalid main frame");
    if (mainFrm)
        mainFrm->AUIManager().DetachPane(this);
}

void Frame::attachToTheMainForm()
{
    auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
    mainFrm.AUIManager().AddPane(this);
    wxAuiPaneInfo& pane = mainFrm.AUIManager().GetPane(this);
    pane.Name(wxT("inspector"));
    pane.Caption(wxT("Inspector"));
    pane.MinSize(wxSize(280, 100));
    pane.MaxSize(wxSize(500, 2000));
    // pane.Resizable(true);
    pane.CaptionVisible(true);
    // pane.Gripper(true);
    pane.CloseButton(true);
    pane.PinButton(true);
    pane.BottomDockable().LeftDockable().RightDockable();
    pane.Floatable();
    pane.Movable();
    pane.Right();
    pane.BestSize(280, 400);
    mainFrm.AUIManager().Update();
}

void Frame::onSelectProperties(void*)
{
    if (gInspector)
    {
        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        wxAuiPaneInfo& pnl = mainFrm.AUIManager().GetPane(gInspector);
        pnl.Caption(wxT("Inspector"));

        wxSizer* sizer = pMainPanel->GetSizer();
        if (pNotes)
            sizer->Show(pNotes, false, false);
        sizer->Show(pPropertyGrid, true, false);
        sizer->Layout();
        mainFrm.AUIManager().Update();
    }
}

void Frame::onSelectNotes(void*)
{
    if (gInspector)
    {
        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        wxAuiPaneInfo& pnl = mainFrm.AUIManager().GetPane(gInspector);
        pnl.Caption(wxT("Notes"));

        if (pNotes)
            pMainPanel->GetSizer()->Show(pNotes, true, false);
        pMainPanel->GetSizer()->Show(pPropertyGrid, false, false);
        pMainPanel->GetSizer()->Layout();
        mainFrm.AUIManager().Update();
    }
}

void Frame::apply(const InspectorData::Ptr& data)
{
#define PROPERTY(X) (pPropertyGrid->GetPropertyByName(wxT(X)))
    if (pAllowAnyObject and not IsShown())
        return;

    // Temporary Revoke the current selection in the property grid
    // to prevent any changes
    pApplyPropertyGrid(nullptr);
    // Keeping a reference to this selection
    pCurrentSelection = data;
    // Avoid unnecessary refresh
    wxWindowUpdateLocker updater(this);

    if (pBtnInspector)
    {
        // Updating the UI
        uint totalNbItems = (!(!data)) ? data->totalNbOfItems() : 0;
        switch (totalNbItems)
        {
        case 0:
            pBtnInspector->caption(wxT("No item"));
            break;
        case 1:
            pBtnInspector->caption(wxT("1 item"));
            break;
        default:
            pBtnInspector->caption(wxString() << totalNbItems << wxT(" items"));
        }
    }

    wxSizer* sizer = GetSizer();

    auto study = Data::Study::Current::Get();
    wxPGProperty* p;
    bool hide;
    bool multiple;

    if (pAllowAnyObject and study)
    {
        if (pPGCommonStudyName and pPGCommonStudyAuthor)
        {
            pPGCommonStudyName->SetValueFromString(wxStringFromUTF8(study->header.caption));
            pPGCommonStudyAuthor->SetValueFromString(wxStringFromUTF8(study->header.author));
        }
    }
    else
    {
        // ugly hack : to allow self refresh when several properties
        // are modified in the same time
        if (not pAlreadyConnectedToSimulationChangesEvent)
        {
            OnStudySimulationSettingsChanged.connect(this, &Frame::delayApply);
            pAlreadyConnectedToSimulationChangesEvent = true;
        }
    }

    // STUDIES
    hide = !data || data->studies.empty();
    multiple = (data and data->studies.size() > 1);
    pPGStudyTitle->Hide(hide);

    // -------
    // STUDIES
    // -------
    pPGStudySpace1->Hide(hide);
    pPGStudySpace2->Hide(hide);
    pPGStudyGrpSimulation->Hide(hide);
    pPGStudyGrpCalendar->Hide(hide);
    pPGStudyMCScenarios->Hide(hide);
    pPGStudyOutputProfile->Hide(hide);
    if (!hide)
    {
        // Context
        Accumulator<PStudyMode>::Apply(pPGStudyMode, data->studies);
        Accumulator<PStudyHorizon>::Apply(pPGStudyHorizon, data->studies);
        Accumulator<PStudyCalendarMonth>::Apply(pPGStudyCalendarMonth, data->studies);
        Accumulator<PStudyCalendarWeek>::Apply(pPGStudyCalendarWeek, data->studies);
        Accumulator<PStudyLeapYear>::Apply(pPGStudyLeapYear, data->studies);
        Accumulator<PStudy1stJanuary>::Apply(pPGStudy1rstJanuary, data->studies);
        Accumulator<PStudyCalBegin>::Apply(pPGStudyCalendarBegin, data->studies);
        Accumulator<PStudyCalEnd>::Apply(pPGStudyCalendarEnd, data->studies);
        // Monte-Carlo scenarios
        Accumulator<PStudyYears>::Apply(pPGStudyYears, data->studies);
        Accumulator<PStudyBuildingMode>::Apply(pPGStudyBuildMode, data->studies);
        Accumulator<PStudyPlaylist>::Apply(pPGStudyPlaylist, data->studies);
        // Output profile
        Accumulator<PStudySynthesis>::Apply(pPGStudySimulationSynthesis, data->studies);
        Accumulator<PStudyYearByYear>::Apply(pPGStudyYearByYear, data->studies);
        Accumulator<PStudyGeographicTrimming>::Apply(pPGGeographicTrimming, data->studies);
        Accumulator<PStudyThematicTrimming>::Apply(pPGThematicTrimming, data->studies);
        Accumulator<PStudyMCScenarios>::Apply(pPGStudyUseMCScenarios, data->studies);
    }

    // AREAS
    hide = !data || data->areas.empty();
    multiple = (data and data->areas.size() > 1);
    pPGAreaTitle->Hide(hide);
    if (!hide)
    {
        if (!multiple)
            pPGAreaTitle->SetLabel(wxT("AREA"));
        else
            pPGAreaTitle->SetLabel(wxString() << data->areas.size() << wxT(" AREAS"));
    }

    // -----
    // AREAS
    // -----
    pPGAreaSeparator->Hide(hide);
    pPGAreaGeneral->Hide(hide);
    pPGAreaFilteringStatus->Hide(!(!hide and study and study->parameters.geographicTrimming));
    if (!hide)
    {
        pPGAreaName->Hide(multiple);
        // Name of the area
        if (!multiple)
            pPGAreaName->SetValueFromString(wxStringFromUTF8((*(data->areas.begin()))->name));
        // Area color
        Accumulator<PAreaColor>::Apply(pPGAreaColor, data->areas);
        // Adequacy patch
        Accumulator<PAdequacyPatchMode>::Apply(pPGAreaAdequacyPatchMode, data->areas);
        // Area position
        if (!multiple)
        {
            PROPERTY("area.x")->SetValueFromInt((*(data->areas.begin()))->ui->x);
            PROPERTY("area.y")->SetValueFromInt((*(data->areas.begin()))->ui->y);
        }
        int nbLinks = 0;
        int nbThermalClusters = 0;
        const auto areaEnd = data->areas.end();
        for (auto i = data->areas.begin(); i != areaEnd; ++i)
        {
            const Data::Area& area = *(*i);
            nbLinks += (uint)area.links.size();
            nbThermalClusters += area.thermal.list.size();
        }
        pPGAreaLinks->SetValueFromInt(nbLinks);
        pPGAreaPlants->SetValueFromInt(nbThermalClusters);
        Accumulator<PAreaUnsuppliedEnergyCost>::Apply(pPGUnsupplied, data->areas);
        Accumulator<PAreaSpilledEnergyCost>::Apply(pPGSpilled, data->areas);
        Accumulator<PAreaResortStatus<Data::anoNonDispatchPower>>::Apply(pPGAreaResortNon,
                                                                         data->areas);
        Accumulator<PAreaResortStatus<Data::anoDispatchHydroPower>>::Apply(pPGAreaResortHydroPower,
                                                                           data->areas);
        Accumulator<PAreaResortStatus<Data::anoOtherDispatchPower>>::Apply(pPGAreaResortOther,
                                                                           data->areas);

        Accumulator<PAreaFiltering<true, Data::filterHourly>>::Apply(pPGAreaFilteringSynthesis[0],
                                                                     data->areas);
        Accumulator<PAreaFiltering<true, Data::filterDaily>>::Apply(pPGAreaFilteringSynthesis[1],
                                                                    data->areas);
        Accumulator<PAreaFiltering<true, Data::filterWeekly>>::Apply(pPGAreaFilteringSynthesis[2],
                                                                     data->areas);
        Accumulator<PAreaFiltering<true, Data::filterMonthly>>::Apply(pPGAreaFilteringSynthesis[3],
                                                                      data->areas);
        Accumulator<PAreaFiltering<true, Data::filterAnnual>>::Apply(pPGAreaFilteringSynthesis[4],
                                                                     data->areas);

        Accumulator<PAreaFiltering<false, Data::filterHourly>>::Apply(pPGAreaFilteringYbY[0],
                                                                      data->areas);
        Accumulator<PAreaFiltering<false, Data::filterDaily>>::Apply(pPGAreaFilteringYbY[1],
                                                                     data->areas);
        Accumulator<PAreaFiltering<false, Data::filterWeekly>>::Apply(pPGAreaFilteringYbY[2],
                                                                      data->areas);
        Accumulator<PAreaFiltering<false, Data::filterMonthly>>::Apply(pPGAreaFilteringYbY[3],
                                                                       data->areas);
        Accumulator<PAreaFiltering<false, Data::filterAnnual>>::Apply(pPGAreaFilteringYbY[4],
                                                                      data->areas);
    }
    pPGAreaOptimization->Hide(hide);
    pPGAreaResort->Hide(hide);
    pPGAreaAdequacyPatchTitle->Hide(hide);
    pPGAreaLocalization->Hide(hide || multiple);
    pPGAreaDeps->Hide(hide);

    // -----
    // LINKS
    // -----
    hide = !data || data->links.empty();
    multiple = (data and data->links.size() > 1);
    pPGLinkSeparator->Hide(hide);
    pPGLinkFilteringStatus->Hide(!(!hide and study and study->parameters.geographicTrimming));
    p = PROPERTY("link.title");
    p->Hide(hide);
    if (!hide)
    {
        if (!multiple)
            p->SetLabel(wxT("LINK"));
        else
            p->SetLabel(wxString() << data->links.size() << wxT(" LINKS"));
        Accumulator<PLinkArea<true>>::Apply(PROPERTY("link.from"), data->links);
        Accumulator<PLinkArea<false>>::Apply(PROPERTY("link.to"), data->links);
        // Accumulator<PLinkCopperPlate>    ::Apply(PROPERTY("link.transmission-capacities"),
        // data->links);
        Accumulator<PLinkHurdlesCost>::Apply(PROPERTY("link.hurdles_cost"), data->links);
        Accumulator<PLinkLoopFlow>::Apply(PROPERTY("link.use_loop_flow"), data->links);
        Accumulator<PLinkPhaseShift>::Apply(PROPERTY("link.use_phase_shifter"), data->links);
        Accumulator<PLinkComments>::Apply(PROPERTY("link.comments"), data->links);
        // Accumulator<PLinkColor>			 ::Apply(PROPERTY("link.color"),
        // data->links);
        Accumulator<PLinkColor>::Apply(pPGLinkColor, data->links);
        Accumulator<PLinkStyle>::Apply(pPGLinkStyle, data->links);
        Accumulator<PLinkWidth>::Apply(pPGLinkWidth, data->links);
        Accumulator<PLinkDisplayComments>::Apply(PROPERTY("link.display_comments"), data->links);

        Accumulator<PLinkFiltering<true, Data::filterHourly>>::Apply(pPGLinkFilteringSynthesis[0],
                                                                     data->links);
        Accumulator<PLinkFiltering<true, Data::filterDaily>>::Apply(pPGLinkFilteringSynthesis[1],
                                                                    data->links);
        Accumulator<PLinkFiltering<true, Data::filterWeekly>>::Apply(pPGLinkFilteringSynthesis[2],
                                                                     data->links);
        Accumulator<PLinkFiltering<true, Data::filterMonthly>>::Apply(pPGLinkFilteringSynthesis[3],
                                                                      data->links);
        Accumulator<PLinkFiltering<true, Data::filterAnnual>>::Apply(pPGLinkFilteringSynthesis[4],
                                                                     data->links);

        Accumulator<PLinkFiltering<false, Data::filterHourly>>::Apply(pPGLinkFilteringYbY[0],
                                                                      data->links);
        Accumulator<PLinkFiltering<false, Data::filterDaily>>::Apply(pPGLinkFilteringYbY[1],
                                                                     data->links);
        Accumulator<PLinkFiltering<false, Data::filterWeekly>>::Apply(pPGLinkFilteringYbY[2],
                                                                      data->links);
        Accumulator<PLinkFiltering<false, Data::filterMonthly>>::Apply(pPGLinkFilteringYbY[3],
                                                                       data->links);
        Accumulator<PLinkFiltering<false, Data::filterAnnual>>::Apply(pPGLinkFilteringYbY[4],
                                                                      data->links);
    }
    PROPERTY("link.connection")->Hide(hide);
    PROPERTY("link.params")->Hide(hide);
    PROPERTY("link.coms")->Hide(hide);
    PROPERTY("link.styles")->Hide(hide);

    // ----------------
    // THERMAL CLUSTERS
    // ----------------
    hide = !data || data->ThClusters.empty();
    multiple = (data and data->ThClusters.size() > 1);
    pPGThClusterSeparator->Hide(hide);
    p = PROPERTY("cluster.title");
    p->Hide(hide);
    if (!hide)
    {
        pPGThClusterName->Hide(multiple);
        if (!multiple) // only one thermal cluster
        {
            p->SetLabel(wxT("THERMAL CLUSTER"));
            pPGThClusterName->SetValueFromString(
              wxStringFromUTF8((*(data->ThClusters.begin()))->name()));
        }
        else
            p->SetLabel(wxString() << data->ThClusters.size() << wxT(" THERMAL CLUSTERS"));

        // Parent Area
        Accumulator<PClusterArea>::Apply(pPGThClusterArea, data->ThClusters);
        // Group
        Accumulator<PClusterGroup>::Apply(pPGThClusterGroup, data->ThClusters);
        // Must run
        Accumulator<PClusterMustRun>::Apply(pPGThClusterMustRun, data->ThClusters);
        // Enabled
        Accumulator<PClusterEnabled>::Apply(pPGThClusterEnabled, data->ThClusters);
        // Unit count
        Accumulator<PClusterUnitCount>::Apply(pPGThClusterUnitCount, data->ThClusters);
        // Nominal capacity
        Accumulator<PClusterNomCapacity>::Apply(pPGThClusterNominalCapacity, data->ThClusters);
        // Installed
        Accumulator<PClusterInstalled, Add>::Apply(pPGThClusterInstalled, data->ThClusters);
        // Min. Up Time
        Accumulator<PClusterMinUpTime>::Apply(pPGThClusterMinUpTime, data->ThClusters);
        // Min. Down Time
        Accumulator<PClusterMinDownTime>::Apply(pPGThClusterMinDownTime, data->ThClusters);
        // Min. Stable Power
        Accumulator<PClusterMinStablePower>::Apply(pPGThClusterMinStablePower, data->ThClusters);
        // Spinning
        Accumulator<PClusterSpinning>::Apply(pPGThClusterSpinning, data->ThClusters);
        // CO2
        Accumulator<PClusterCO2>::Apply(pPGThClusterCO2, data->ThClusters);
        // Volatility
        Accumulator<PClusterVolatilityPlanned>::Apply(pPGThClusterVolatilityPlanned,
                                                      data->ThClusters);
        Accumulator<PClusterVolatilityForced>::Apply(pPGThClusterVolatilityForced,
                                                     data->ThClusters);
        // Laws
        Accumulator<PClusterLawPlanned>::Apply(pPGThClusterLawPlanned, data->ThClusters);
        Accumulator<PClusterLawForced>::Apply(pPGThClusterLawForced, data->ThClusters);
        // Costs
        Accumulator<PClusterMarginalCost>::Apply(pPGThClusterMarginalCost, data->ThClusters);
        Accumulator<PClusterReference>::Apply(pPGThClusterOperatingCost, data->ThClusters);
        Accumulator<PClusterFixedCost>::Apply(pPGThClusterFixedCost, data->ThClusters);
        Accumulator<PClusterStartupCost>::Apply(pPGThClusterStartupCost, data->ThClusters);
        Accumulator<PClusterRandomSpread>::Apply(pPGThClusterRandomSpread, data->ThClusters);
        // Override global TS generation setting, per cluster
        Accumulator<PClusterDoGenerateTS>::Apply(pPGThClusterDoGenerateTS, data->ThClusters);

        // check Nominal capacity with thermal modulation
        AccumulatorCheck<PClusterNomCapacityColor>::ApplyTextColor(pPGThClusterNominalCapacity,
                                                                   data->ThClusters);
        // check Min. Stable Power with thermal modulation
        AccumulatorCheck<PClusterMinStablePowerColor>::ApplyTextColor(pPGThClusterMinStablePower,
                                                                      data->ThClusters);
        // check Min. Stable Power with thermal modulation
        AccumulatorCheck<PClusterSpinningColor>::ApplyTextColor(pPGThClusterSpinning,
                                                                data->ThClusters);
    }

    pPGThClusterParams->Hide(hide);
    pPGThClusterReliabilityModel->Hide(hide);
    pPGThClusterCosts->Hide(hide);
    pPGThClusterGeneral->Hide(hide);

    // --------------------
    // RENEWABLE CLUSTERS
    // --------------------
    hide = !data || data->RnClusters.empty();
    multiple = (data and data->RnClusters.size() > 1);
    pPGRnClusterSeparator->Hide(hide);
    p = PROPERTY("rn-cluster.title");
    p->Hide(hide);
    if (!hide)
    {
        pPGRnClusterName->Hide(multiple);
        if (!multiple) // only one renewable cluster
        {
            p->SetLabel(wxT("RENEWABLE CLUSTER"));
            pPGRnClusterName->SetValueFromString(
              wxStringFromUTF8((*(data->RnClusters.begin()))->name()));
        }
        else
            p->SetLabel(wxString() << data->RnClusters.size() << wxT(" RENEWABLE CLUSTERS"));

        // Parent Area
        Accumulator<PClusterArea>::Apply(pPGRnClusterArea, data->RnClusters);
        // Group
        Accumulator<PClusterGroup>::Apply(pPGRnClusterGroup, data->RnClusters);
        // TS mode
        Accumulator<PRnClusterTSMode>::Apply(pPGRnClusterTSMode, data->RnClusters);
        // Enabled
        Accumulator<PClusterEnabled>::Apply(pPGRnClusterEnabled, data->RnClusters);
        // Unit count
        Accumulator<PClusterUnitCount>::Apply(pPGRnClusterUnitCount, data->RnClusters);
        // Nominal capacity
        Accumulator<PClusterNomCapacity>::Apply(pPGRnClusterNominalCapacity, data->RnClusters);
        // Installed capacity
        Accumulator<PClusterInstalled, Add>::Apply(pPGRnClusterInstalled, data->RnClusters);
        // gp : what should we do with that ?
        // check Nominal capacity with thermal modulation
        // AccumulatorCheck<PClusterNomCapacityColor>::ApplyTextColor(pPGRnClusterNominalCapacity,
        //    data->RnClusters);
    }

    pPGRnClusterParams->Hide(hide);
    pPGRnClusterGeneral->Hide(hide);

    // -----------
    // CONSTRAINTS
    // -----------
    hide = !data || data->constraints.empty();
    multiple = (data and data->constraints.size() > 1);
    pPGConstraintSeparator->Hide(hide);
    p = pPGConstraintTitle;
    p->Hide(hide);
    if (!hide)
    {
        pPGConstraintName->Hide(multiple);
        if (!multiple) // only one thermal cluster
        {
            p->SetLabel(wxT("CONSTRAINT"));
            pPGConstraintName->SetValueFromString(
              wxStringFromUTF8((*(data->constraints.begin()))->name()));
        }
        else
        {
            p->SetLabel(wxString() << data->constraints.size() << wxT(" CONSTRAINTS"));
        }

        Accumulator<PConstraintComments>::Apply(PROPERTY("constraint.comments"), data->constraints);
        Accumulator<PConstraintEnabled>::Apply(PROPERTY("constraint.enabled"), data->constraints);
        Accumulator<PConstraintType>::Apply(PROPERTY("constraint.type"), data->constraints);
    }
    pPGConstraintGeneral->Hide(hide);

    // Rebuild the layout
    sizer->Layout();
#undef PROPERTY

    // inform the property grid that a selection set has arrived
    pApplyPropertyGrid(pCurrentSelection);
}

void Frame::onLoadUserNotes()
{
    if (pNotes and Data::Study::Current::Valid())
        pNotes->loadFromStudy();
}

} // namespace Inspector
} // namespace Window
} // namespace Antares
