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

#include "advanced.h"
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include "../../../toolbox/create.h"
#include "../../../toolbox/resources.h"
#include "../../../application/study.h"
#include "../../../application/menus.h"
#include "../../../windows/message.h"
#include <ui/common/component/panel.h>
#include <antares/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Options
{

Yuni::Event<void(bool)> OnRenewableGenerationModellingChanged;

static void Title(wxWindow* parent, wxSizer* sizer, const wxChar* text, bool margintop = true)
{
    if (margintop)
    {
        sizer->AddSpacer(25);
        sizer->AddSpacer(25);
    }

    auto* label = Component::CreateLabel(parent, text, true);

    sizer->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    sizer->AddSpacer(5);
    sizer->AddSpacer(5);
    sizer->AddSpacer(5);
}

AdvancedParameters::AdvancedParameters(wxWindow* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Advanced parameters"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN)
{
    assert(parent);

    for (uint i = 0; i != (uint)Data::seedMax; ++i)
        pEditSeeds[i] = nullptr;

    // Title of the Form
    SetLabel(wxT("Advanced parameters"));
    SetTitle(wxT("Advanced parameters"));

    // Background color
    wxColour defaultBgColor = GetBackgroundColour();
    wxColour bgColor(255, 255, 255);
    SetBackgroundColour(bgColor);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
    hz->AddSpacer(6);
    hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/64x64/advanced.png"),
            0,
            wxALL | wxALIGN_TOP);
    hz->AddSpacer(35);

    auto* s = new wxFlexGridSizer(0, 2, 1, 10);
    s->AddGrowableCol(0, 1);
    hz->Add(s, 1, wxALL | wxEXPAND);
    sizer->Add(hz, 0, wxALL | wxEXPAND, 20);

    wxStaticText* label;
    Component::Button* button;
    Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)> onPopup;

    Title(this, s, wxT("Seeds for random numbers"), false);

    for (uint seed = 0; seed != (uint)Data::seedMax; ++seed)
    {
        pEditSeeds[seed] = insertEdit(this,
                                      s,
                                      wxStringFromUTF8(Data::SeedToCString((Data::SeedIndex)seed)),
                                      wxCommandEventHandler(AdvancedParameters::onEditSeedTSDraws));
    }

    // s = new wxFlexGridSizer( 2, 1, 10);
    // s->AddGrowableCol(0, 1);
    // hz->AddSpacer(20);
    // hz->Add(s, 1, wxTOP|wxALIGN_TOP|wxEXPAND);

    Title(this, s, wxT("Spatial time-series correlation"));

    // Numeric quality
    {
        label = Component::CreateLabel(this, wxT("Numeric quality: load"));
        button = new Component::Button(this, wxT("include"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onNumericQuality, Data::timeSeriesLoad);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnNumericQualityLoad = button;
    }
    {
        label = Component::CreateLabel(this, wxT("Numeric quality: wind"));
        button = new Component::Button(this, wxT("include"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onNumericQuality, Data::timeSeriesWind);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnNumericQualityWind = button;
    }
    {
        label = Component::CreateLabel(this, wxT("Numeric quality: solar"));
        button = new Component::Button(this, wxT("include"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onNumericQuality, Data::timeSeriesSolar);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnNumericQualitySolar = button;
    }

    // Other preferences
    Title(this, s, wxT("Other preferences"));

    // Initial reservoir levels
    {
        label = Component::CreateLabel(this, wxT("Initial reservoir levels"));
        button = new Component::Button(this, wxT("cold start"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onInitialReservoirLevels);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnInitialReservoirLevels = button;
    }
    // Hydro heuristic policy
    {
        label = Component::CreateLabel(this, wxT("Hydro heuristic policy"));
        button
          = new Component::Button(this, wxT("accommodate rule curves"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onHydroHeuristicPolicy);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnHydroHeuristicPolicy = button;
    }
    // Hydro Pricing Mode
    {
        label = Component::CreateLabel(this, wxT("Hydro Pricing Mode"));
        button = new Component::Button(this, wxT("Fast"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onHydroPricingMode);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnHydroPricing = button;
    }
    // Power Fluctuations
    {
        label = Component::CreateLabel(this, wxT("Power fluctuations"));
        button = new Component::Button(this, wxT("minimize ramping"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onPowerFluctuations);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnPowerFluctuations = button;
    }
    // Shedding Policy
    {
        label = Component::CreateLabel(this, wxT("Shedding Policy"));
        button = new Component::Button(this, wxT("minimize duration"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onSheddingPolicy);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnSheddingPolicy = button;
    }
    // District marginal prices
    {
        label = Component::CreateLabel(this, wxT("District marginal prices"));
        button = new Component::Button(this, wxT("include"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        // button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnMultiNodalMarginalPrices = button;
        button->enabled(false);
    }

    // Day ahead reserve allocation
    {
        label = Component::CreateLabel(this, wxT("Day ahead reserve management"));
        button = new Component::Button(this, wxT("local"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        // onPopup.bind(this, &AdvancedParameters::onDAReserveManagementMode);
        // button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnDARreserveManagement = button;
        button->enabled(false);
    }

    // Unit Commitment mode
    {
        label = Component::CreateLabel(this, wxT("Unit Commitment Mode"));
        button = new Component::Button(this, wxT("Fast"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onUnitCommitmentMode);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnUnitCommitment = button;
    }

    // Number of cores
    {
        label = Component::CreateLabel(this, wxT("Simulation cores"));
        button = new Component::Button(this, wxT("Min"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onNumberOfCores);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnNumberOfCores = button;
    }

    // Renewable generation modelling
    {
        label = Component::CreateLabel(this, wxT("Renewable generation modeling"));
        button = new Component::Button(this, wxT("Aggregated"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdvancedParameters::onRenewableGenerationModelling);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnRenewableGenModelling = button;
    }

    sizer->AddSpacer(15);

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
                                   &AdvancedParameters::onResetToDefault);
    pnlSizerBtns->Add(22, 5);
    pnlSizerBtns->Add(button, 0, wxALL | wxEXPAND);

    pnlSizerBtns->AddStretchSpacer();
    wxButton* pBtnCancel = Antares::Component::CreateButton(
      panel, wxT("  Close  "), this, &AdvancedParameters::onClose);
    pBtnCancel->SetDefault();
    pnlSizerBtns->Add(pBtnCancel, 0, wxALL | wxEXPAND);
    pnlSizerBtns->Add(25, 5);

    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND);
    sizer->Add(panel, 0, wxALL | wxEXPAND);

    // refresh
    Connect(
      GetId(), wxEVT_MOTION, wxMouseEventHandler(AdvancedParameters::onInternalMotion), NULL, this);

    refresh();
    SetSizer(sizer);
    sizer->Layout();

    // Recentre the window
    sizer->Fit(this);
    Centre(wxBOTH);
}

AdvancedParameters::~AdvancedParameters()
{
}

void AdvancedParameters::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

void AdvancedParameters::onResetToDefault(void*)
{
    if (not Data::Study::Current::Valid())
        return;

    Window::Message message(this,
                            wxT("Advanced parameters"),
                            wxT("Advanced parameters"),
                            wxT("Do you really want to reset to the default values ?"));
    message.add(Window::Message::btnContinue);
    message.add(Window::Message::btnCancel, true);

    if (message.showModal() == Window::Message::btnContinue)
    {
        auto& study = *Data::Study::Current::Get();
        auto& parameters = study.parameters;

        parameters.resetSeeds();
        parameters.timeSeriesAccuracyOnCorrelation &= ~Data::timeSeriesLoad;
        parameters.timeSeriesAccuracyOnCorrelation &= ~Data::timeSeriesWind;
        parameters.timeSeriesAccuracyOnCorrelation &= ~Data::timeSeriesSolar;

        parameters.initialReservoirLevels.iniLevels = Data::irlColdStart;
        parameters.hydroHeuristicPolicy.hhPolicy = Data::hhpAccommodateRuleCurves;
        parameters.hydroPricing.hpMode = Data::hpHeuristic;
        parameters.power.fluctuations = Data::lssFreeModulations;
        // parameters.shedding.strategy = Data::shsShareMargins;
        parameters.shedding.policy = Data::shpShavePeaks;
        parameters.reserveManagement.daMode = Data::daGlobal;
        parameters.unitCommitment.ucMode = Data::ucHeuristic;
        parameters.nbCores.ncMode = Data::ncAvg;

        refresh();
        MarkTheStudyAsModified();
    }
}

void AdvancedParameters::onInternalMotion(wxMouseEvent&)
{
    Component::Panel::OnMouseMoveFromExternalComponent();
}

void AdvancedParameters::refresh()
{
    if (not Data::Study::Current::Valid())
        return;
    // The current study
    auto& study = *Data::Study::Current::Get();

    for (uint seed = 0; seed != (uint)Data::seedMax; ++seed)
    {
        if (pEditSeeds[seed])
            pEditSeeds[seed]->SetValue(wxString() << study.parameters.seed[seed]);
    }

    // accuracy
    {
        uint accuracy = study.parameters.timeSeriesAccuracyOnCorrelation;
        pBtnNumericQualityLoad->caption((0 != (accuracy & Data::timeSeriesLoad)) ? wxT("high")
                                                                                 : wxT("standard"));

        pBtnNumericQualitySolar->caption(
          (0 != (accuracy & Data::timeSeriesSolar)) ? wxT("high") : wxT("standard"));

        pBtnNumericQualityWind->caption((0 != (accuracy & Data::timeSeriesWind)) ? wxT("high")
                                                                                 : wxT("standard"));
    }

    wxString text;

    text = wxStringFromUTF8(
      InitialReservoirLevelsToCString(study.parameters.initialReservoirLevels.iniLevels));
    pBtnInitialReservoirLevels->caption(text);

    text = wxStringFromUTF8(
      HydroHeuristicPolicyToCString(study.parameters.hydroHeuristicPolicy.hhPolicy));
    pBtnHydroHeuristicPolicy->caption(text);

    text = wxStringFromUTF8(HydroPricingModeToCString(study.parameters.hydroPricing.hpMode));
    pBtnHydroPricing->caption(text);

    text = wxStringFromUTF8(PowerFluctuationsToCString(study.parameters.power.fluctuations));
    pBtnPowerFluctuations->caption(text);

    text = wxStringFromUTF8(SheddingPolicyToCString(study.parameters.shedding.policy));
    pBtnSheddingPolicy->caption(text);

    pBtnMultiNodalMarginalPrices->caption(wxT("average"));

    text = wxStringFromUTF8(UnitCommitmentModeToCString(study.parameters.unitCommitment.ucMode));
    pBtnUnitCommitment->caption(text);

    text = wxStringFromUTF8(NumberOfCoresModeToCString(study.parameters.nbCores.ncMode));
    pBtnNumberOfCores->caption(text);

    text = wxStringFromUTF8(RenewableGenerationModellingToCString(study.parameters.renewableGeneration()));
    pBtnRenewableGenModelling->caption(text);

    text = wxStringFromUTF8(
      DayAheadReserveManagementModeToCString(study.parameters.reserveManagement.daMode));
    // pBtnDAReserveAllocation->caption(text);
    pBtnDARreserveManagement->caption(wxT("global"));
}

wxTextCtrl* AdvancedParameters::insertEdit(wxWindow* parent,
                                           wxSizer* sizer,
                                           const wxString& text,
                                           wxObjectEventFunction method)
{
    auto* label = Component::CreateLabel(parent, text);
    auto* edit = new wxTextCtrl(parent, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize(180, 20));
    sizer->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    sizer->Add(edit, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    // Connect event
    edit->Connect(edit->GetId(), wxEVT_COMMAND_TEXT_UPDATED, method, nullptr, this);
    return edit;
}

void AdvancedParameters::onEditSeedTSDraws(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    int id = evt.GetId();

    // Looking for the good id
    for (uint i = 0; i != (uint)Data::seedMax; ++i)
    {
        if (pEditSeeds[i] && id == pEditSeeds[i]->GetId())
        {
            String text;
            wxStringToString(pEditSeeds[i]->GetValue(), text);

            uint newseed;
            if (not text.to(newseed))
            {
                logs.error() << "impossible to update the seed for '"
                             << Data::SeedToCString((Data::SeedIndex)i) << "'";
            }
            else
            {
                if (newseed != study.parameters.seed[i])
                {
                    study.parameters.seed[i] = newseed;
                    MarkTheStudyAsModified();
                }
            }
            return;
        }
    }
}

void AdvancedParameters::onNumericQuality(Component::Button&,
                                          wxMenu& menu,
                                          void*,
                                          Data::TimeSeries ts)
{
    pCurrentTS = ts;
    wxMenuItem* it;

    wxString text(wxT("standard"));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNumericQualityStandard),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("high"), "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNumericQualityHigh),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectNumericQualityStandard(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    uint old = study.parameters.timeSeriesAccuracyOnCorrelation;
    study.parameters.timeSeriesAccuracyOnCorrelation &= ~pCurrentTS;
    if (old != study.parameters.timeSeriesAccuracyOnCorrelation)
    {
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectNumericQualityHigh(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    uint old = study.parameters.timeSeriesAccuracyOnCorrelation;
    study.parameters.timeSeriesAccuracyOnCorrelation |= pCurrentTS;
    if (old != study.parameters.timeSeriesAccuracyOnCorrelation)
    {
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onInitialReservoirLevels(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(InitialReservoirLevelsToCString(Data::irlColdStart));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectColdStart),
                 nullptr,
                 this);

    text.clear();
    text << wxStringFromUTF8(InitialReservoirLevelsToCString(Data::irlHotStart));
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectHotStart),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectColdStart(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.initialReservoirLevels.iniLevels != Data::irlColdStart)
    {
        study.parameters.initialReservoirLevels.iniLevels = Data::irlColdStart;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectHotStart(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.initialReservoirLevels.iniLevels != Data::irlHotStart)
    {
        study.parameters.initialReservoirLevels.iniLevels = Data::irlHotStart;
        MarkTheStudyAsModified();
        refresh();
    }
}

// ... Hydro heuristic policy
void AdvancedParameters::onHydroHeuristicPolicy(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(HydroHeuristicPolicyToCString(Data::hhpAccommodateRuleCurves));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectAccomodateRuleCurves),
                 nullptr,
                 this);

    text.clear();
    text << wxStringFromUTF8(HydroHeuristicPolicyToCString(Data::hhpMaximizeGeneration));
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectMaximizeGeneration),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectAccomodateRuleCurves(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.hydroHeuristicPolicy.hhPolicy != Data::hhpAccommodateRuleCurves)
    {
        study.parameters.hydroHeuristicPolicy.hhPolicy = Data::hhpAccommodateRuleCurves;
        MarkTheStudyAsModified();
        refresh();
    }
}
void AdvancedParameters::onSelectMaximizeGeneration(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.hydroHeuristicPolicy.hhPolicy != Data::hhpMaximizeGeneration)
    {
        study.parameters.hydroHeuristicPolicy.hhPolicy = Data::hhpMaximizeGeneration;
        MarkTheStudyAsModified();
        refresh();
    }
}

// ... Hydro Pricing ...
void AdvancedParameters::onHydroPricingMode(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(HydroPricingModeToCString(Data::hpHeuristic)); // Fast
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectHPHeuristic),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(HydroPricingModeToCString(Data::hpMILP)); // Accurate
    text << wxT("   (slow)");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectHPMixedIntegerLinearProblem),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectHPHeuristic(wxCommandEvent& evt)
{
    auto& study = *Data::Study::Current::Get();
    if (not Data::Study::Current::Valid())
        return;

    if (study.parameters.hydroPricing.hpMode != Data::hpHeuristic)
    {
        study.parameters.hydroPricing.hpMode = Data::hpHeuristic;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectHPMixedIntegerLinearProblem(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.hydroPricing.hpMode != Data::hpMILP)
    {
        study.parameters.hydroPricing.hpMode = Data::hpMILP;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onPowerFluctuations(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(PowerFluctuationsToCString(Data::lssFreeModulations));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectFreeModulations),
                 nullptr,
                 this);

    text.clear();
    text << wxStringFromUTF8(PowerFluctuationsToCString(Data::lssMinimizeExcursions))
         << wxT("  (slow)");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectMinimizeExcursions),
                 nullptr,
                 this);

    text.clear();
    text << wxStringFromUTF8(PowerFluctuationsToCString(Data::lssMinimizeRamping))
         << wxT("  (slow)");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectMinimizeRamping),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectMinimizeRamping(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.power.fluctuations != Data::lssMinimizeRamping)
    {
        study.parameters.power.fluctuations = Data::lssMinimizeRamping;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectMinimizeExcursions(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.power.fluctuations != Data::lssMinimizeExcursions)
    {
        study.parameters.power.fluctuations = Data::lssMinimizeExcursions;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectFreeModulations(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.power.fluctuations != Data::lssFreeModulations)
    {
        study.parameters.power.fluctuations = Data::lssFreeModulations;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSheddingPolicy(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(SheddingPolicyToCString(Data::shpShavePeaks));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectSHPShavePeaks),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(SheddingPolicyToCString(Data::shpMinimizeDuration));
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectSHPMinimizeDuration),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectSHPShavePeaks(wxCommandEvent&)
{
    auto& study = *Data::Study::Current::Get();
    if (not Data::Study::Current::Valid())
        return;

    if (study.parameters.shedding.policy != Data::shpShavePeaks)
    {
        study.parameters.shedding.policy = Data::shpShavePeaks;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectSHPMinimizeDuration(wxCommandEvent&)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.shedding.policy != Data::shpMinimizeDuration)
    {
        study.parameters.shedding.policy = Data::shpMinimizeDuration;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onUnitCommitmentMode(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(UnitCommitmentModeToCString(Data::ucHeuristic)); // Fast
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectUCHeuristic),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(UnitCommitmentModeToCString(Data::ucMILP)); // Accurate
    text << wxT("   (slow)");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectUCMixedIntegerLinearProblem),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectUCHeuristic(wxCommandEvent& evt)
{
    auto& study = *Data::Study::Current::Get();
    if (not Data::Study::Current::Valid())
        return;

    if (study.parameters.unitCommitment.ucMode != Data::ucHeuristic)
    {
        study.parameters.unitCommitment.ucMode = Data::ucHeuristic;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectUCMixedIntegerLinearProblem(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.unitCommitment.ucMode != Data::ucMILP)
    {
        study.parameters.unitCommitment.ucMode = Data::ucMILP;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onNumberOfCores(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(NumberOfCoresModeToCString(Data::ncMin)); // Min
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNCmin),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(NumberOfCoresModeToCString(Data::ncLow)); // Low
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNClow),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(NumberOfCoresModeToCString(Data::ncAvg)); // Average (or medium)
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNCaverage),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(NumberOfCoresModeToCString(Data::ncHigh)); // High
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNChigh),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(NumberOfCoresModeToCString(Data::ncMax)); // Max
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectNCmax),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectNCmin(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.nbCores.ncMode != Data::ncMin)
    {
        study.parameters.nbCores.ncMode = Data::ncMin;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectNClow(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.nbCores.ncMode != Data::ncLow)
    {
        study.parameters.nbCores.ncMode = Data::ncLow;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectNCaverage(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.nbCores.ncMode != Data::ncAvg)
    {
        study.parameters.nbCores.ncMode = Data::ncAvg;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectNChigh(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.nbCores.ncMode != Data::ncHigh)
    {
        study.parameters.nbCores.ncMode = Data::ncHigh;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectNCmax(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.nbCores.ncMode != Data::ncMax)
    {
        study.parameters.nbCores.ncMode = Data::ncMax;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onRenewableGenerationModelling(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(RenewableGenerationModellingToCString(Data::rgClusters));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(AdvancedParameters::onSelectRGMrenewableClusters),
        nullptr,
        this);

    text.clear();
    text = wxStringFromUTF8(RenewableGenerationModellingToCString(Data::rgAggregated));
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(AdvancedParameters::onSelectRGMaggregated),
        nullptr,
        this);
}

void AdvancedParameters::onSelectRGMaggregated(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.renewableGeneration() != Data::rgAggregated)
    {
        study.parameters.renewableGeneration.rgModelling = Data::rgAggregated;
        MarkTheStudyAsModified();
        OnRenewableGenerationModellingChanged(false);
        refresh();
    }
}

void AdvancedParameters::onSelectRGMrenewableClusters(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.renewableGeneration() != Data::rgClusters)
    {
        study.parameters.renewableGeneration.rgModelling = Data::rgClusters;
        MarkTheStudyAsModified();
        OnRenewableGenerationModellingChanged(false);
        refresh();
    }
}

void AdvancedParameters::onDAReserveAllocationMode(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;
    wxString text;

    text = wxStringFromUTF8(DayAheadReserveManagementModeToCString(Data::daGlobal));
    text << wxT("   [default]");
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectDAGlobal),
                 nullptr,
                 this);

    text.clear();
    text = wxStringFromUTF8(DayAheadReserveManagementModeToCString(Data::daLocal));
    it = Menu::CreateItem(&menu, wxID_ANY, text, "images/16x16/tag.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdvancedParameters::onSelectDALocal),
                 nullptr,
                 this);
}

void AdvancedParameters::onSelectDAGlobal(wxCommandEvent& evt)
{
    auto& study = *Data::Study::Current::Get();
    if (not Data::Study::Current::Valid())
        return;

    if (study.parameters.reserveManagement.daMode != Data::daGlobal)
    {
        study.parameters.reserveManagement.daMode = Data::daGlobal;
        MarkTheStudyAsModified();
        refresh();
    }
}

void AdvancedParameters::onSelectDALocal(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    if (study.parameters.reserveManagement.daMode != Data::daLocal)
    {
        study.parameters.reserveManagement.daMode = Data::daLocal;
        MarkTheStudyAsModified();
        refresh();
    }
}

} // namespace Options
} // namespace Window
} // namespace Antares
