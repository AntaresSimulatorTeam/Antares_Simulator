/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "optimization.h"
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include "../../../toolbox/create.h"
#include "../../../toolbox/resources.h"
#include "../../../application/study.h"
#include "../../../windows/message.h"
#include <ui/common/component/panel.h>
#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Options
{
static void ResetButton(Component::Button* button, bool value)
{
    assert(button != NULL);
    if (value)
    {
        button->image("images/16x16/light_green.png");
        button->caption(wxT("local values"));
    }
    else
    {
        button->image("images/16x16/light_orange.png");
        button->caption(wxT("ignore"));
    }
}

static void ResetButton(Component::Button* button, Data::GlobalTransmissionCapacities value)
{
    assert(button != NULL);
    button->image(transmissionCapacityIcon(value));
    button->caption(GlobalTransmissionCapacitiesToString_Display(value));
}

const char* mpsExportIcon(const Data::mpsExportStatus& mps_export_status)
{
    switch (mps_export_status)
    {
    case Data::mpsExportStatus::NO_EXPORT:
        return "images/16x16/light_orange.png";
    case Data::mpsExportStatus::EXPORT_FIRST_OPTIM:
        return "images/16x16/light_green.png";
    case Data::mpsExportStatus::EXPORT_SECOND_OPTIM:
        return "images/16x16/light_green.png";
    case Data::mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return "images/16x16/light_green.png";
    default:
        return "images/16x16/light_orange.png";
    }
}
const char* transmissionCapacityIcon(Data::GlobalTransmissionCapacities capacity)
{
    using GTransmission = Data::GlobalTransmissionCapacities;
    switch (capacity)
    {
    case GTransmission::localValuesForAllLinks:
        return "images/16x16/light_green.png";
    case GTransmission::infiniteForAllLinks:
    case GTransmission::infiniteForPhysicalLinks:
        return "images/16x16/infinity.png";
    default:
        return "images/16x16/light_orange.png";
    }
}

Optimization::Optimization(wxWindow* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Optimization preferences"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN),
 pTargetRef(nullptr)
{
    assert(parent);

    // Title of the Form
    SetLabel(wxT("Optimization preferences"));
    SetTitle(wxT("Optimization preferences"));

    auto& study = *GetCurrentStudy();

    // Background color
    wxColour defaultBgColor = GetBackgroundColour();
    wxColour bgColor(255, 255, 255);
    SetBackgroundColour(bgColor);

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* hz = new wxBoxSizer(wxHORIZONTAL);
    hz->AddSpacer(6);
    hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/64x64/db.png"),
            0,
            wxALL | wxALIGN_TOP | wxALIGN_CENTER);
    hz->AddSpacer(35);

    auto* s = new wxFlexGridSizer(0, 2, 1, 10);
    s->AddGrowableCol(0, 1);
    hz->Add(s, 1, wxALL | wxEXPAND);
    sizer->Add(hz, 0, wxALL | wxEXPAND, 20);

    wxStaticText* label;
    Component::Button* button;
    Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)> onPopup;

    // Binding constraints
    {
        label = Component::CreateLabel(this, wxT("Binding constraints"));
        button = new Component::Button(this, wxT("local values"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &Optimization::onPopupMenu,
                     PopupInfo(study.parameters.include.constraints, wxT("binding constraints")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnConstraints = button;
    }
    // Hurdle costs
    {
        label = Component::CreateLabel(this, wxT("Hurdle costs"));
        button = new Component::Button(this, wxT("local values"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &Optimization::onPopupMenu,
                     PopupInfo(study.parameters.include.hurdleCosts, wxT("hurdle costs")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnHurdleCosts = button;
    }
    // Transmission capacities
    {
        label = Component::CreateLabel(this, wxT("Transmission capacities"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &Optimization::onPopupMenuTransmissionCapacities);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnTransmissionCapacities = button;
    }
    // Thermal clusters Min Stable Power
    {
        label = Component::CreateLabel(this, wxT("Thermal clusters Min Stable power"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &Optimization::onPopupMenu,
                     PopupInfo(study.parameters.include.thermal.minStablePower,
                               wxT("thermal cluster min stable power")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnThermalClusterMinStablePower = button;
    }
    // Thermal clusters Min U/D time
    {
        label = Component::CreateLabel(this, wxT("Thermal clusters Min U/D time"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &Optimization::onPopupMenu,
                     PopupInfo(study.parameters.include.thermal.minUPTime,
                               wxT("thermal cluster min U/D time")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnThermalClusterMinUPTime = button;
    }
    // Day ahead reserve
    {
        label = Component::CreateLabel(this, wxT("Day ahead Reserve"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(
          this,
          &Optimization::onPopupMenu,
          PopupInfo(study.parameters.include.reserve.dayAhead, wxT("day ahead reserve")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnDayAheadReserve = button;
    }
    // Primary reserve
    {
        label = Component::CreateLabel(this, wxT("Primary reserve"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &Optimization::onPopupMenu,
                     PopupInfo(study.parameters.include.reserve.primary, wxT("primary reserve")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnPrimaryReserve = button;
    }

    // Strategic reserve
    {
        label = Component::CreateLabel(this, wxT("Strategic reserve"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(
          this,
          &Optimization::onPopupMenu,
          PopupInfo(study.parameters.include.reserve.strategic, wxT("strategic reserve")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnStrategicReserve = button;
    }
    // Spinning reserve
    {
        label = Component::CreateLabel(this, wxT("Spinning reserve"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &Optimization::onPopupMenu,
                     PopupInfo(study.parameters.include.reserve.spinning, wxT("spinning reserve")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnSpinningReserve = button;
    }
    // Export MPS
    {
        label = Component::CreateLabel(this, wxT("Export mps"));

        const Data::mpsExportStatus& defaultValue = Data::mpsExportStatus::NO_EXPORT;
        button = new Component::Button(
          this, mpsExportStatusToString(defaultValue), mpsExportIcon(defaultValue));

        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &Optimization::onPopupMenuExportMPSstatus);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnExportMPS = button;
    }
    // Unfeasible problem behavior
    {
        label = Component::CreateLabel(this, wxT("Unfeasible problem behavior"));

        const Data::UnfeasibleProblemBehavior& defaultValue
          = Data::UnfeasibleProblemBehavior::ERROR_DRY;
        button = new Component::Button(
          this, Data::getDisplayName(defaultValue), Data::getIcon(defaultValue));
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &Optimization::onPopupMenuUnfeasibleBehavior);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnUnfeasibleProblemBehavior = button;
    }

    // Simplex optimization range
    {
        label = Component::CreateLabel(this, wxT("Simplex optimization range"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/calendar_day.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        button->onPopupMenu(this, &Optimization::onPopupMenuSimplex);

        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnSimplexOptimizationRange = button;
    }
    if (0)
    {
        label = Component::CreateLabel(
          this, wxT("Weekly binding constraints will be ignored"), false, false, -1);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->AddSpacer(2);
    }
    {
        s->AddSpacer(2);
        s->Add(105, 2);
    }

    sizer->AddSpacer(10);

    // Buttons
    Component::Panel* panel = new Component::Panel(this);
    panel->SetBackgroundColour(defaultBgColor);
    wxBoxSizer* pnlSizerBtns = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pnlSizerBtnsV = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(pnlSizerBtnsV);
    pnlSizerBtnsV->AddSpacer(8);
    pnlSizerBtnsV->Add(pnlSizerBtns, 1, wxALL | wxEXPAND);
    pnlSizerBtnsV->AddSpacer(8);

    button = new Component::Button(panel,
                                   wxT("Reset to default"),
                                   "images/16x16/param_reset.png",
                                   this,
                                   &Optimization::onResetToDefault);
    pnlSizerBtns->Add(22, 5);
    pnlSizerBtns->Add(button, 0, wxALL | wxEXPAND);

    pnlSizerBtns->AddStretchSpacer();
    wxButton* pBtnCancel
      = Antares::Component::CreateButton(panel, wxT("  Close  "), this, &Optimization::onClose);
    pBtnCancel->SetDefault();
    pnlSizerBtns->Add(pBtnCancel, 0, wxALL | wxEXPAND);
    pnlSizerBtns->Add(25, 5);

    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND);
    sizer->Add(panel, 0, wxALL | wxEXPAND);

    // refresh
    Connect(GetId(), wxEVT_MOTION, wxMouseEventHandler(Optimization::onInternalMotion), NULL, this);

    refresh();
    SetSizer(sizer);
    sizer->Layout();

    // Recentre the window
    sizer->Fit(this);
    Centre(wxBOTH);
}

Optimization::~Optimization()
{
}

void Optimization::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

void Optimization::onResetToDefault(void*)
{
    auto studyptr = GetCurrentStudy();
    if (!(!studyptr))
    {
        Window::Message message(this,
                                wxT("Optimization preferences"),
                                wxT("Optimization preferences"),
                                wxT("Do you really want to reset to the default values ?"));
        message.add(Window::Message::btnContinue);
        message.add(Window::Message::btnCancel, true);

        if (message.showModal() == Window::Message::btnContinue)
        {
            auto& study = *studyptr;
            study.parameters.include.constraints = true;
            study.parameters.include.hurdleCosts = true;
            study.parameters.transmissionCapacities
              = Data::GlobalTransmissionCapacities::localValuesForAllLinks;
            study.parameters.include.thermal.minStablePower = true;
            study.parameters.include.thermal.minUPTime = true;
            study.parameters.include.reserve.dayAhead = true;
            study.parameters.include.reserve.strategic = true;
            study.parameters.include.reserve.primary = true;
            study.parameters.include.reserve.spinning = true;
            study.parameters.include.exportMPS = Data::mpsExportStatus::NO_EXPORT;
            study.parameters.simplexOptimizationRange = Data::sorWeek;
            study.parameters.include.unfeasibleProblemBehavior
              = Data::UnfeasibleProblemBehavior::ERROR_MPS;

            refresh();
            MarkTheStudyAsModified();
            return;
        }
    }
}

void Optimization::onInternalMotion(wxMouseEvent&)
{
    Component::Panel::OnMouseMoveFromExternalComponent();
}

void Optimization::refresh()
{
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return;
    // The current study
    auto& study = *studyptr;

    // Constraints
    ResetButton(pBtnConstraints, study.parameters.include.constraints);
    // Hurdle costs
    ResetButton(pBtnHurdleCosts, study.parameters.include.hurdleCosts);
    // Transmission capacities
    ResetButton(pBtnTransmissionCapacities, study.parameters.transmissionCapacities);
    // Min Stable power
    ResetButton(pBtnThermalClusterMinStablePower, study.parameters.include.thermal.minStablePower);
    // Min U/D time
    ResetButton(pBtnThermalClusterMinUPTime, study.parameters.include.thermal.minUPTime);
    // Day ahead reserve
    ResetButton(pBtnDayAheadReserve, study.parameters.include.reserve.dayAhead);
    // Primary reserve
    ResetButton(pBtnPrimaryReserve, study.parameters.include.reserve.primary);
    // Strategic reserve
    ResetButton(pBtnStrategicReserve, study.parameters.include.reserve.strategic);
    // Spinning reserve
    ResetButton(pBtnSpinningReserve, study.parameters.include.reserve.spinning);
    // Export mps
    pBtnExportMPS->image(mpsExportIcon(study.parameters.include.exportMPS));
    pBtnExportMPS->caption(Data::mpsExportStatusToString(study.parameters.include.exportMPS));

    // Unfeasible problem behavior
    pBtnUnfeasibleProblemBehavior->image(
      Data::getIcon(study.parameters.include.unfeasibleProblemBehavior));
    pBtnUnfeasibleProblemBehavior->caption(
      Data::getDisplayName(study.parameters.include.unfeasibleProblemBehavior));

    // Simplex Optimization Range
    if (Data::sorDay == study.parameters.simplexOptimizationRange)
    {
        pBtnSimplexOptimizationRange->image("images/16x16/calendar_day.png");
        pBtnSimplexOptimizationRange->caption(wxT("Day"));
    }
    else
    {
        pBtnSimplexOptimizationRange->image("images/16x16/calendar_week.png");
        pBtnSimplexOptimizationRange->caption(wxT("Week"));
    }
}

void Optimization::onPopupMenu(Component::Button&, wxMenu& menu, void*, const PopupInfo& info)
{
    pTargetRef = &info.rval;
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxString() << wxT("local values (") << info.text << wxT(")"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectModeInclude),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("ignore"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectModeIgnore),
                 nullptr,
                 this);
}

void Optimization::onPopupMenuSpecify(Component::Button&,
                                      wxMenu& menu,
                                      void*,
                                      const PopupInfo& info)
{
    pTargetRef = &info.rval;
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxString() << info.text, "images/16x16/light_green.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectModeInclude),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("false"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectModeIgnore),
                 nullptr,
                 this);
}

void Optimization::onPopupMenuSimplex(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Day"), "images/16x16/calendar_day.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectSimplexDay),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Week"), "images/16x16/calendar_week.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Optimization::onSelectSimplexWeek),
                 nullptr,
                 this);
}

void Optimization::onPopupMenuTransmissionCapacities(Component::Button&, wxMenu& menu, void*)
{
    using GT = Data::GlobalTransmissionCapacities;
    createGlobalTransmissionCapacitiesItemIntoMenu<GT::localValuesForAllLinks>(menu);
    createGlobalTransmissionCapacitiesItemIntoMenu<GT::nullForAllLinks>(menu);
    createGlobalTransmissionCapacitiesItemIntoMenu<GT::infiniteForAllLinks>(menu);
    createGlobalTransmissionCapacitiesItemIntoMenu<GT::nullForPhysicalLinks>(menu);
    createGlobalTransmissionCapacitiesItemIntoMenu<GT::infiniteForPhysicalLinks>(menu);
}

void Optimization::onPopupMenuExportMPSstatus(Component::Button&, wxMenu& menu, void*)
{
    using MPS = Data::mpsExportStatus;
    createMPSexportItemIntoMenu<MPS::NO_EXPORT>(menu);
    createMPSexportItemIntoMenu<MPS::EXPORT_FIRST_OPTIM>(menu);
    createMPSexportItemIntoMenu<MPS::EXPORT_SECOND_OPTIM>(menu);
    createMPSexportItemIntoMenu<MPS::EXPORT_BOTH_OPTIMS>(menu);
}

void Optimization::onPopupMenuUnfeasibleBehavior(Component::Button&, wxMenu& menu, void*)
{
    // Warning dry
    {
        const Data::UnfeasibleProblemBehavior& value = Data::UnfeasibleProblemBehavior::WARNING_DRY;
        wxMenuItem* it = Menu::CreateItem(
          &menu, wxID_ANY, Data::getDisplayName(value), Data::getIcon(value), wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Optimization::onSelectUnfeasibleBehaviorWarningDry),
                     nullptr,
                     this);
    }

    // Warning mps
    {
        const Data::UnfeasibleProblemBehavior& value = Data::UnfeasibleProblemBehavior::WARNING_MPS;
        wxMenuItem* it = Menu::CreateItem(
          &menu, wxID_ANY, Data::getDisplayName(value), Data::getIcon(value), wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Optimization::onSelectUnfeasibleBehaviorWarningMps),
                     nullptr,
                     this);
    }
    // Error dry
    {
        const Data::UnfeasibleProblemBehavior& value = Data::UnfeasibleProblemBehavior::ERROR_DRY;
        wxMenuItem* it = Menu::CreateItem(
          &menu, wxID_ANY, Data::getDisplayName(value), Data::getIcon(value), wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Optimization::onSelectUnfeasibleBehaviorErrorDry),
                     nullptr,
                     this);
    }

    // Error mps
    {
        const Data::UnfeasibleProblemBehavior& value = Data::UnfeasibleProblemBehavior::ERROR_MPS;
        wxMenuItem* it = Menu::CreateItem(
          &menu, wxID_ANY, Data::getDisplayName(value), Data::getIcon(value), wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Optimization::onSelectUnfeasibleBehaviorErrorMps),
                     nullptr,
                     this);
    }
}

void Optimization::onSelectModeInclude(wxCommandEvent&)
{
    if (pTargetRef and !*pTargetRef)
    {
        *pTargetRef = true;
        MarkTheStudyAsModified();
        refresh();
    }
    pTargetRef = nullptr;
}

void Optimization::onSelectModeIgnore(wxCommandEvent&)
{
    if (pTargetRef and *pTargetRef)
    {
        *pTargetRef = false;
        MarkTheStudyAsModified();
        refresh();
    }
    pTargetRef = nullptr;
}

void Optimization::onSelectSimplexDay(wxCommandEvent&)
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        if (study->parameters.simplexOptimizationRange != Data::sorDay)
        {
            study->parameters.simplexOptimizationRange = Data::sorDay;
            refresh();
            MarkTheStudyAsModified();
        }
    }
}

void Optimization::onSelectSimplexWeek(wxCommandEvent&)
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        if (study->parameters.simplexOptimizationRange != Data::sorWeek)
        {
            study->parameters.simplexOptimizationRange = Data::sorWeek;
            refresh();
            MarkTheStudyAsModified();
        }
    }
}

void Optimization::setTransmissionCapacity(Data::GlobalTransmissionCapacities newCapacity)
{
    auto study = GetCurrentStudy();
    if (study && study->parameters.transmissionCapacities != newCapacity)
    {
        study->parameters.transmissionCapacities = newCapacity;
        refresh();
        MarkTheStudyAsModified();
    }
}

template<Data::GlobalTransmissionCapacities capacity>
void Optimization::onSelectTransmissionCapacity(wxCommandEvent&)
{
    setTransmissionCapacity(capacity);
}

// -----------------------------------
// On select methods for MPS export
// -----------------------------------
void Optimization::onSelectExportMPS(const Data::mpsExportStatus& mps_export_status)
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        if (study->parameters.include.exportMPS != mps_export_status)
        {
            study->parameters.include.exportMPS = mps_export_status;
            refresh();
            MarkTheStudyAsModified();
        }
    }
}

// ----------------------------------------------------
// On select methods for unfeasible problem behavior
// ----------------------------------------------------
void Optimization::onSelectUnfeasibleBehavior(
  const Data::UnfeasibleProblemBehavior& unfeasibleProblemBehavior)
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        if (study->parameters.include.unfeasibleProblemBehavior != unfeasibleProblemBehavior)
        {
            study->parameters.include.unfeasibleProblemBehavior = unfeasibleProblemBehavior;
            refresh();
            MarkTheStudyAsModified();
        }
    }
}

void Optimization::onSelectUnfeasibleBehaviorWarningDry(wxCommandEvent&)
{
    onSelectUnfeasibleBehavior(Data::UnfeasibleProblemBehavior::WARNING_DRY);
}

void Optimization::onSelectUnfeasibleBehaviorWarningMps(wxCommandEvent&)
{
    onSelectUnfeasibleBehavior(Data::UnfeasibleProblemBehavior::WARNING_MPS);
}

void Optimization::onSelectUnfeasibleBehaviorErrorDry(wxCommandEvent&)
{
    onSelectUnfeasibleBehavior(Data::UnfeasibleProblemBehavior::ERROR_DRY);
}

void Optimization::onSelectUnfeasibleBehaviorErrorMps(wxCommandEvent&)
{
    onSelectUnfeasibleBehavior(Data::UnfeasibleProblemBehavior::ERROR_MPS);
}

} // namespace Options
} // namespace Window
} // namespace Antares
