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

#include "management.h"
#include <wx/stattext.h>
#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"
#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"
#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"
#include "../../toolbox/components/button.h"
#include "../../toolbox/validator.h"
#include "../../toolbox/create.h"
#include "../../application/menus.h"
#include <wx/statline.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Hydro
{
Management::Management(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
 wxScrolledWindow(parent), pInputAreaSelector(notifier)
{
    OnStudyClosed.connect(this, &Management::onStudyClosed);
    if (notifier)
        notifier->onAreaChanged.connect(this, &Management::onAreaChanged);
}

void Management::createComponents()
{
    if (pComponentsAreReady)
        return;
    pComponentsAreReady = true;

    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(sizer);
        pSupport = new Component::Panel(this);
        sizer->Add(pSupport, 1, wxALL | wxEXPAND);
    }
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pSupport->SetSizer(sizer);

    const wxSize ourDefaultSize(55, wxDefaultSize.GetHeight());

    enum
    {
        right = wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL,
        left = wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL,
    };

    auto* pGrid = new wxFlexGridSizer(6, 0, 8);
    sizer->Add(pGrid, 0, wxALL, 15);

    // Follow load modulations
    {
        pGrid->Add(Component::CreateLabel(pSupport, wxT("Follow load modulations")), 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleFollowLoad);
        pGrid->Add(button, 0, left);
        pFollowLoad = button;
    }
    // Interdaily breakdown
    {
        pGrid->Add(Component::CreateLabel(pSupport, wxT("Inter-daily breakdown")), 0, right);

        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pInterdailyBreakdown = edit;
    }

    // Interdaily modulation
    {
        pGrid->Add(Component::CreateLabel(pSupport, wxT("Intra-daily modulation")), 0, right);

        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pIntradailyModulation = edit;
    }

    // Space
    {
        enum
        {
            verticalSpace = 10
        };
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
    }

    // Reservoir management
    {
        pGrid->Add(Component::CreateLabel(pSupport, wxT("Reservoir management")), 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleReservoirManagement);
        pGrid->Add(button, 0, left);
        pReservoirManagement = button;
    }

    // Use Water Value
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Use water value"));
        pLabelUseWaterValues = label;
        pGrid->Add(label, 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleUseWaterValue);
        pGrid->Add(button, 0, left);
        pUseWaterValue = button;
    }

    // Use heuristic target
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Use heuristic target"));
        pLabelUseHeuristicTarget = label;
        pGrid->Add(label, 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleUseHeuristicTarget);
        pGrid->Add(button, 0, left);
        pUseHeuristicTarget = button;
    }

    // Space
    {
        enum
        {
            verticalSpace = 10
        };
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
    }

    // Reservoir capacity
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Reservoir capacity (MWh)"));
        pLabelReservoirCapacity = label;
        pGrid->Add(label, 0, right);

        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pReservoirCapacity = edit;
    }

    // Initialize reservoir level date
    {
        pGrid->Add(
          Component::CreateLabel(pSupport, wxT("Initialize reservoir level on ")), 0, right);
        auto* button = new Component::Button(pSupport, Antares::Date::MonthToString(0, 0));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleInitializeReservoirLevelDate);
        pGrid->Add(button, 0, left);
        pInitializeReservoirLevelDate = button;
    }

    // Intermonthly breakdown
    {
        pGrid->Add(Component::CreateLabel(pSupport, wxT("    Inter-monthly breakdown")), 0, right);

        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pIntermonthlyBreakdown = edit;
    }

    // Space
    {
        enum
        {
            verticalSpace = 10
        };
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
    }

    // Use Leeway
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Use Leeway"));
        pLabelUseLeeway = label;
        pGrid->Add(label, 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleUseLeeway);
        pGrid->Add(button, 0, left);
        pUseLeeway = button;
    }

    // Leeway low bound
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Leeway low bound"));
        pLabelLeewayLow = label;
        pGrid->Add(label, 0, right);
        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pLeewayLowerBound = edit;
    }

    // Leeway upper bound
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Leeway upper bound"));
        pLabelLeewayUp = label;
        pGrid->Add(label, 0, right);
        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pLeewayUpperBound = edit;
    }

    // Space
    {
        enum
        {
            verticalSpace = 10
        };
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
        pGrid->AddSpacer(verticalSpace);
    }

    // Hard Bounds on rules curves
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Hard bounds on rules curves"));
        pLabelHardBounds = label;
        pGrid->Add(label, 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onToggleHardBoundsOnRuleCurves);
        pGrid->Add(button, 0, left);
        pHardBoundsOnRuleCurves = button;
    }

    // Power to level modulations
    {
        auto* label = Component::CreateLabel(pSupport, wxT("Power to level modulations :"));
        pLabelPowerToLevel = label;
        pGrid->Add(label, 0, right);

        auto* button = new Component::Button(pSupport, wxT("Yes"));
        button->menu(true);
        button->onPopupMenu(this, &Management::onTogglePowerToLevel);
        pGrid->Add(button, 0, left);
        pPowerToLevel = button;
    }

    // Pumping Efficiency
    {
        pGrid->Add(Component::CreateLabel(pSupport, wxT("  Pumping Efficiency Ratio")), 0, right);

        auto* edit = new wxTextCtrl(pSupport,
                                    wxID_ANY,
                                    wxT("0.0"),
                                    wxDefaultPosition,
                                    ourDefaultSize,
                                    0,
                                    Toolbox::Validator::Numeric());
        pGrid->Add(edit, 0, wxALL | wxEXPAND);
        pPumpingEfficiency = edit;
    }

    pGrid->SetItemMinSize(pIntermonthlyBreakdown, 64, 10);
    pGrid->SetItemMinSize(pReservoirCapacity, 64, 10);

    sizer->Layout();

    pInterdailyBreakdown->Connect(pInterdailyBreakdown->GetId(),
                                  wxEVT_COMMAND_TEXT_UPDATED,
                                  wxCommandEventHandler(Management::onInterdailyBreakdownChanged),
                                  nullptr,
                                  this);
    pIntradailyModulation->Connect(pIntradailyModulation->GetId(),
                                   wxEVT_COMMAND_TEXT_UPDATED,
                                   wxCommandEventHandler(Management::onIntradailyModulationChanged),
                                   nullptr,
                                   this);

    pIntermonthlyBreakdown->Connect(
      pIntermonthlyBreakdown->GetId(),
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(Management::onIntermonthlyBreakdownChanged),
      nullptr,
      this);

    pReservoirCapacity->Connect(pReservoirCapacity->GetId(),
                                wxEVT_COMMAND_TEXT_UPDATED,
                                wxCommandEventHandler(Management::onReservoirCapacityChanged),
                                nullptr,
                                this);

    pLeewayUpperBound->Connect(pLeewayUpperBound->GetId(),
                               wxEVT_COMMAND_TEXT_UPDATED,
                               wxCommandEventHandler(Management::onLeewayUpperBoundChanged),
                               nullptr,
                               this);
    pLeewayLowerBound->Connect(pLeewayLowerBound->GetId(),
                               wxEVT_COMMAND_TEXT_UPDATED,
                               wxCommandEventHandler(Management::onLeewayLowBoundChanged),
                               nullptr,
                               this);

    pPumpingEfficiency->Connect(pPumpingEfficiency->GetId(),
                                wxEVT_COMMAND_TEXT_UPDATED,
                                wxCommandEventHandler(Management::onPumpingEfficiencyChanged),
                                nullptr,
                                this);
}

Management::~Management()
{
    destroyBoundEvents();
    // destroy all children as soon as possible to prevent against corrupt vtable
    DestroyChildren();
}

void Management::onAreaChanged(Data::Area* area)
{
    pArea = area;
    if (area and area->hydro.prepro)
    {
        // create components on-demand
        if (!pComponentsAreReady)
            createComponents();
        else
            GetSizer()->Show(pSupport, true);

        pIntermonthlyBreakdown->ChangeValue(wxString() << area->hydro.intermonthlyBreakdown);
        pInterdailyBreakdown->ChangeValue(wxString() << area->hydro.interDailyBreakdown);
        pIntradailyModulation->ChangeValue(wxString() << area->hydro.intraDailyModulation);
        pReservoirCapacity->ChangeValue(wxString() << area->hydro.reservoirCapacity);
        pLeewayUpperBound->ChangeValue(wxString() << area->hydro.leewayUpperBound);
        pLeewayLowerBound->ChangeValue(wxString() << area->hydro.leewayLowerBound);
        pPumpingEfficiency->ChangeValue(wxString() << area->hydro.pumpingEfficiency);
        if (area->hydro.reservoirManagement)
        {
            pReservoirManagement->caption(wxT("Yes"));
            pReservoirManagement->image("images/16x16/light_green.png");
            pLabelReservoirCapacity->Enable(true);
            pReservoirCapacity->Enable(true);
            pLabelUseHeuristicTarget->Enable(true);
            pUseWaterValue->Enable(true);
            pLabelUseWaterValues->Enable(true);
            pUseHeuristicTarget->Enable(true);
            if (area->hydro.useHeuristicTarget && area->hydro.useWaterValue)
            {
                pUseLeeway->Enable(true);
                pLabelUseLeeway->Enable(true);
            }
            pHardBoundsOnRuleCurves->Enable(true);
            pLabelHardBounds->Enable(true);
            pPumpingEfficiency->Enable(true);
            pInitializeReservoirLevelDate->Enable(true);
            pIntermonthlyBreakdown->Enable(true);
        }
        else
        {
            pReservoirManagement->caption(wxT("No"));
            pReservoirManagement->image("images/16x16/light_orange.png");
            pLabelReservoirCapacity->Enable(false);
            pReservoirCapacity->Enable(false);
            pLabelUseHeuristicTarget->Enable(false);
            pUseWaterValue->Enable(false);
            pLabelUseWaterValues->Enable(false);
            area->hydro.useWaterValue = false;
            pUseWaterValue->Enable(false);
            area->hydro.hardBoundsOnRuleCurves = false;
            pHardBoundsOnRuleCurves->Enable(false);
            pLabelHardBounds->Enable(false);
            area->hydro.useHeuristicTarget = true;
            pUseHeuristicTarget->Enable(false);
            pUseLeeway->Enable(false);
            pLabelUseLeeway->Enable(false);
            area->hydro.useLeeway = false;
            pPumpingEfficiency->Enable(false);
            pInitializeReservoirLevelDate->Enable(false);
            pIntermonthlyBreakdown->Enable(false);
        }

        if (area->hydro.followLoadModulations)
        {
            pFollowLoad->caption(wxT("Yes"));
            pFollowLoad->image("images/16x16/light_green.png");
        }
        else
        {
            pFollowLoad->caption(wxT("No"));
            pFollowLoad->image("images/16x16/light_orange.png");
        }

        if (area->hydro.useWaterValue)
        {
            pUseWaterValue->caption(wxT("Yes"));
            pUseWaterValue->image("images/16x16/light_green.png");
        }
        else
        {
            pUseWaterValue->caption(wxT("No"));
            pUseWaterValue->image("images/16x16/light_orange.png");
            pUseLeeway->Enable(false);
            pLabelUseLeeway->Enable(false);
            area->hydro.useLeeway = false;
        }

        if (area->hydro.hardBoundsOnRuleCurves)
        {
            pHardBoundsOnRuleCurves->caption(wxT("Yes"));
            pHardBoundsOnRuleCurves->image("images/16x16/light_green.png");
        }
        else
        {
            pHardBoundsOnRuleCurves->caption(wxT("No"));
            pHardBoundsOnRuleCurves->image("images/16x16/light_orange.png");
        }

        if (area->hydro.initializeReservoirLevelDate)
        {
            pInitializeReservoirLevelDate->caption(
              Antares::Date::MonthToString(area->hydro.initializeReservoirLevelDate, 0));
        }
        else
        {
            pInitializeReservoirLevelDate->caption(Antares::Date::MonthToString(0, 0));
        }

        if (area->hydro.useHeuristicTarget)
        {
            pUseHeuristicTarget->caption(wxT("Yes"));
            pUseHeuristicTarget->image("images/16x16/light_green.png");
        }
        else
        {
            pUseHeuristicTarget->caption(wxT("No"));
            pUseHeuristicTarget->image("images/16x16/light_orange.png");
            area->hydro.useLeeway = false;
            pUseLeeway->Enable(false);
            pLabelUseLeeway->Enable(false);
        }
        if (area->hydro.useLeeway)
        {
            pUseLeeway->caption(wxT("Yes"));
            pUseLeeway->image("images/16x16/light_green.png");
            if (area->hydro.reservoirManagement && area->hydro.useWaterValue)
            {
                pLeewayLowerBound->Enable(true);
                pLabelLeewayLow->Enable(true);
                pLeewayUpperBound->Enable(true);
                pLabelLeewayUp->Enable(true);
            }
        }
        else
        {
            pUseLeeway->caption(wxT("No"));
            pUseLeeway->image("images/16x16/light_orange.png");

            pLeewayLowerBound->Enable(false);
            pLabelLeewayLow->Enable(false);
            pLeewayUpperBound->Enable(false);
            pLabelLeewayUp->Enable(false);
        }
        // Condition equivalent to generating between bounds
        if ((area->hydro.useHeuristicTarget && area->hydro.useLeeway)
            or not area->hydro.useHeuristicTarget)
        {
            pPowerToLevel->Enable(true);
            pLabelPowerToLevel->Enable(true);
        }
        else
        {
            pPowerToLevel->Enable(false);
            pLabelPowerToLevel->Enable(false);
            area->hydro.powerToLevel = false;
        }
        if (area->hydro.powerToLevel)
        {
            pPowerToLevel->caption(wxT("Yes"));
            pPowerToLevel->image("images/16x16/light_green.png");
        }
        else
        {
            pPowerToLevel->caption(wxT("No"));
            pPowerToLevel->image("images/16x16/light_orange.png");
        }
    }
    else
    {
        if (pComponentsAreReady)
        {
            pIntermonthlyBreakdown->ChangeValue(wxString(wxT("0.0")));
            pInterdailyBreakdown->ChangeValue(wxString(wxT("0.0")));
            pIntradailyModulation->ChangeValue(wxString(wxT("0.0")));
            pReservoirCapacity->ChangeValue(wxString(wxT("0")));
            pLeewayUpperBound->ChangeValue(wxString(wxT("0")));
            pLeewayLowerBound->ChangeValue(wxString(wxT("0")));
            pPumpingEfficiency->ChangeValue(wxString(wxT("0.0")));
            pReservoirManagement->caption(wxT("No"));
            pReservoirManagement->image("images/16x16/light_orange.png");
            pFollowLoad->caption(wxT("No"));
            pFollowLoad->image("images/16x16/light_orange.png");
            pUseWaterValue->caption(wxT("No"));
            pUseWaterValue->image("images/16x16/light_orange.png");
            pUseLeeway->caption(wxT("No"));
            pUseLeeway->image("images/16x16/light_orange.png");
            pPowerToLevel->caption(wxT("No"));
            pPowerToLevel->image("images/16x16/light_orange.png");
            pHardBoundsOnRuleCurves->caption(wxT("No"));
            pHardBoundsOnRuleCurves->image("images/16x16/light_orange.png");
            pUseHeuristicTarget->caption(wxT("No"));
            pUseHeuristicTarget->image("images/16x16/light_orange.png");
            pLabelReservoirCapacity->Enable(false);
            pInitializeReservoirLevelDate->caption(Antares::Date::MonthToString(0, 0));
        }
    }

    this->FitInside(); // ask the sizer about the needed size
    this->SetScrollRate(5, 5);
}

void Management::onIntermonthlyBreakdownChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);
        if (not Math::Equals(d, pArea->hydro.intermonthlyBreakdown))
        {
            if (d < 0.)
            {
                d = 0.;
                pIntermonthlyBreakdown->ChangeValue(wxT("0.0"));
            }
            pArea->hydro.intermonthlyBreakdown = d;
            MarkTheStudyAsModified();
        }
    }
}

void Management::onInterdailyBreakdownChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);
        if (not Math::Equals(d, pArea->hydro.interDailyBreakdown))
        {
            pArea->hydro.interDailyBreakdown = d;
            MarkTheStudyAsModified();
        }
    }
}

void Management::onIntradailyModulationChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);
        if (not Math::Equals(d, pArea->hydro.intraDailyModulation))
        {
            if (d < 1.)
            {
                d = 1.;
                pIntradailyModulation->ChangeValue(wxT("1"));
            }

            pArea->hydro.intraDailyModulation = d;
            MarkTheStudyAsModified();
        }
    }
}

void Management::onReservoirCapacityChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);
        if (d < 1e-6)
            d = 0;
        if (not Math::Equals(d, pArea->hydro.reservoirCapacity))
        {
            pArea->hydro.reservoirCapacity = d;
            MarkTheStudyAsModified();
        }
    }
}

void Management::onLeewayLowBoundChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);
        if (not Math::Equals(d, pArea->hydro.leewayLowerBound))
        {
            if (d >= 0 && d <= pArea->hydro.leewayUpperBound)
            {
                pArea->hydro.leewayLowerBound = d;
                MarkTheStudyAsModified();
            }

            if (d < 0)
            {
                pArea->hydro.leewayLowerBound = 0;
                pLeewayLowerBound->ChangeValue(wxT("0"));
            }

            if (d > pArea->hydro.leewayUpperBound)
            {
                pArea->hydro.leewayLowerBound = pArea->hydro.leewayUpperBound;
                pLeewayLowerBound->ChangeValue(wxString() << pArea->hydro.leewayUpperBound);
            }
        }
    }
}

void Management::onLeewayUpperBoundChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);
        if (not Math::Equals(d, pArea->hydro.leewayUpperBound))
        {
            if (d >= 0 && d >= pArea->hydro.leewayLowerBound)
            {
                pArea->hydro.leewayUpperBound = d;
                MarkTheStudyAsModified();
            }

            if (d < 0)
            {
                pArea->hydro.leewayUpperBound = 0.;
                pLeewayUpperBound->ChangeValue(wxT("0"));
            }

            if (d < pArea->hydro.leewayLowerBound)
            {
                pArea->hydro.leewayUpperBound = pArea->hydro.leewayLowerBound;
                pLeewayUpperBound->ChangeValue(wxString() << pArea->hydro.leewayLowerBound);
            }
        }
    }
}

void Management::onPumpingEfficiencyChanged(wxCommandEvent& evt)
{
    if (pArea)
    {
        if (evt.GetString().empty())
            return;
        double d;
        evt.GetString().ToDouble(&d);

        if (not Math::Equals(d, pArea->hydro.pumpingEfficiency))
        {
            if (d < 0.)
            {
                d = 0.;
                pPumpingEfficiency->ChangeValue(wxT("0.0"));
            }
            pArea->hydro.pumpingEfficiency = d;
            MarkTheStudyAsModified();
        }
    }
}

void Management::onStudyClosed()
{
    pArea = nullptr;

    if (GetSizer())
        GetSizer()->Show(pSupport, false);
}

void Management::onToggleReservoirManagement(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxT("Use reservoir management"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onEnableReserveManagement),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onDisableReserveManagement),
                 nullptr,
                 this);
}

void Management::onToggleFollowLoad(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxT("Follow load modulations"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onFollowingLoadModulations),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onUnfollowingLoadModulations),
                 nullptr,
                 this);
}

void Management::onToggleUseWaterValue(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Use water value"), "images/16x16/light_green.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onEnableUseWaterValue),
                 nullptr,
                 this);
    if (pArea->hydro.useHeuristicTarget)
    {
        it = Menu::CreateItem(
          &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Management::onDisableUseWaterValue),
                     nullptr,
                     this);
    }
}

void Management::onToggleHardBoundsOnRuleCurves(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxT("Hard bounds on rule curves"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onEnableHardBoundsOnRuleCurves),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onDisableHardBoundsOnRuleCurves),
                 nullptr,
                 this);
}

void Management::onToggleUseHeuristicTarget(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Use heuristic target"), "images/16x16/light_green.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onEnableUseHeuristicTarget),
                 nullptr,
                 this);

    if (pArea->hydro.useWaterValue)
    {
        it = Menu::CreateItem(
          &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Management::onDisableUseHeuristicTarget),
                     nullptr,
                     this);
    }
}

void Management::onToggleUseLeeway(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Use Leeway"), "images/16x16/light_green.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onEnableUseLeeway),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onDisableUseLeeway),
                 nullptr,
                 this);
}

void Management::onToggleInitializeReservoirLevelDate(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    for (int i = 0; i < 12; i++)
    {
        it = Menu::CreateItem(&menu, i, Antares::Date::MonthToString(i, 0), NULL, wxEmptyString);
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Management::onChangingInitializeReservoirLevelDate),
                     nullptr,
                     this);
    }
}

void Management::onTogglePowerToLevel(Component::Button&, wxMenu& menu, void*)
{
    if (!pArea)
    {
        Menu::CreateItem(&menu,
                         wxID_ANY,
                         wxT("Please select an area"),
                         "images/16x16/light_orange.png",
                         wxEmptyString);
        return;
    }
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxT("Power to Level Modulations"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onEnablePowerToLevel),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Management::onDisablePowerToLevel),
                 nullptr,
                 this);
}

void Management::onEnableReserveManagement(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.reservoirManagement)
    {
        pArea->hydro.reservoirManagement = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onDisableReserveManagement(wxCommandEvent&)
{
    if (pArea and pArea->hydro.reservoirManagement)
    {
        pArea->hydro.reservoirManagement = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onFollowingLoadModulations(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.followLoadModulations)
    {
        pArea->hydro.followLoadModulations = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onUnfollowingLoadModulations(wxCommandEvent&)
{
    if (pArea and pArea->hydro.followLoadModulations)
    {
        pArea->hydro.followLoadModulations = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onEnableUseWaterValue(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.useWaterValue)
    {
        pArea->hydro.useWaterValue = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onDisableUseWaterValue(wxCommandEvent&)
{
    if (pArea and pArea->hydro.useWaterValue)
    {
        pArea->hydro.useWaterValue = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onEnableHardBoundsOnRuleCurves(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.hardBoundsOnRuleCurves)
    {
        pArea->hydro.hardBoundsOnRuleCurves = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onDisableHardBoundsOnRuleCurves(wxCommandEvent&)
{
    if (pArea and pArea->hydro.hardBoundsOnRuleCurves)
    {
        pArea->hydro.hardBoundsOnRuleCurves = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onEnableUseHeuristicTarget(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.useHeuristicTarget)
    {
        pArea->hydro.useHeuristicTarget = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onDisableUseHeuristicTarget(wxCommandEvent&)
{
    if (pArea and pArea->hydro.useHeuristicTarget)
    {
        pArea->hydro.useHeuristicTarget = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onEnableUseLeeway(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.useLeeway)
    {
        pArea->hydro.useLeeway = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onDisableUseLeeway(wxCommandEvent&)
{
    if (pArea and pArea->hydro.useLeeway)
    {
        pArea->hydro.useLeeway = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onEnablePowerToLevel(wxCommandEvent&)
{
    if (pArea and not pArea->hydro.powerToLevel)
    {
        pArea->hydro.powerToLevel = true;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onDisablePowerToLevel(wxCommandEvent&)
{
    if (pArea and pArea->hydro.powerToLevel)
    {
        pArea->hydro.powerToLevel = false;
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

void Management::onChangingInitializeReservoirLevelDate(wxCommandEvent& evt)
{
    if (pArea)
    {
        pArea->hydro.initializeReservoirLevelDate = evt.GetId();
        MarkTheStudyAsModified();
        onAreaChanged(pArea);
    }
}

} // namespace Hydro
} // namespace Window
} // namespace Antares
