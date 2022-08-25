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

#include "adequacy-patch-options.h"
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
using namespace Data::AdequacyPatch;

namespace Antares::Window::Options
{
static void addLabelAdqPatch(wxWindow* parent, wxSizer* sizer, const wxChar* text)
{
    sizer->AddSpacer(25);
    sizer->AddSpacer(25);

    auto* label = Component::CreateLabel(parent, text, true);

    sizer->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    sizer->AddSpacer(5);
    sizer->AddSpacer(5);
    sizer->AddSpacer(5);
}

static void updateButton(Component::Button* button, bool value, std::string const& buttonType)
{
    char type;
    if (buttonType == "ntc")
    {
        type = 'N';
    }
    else
    {
        type = (buttonType == "pto") ? 'P' : 'S';
    }

    assert(button != NULL);
    if (value)
    {
        switch (type)
        {
        case 'N':
            button->image("images/16x16/light_orange.png");
            button->caption(wxT("set to null"));
            break;
        case 'P':
            button->image("images/16x16/tag.png");
            button->caption(wxT("Load"));
            break;
        default:
            button->image("images/16x16/light_green.png");
            button->caption(wxT("true"));
            break;
        }
    }
    else
    {
        switch (type)
        {
        case 'N':
            button->image("images/16x16/light_green.png");
            button->caption(wxT("local values"));
            break;
        case 'P':
            button->image("images/16x16/tag.png");
            button->caption(wxT("DENS"));
            break;
        default:
            button->image("images/16x16/light_orange.png");
            button->caption(wxT("false"));
            break;
        }
    }
}

AdequacyPatchOptions::AdequacyPatchOptions(wxWindow* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Adequacy Patch Options"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN),
 pTargetRef(nullptr)
{
    assert(parent);

    // Title of the Form
    SetLabel(wxT("Adequacy Patch Options"));
    SetTitle(wxT("Adequacy Patch Options"));

    auto& study = *Data::Study::Current::Get();

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

    {
        label = Component::CreateLabel(this, wxT("Enable Adequacy patch"));
        button = new Component::Button(this, wxT("true"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &AdequacyPatchOptions::onPopupMenuSpecify,
                     PopupInfo(study.parameters.adqPatch.enabled, wxT("true")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnAdequacyPatch = button;
    }
    // Transmission capacities (NTC) from physical areas outside adequacy patch (area type 1) to
    // physical areas inside adequacy patch (area type 2). Used in the first step of adequacy patch
    // local matching rule.
    {
        label = Component::CreateLabel(
          this, wxT("NTC from physical areas outside to physical areas inside adequacy patch"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &AdequacyPatchOptions::onPopupMenuNTC,
                     PopupInfo(study.parameters.adqPatch.localMatching.setToZeroOutsideInsideLinks,
                               wxT("NTC")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnNTCfromOutToInAdqPatch = button;
    }
    // Transmission capacities (NTC) between physical areas outside adequacy patch (area type 1).
    // Used in the first step of adequacy patch local matching rule.
    {
        label = Component::CreateLabel(this, wxT("NTC between physical areas outside adequacy patch"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this,
                     &AdequacyPatchOptions::onPopupMenuNTC,
                     PopupInfo(study.parameters.adqPatch.localMatching.setToZeroOutsideOutsideLinks,
                               wxT("NTC")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnNTCfromOutToOutAdqPatch = button;
    }
    // PTO (Price Taking Order). User can choose between DENS and Load
    {
        label = Component::CreateLabel(this, wxT("Price taking order"));
        button = new Component::Button(this, wxT("Day"), "images/16x16/tag.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(this, &AdequacyPatchOptions::onPopupMenuPTO);
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnAdequacyPatchPTO = button;
    }
    // Select whether the hurdle cost will be included into curtailment sharing rule cost function
    {
        label = Component::CreateLabel(this, wxT("Include hurdle cost in CSR optimization"));
        button = new Component::Button(this, wxT("true"), "images/16x16/light_green.png");
        button->SetBackgroundColour(bgColor);
        button->menu(true);
        onPopup.bind(
          this,
          &AdequacyPatchOptions::onPopupMenuSpecify,
          PopupInfo(study.parameters.adqPatch.curtailmentSharing.includeHurdleCost, wxT("true")));
        button->onPopupMenu(onPopup);
        s->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        s->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pBtnAdequacyPatchIncludeHurdleCostCsr = button;
    }
    addLabelAdqPatch(this, s, wxT("Thresholds"));
    // Threshold values
    {
        pThresholdCSRStart = nullptr;
        pThresholdLMRviolations = nullptr;
        pThresholdCSRStart
          = insertEdit(this,
                       s,
                       wxStringFromUTF8("Initiate curtailment sharing rule"),
                       wxCommandEventHandler(AdequacyPatchOptions::onEditThresholds));
        pThresholdLMRviolations
          = insertEdit(this,
                       s,
                       wxStringFromUTF8("Display local matching rule violations"),
                       wxCommandEventHandler(AdequacyPatchOptions::onEditThresholds));
    }

    {
        s->AddSpacer(2);
        s->Add(105, 2);
    }

    sizer->AddSpacer(10);

    // Buttons
    auto panel = new Component::Panel(this);
    panel->SetBackgroundColour(defaultBgColor);
    auto pnlSizerBtns = new wxBoxSizer(wxHORIZONTAL);
    auto pnlSizerBtnsV = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(pnlSizerBtnsV);
    pnlSizerBtnsV->AddSpacer(8);
    pnlSizerBtnsV->Add(pnlSizerBtns, 1, wxALL | wxEXPAND);
    pnlSizerBtnsV->AddSpacer(8);

    button = new Component::Button(panel,
                                   wxT("Reset to default"),
                                   "images/16x16/param_reset.png",
                                   this,
                                   &AdequacyPatchOptions::onResetToDefault);
    pnlSizerBtns->Add(22, 5);
    pnlSizerBtns->Add(button, 0, wxALL | wxEXPAND);

    pnlSizerBtns->AddStretchSpacer();
    wxButton* pBtnCancel
      = Antares::Component::CreateButton(panel, wxT("  Close  "), this, &AdequacyPatchOptions::onClose);
    pBtnCancel->SetDefault();
    pnlSizerBtns->Add(pBtnCancel, 0, wxALL | wxEXPAND);
    pnlSizerBtns->Add(25, 5);

    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND);
    sizer->Add(panel, 0, wxALL | wxEXPAND);

    // refresh
    Connect(GetId(), wxEVT_MOTION, wxMouseEventHandler(AdequacyPatchOptions::onInternalMotion), nullptr, this);

    refresh();
    SetSizer(sizer);
    sizer->Layout();

    // Recentre the window
    sizer->Fit(this);
    Centre(wxBOTH);
}

AdequacyPatchOptions::~AdequacyPatchOptions() = default;

void AdequacyPatchOptions::onClose(const void*)
{
    Dispatcher::GUI::Close(this);
}

void AdequacyPatchOptions::onResetToDefault(void*)
{
    auto studyptr = Data::Study::Current::Get();
    if (!(!studyptr))
    {
        Window::Message message(this,
                                wxT("Adequacy Patch Options"),
                                wxT("Adequacy Patch Options"),
                                wxT("Do you really want to reset to the default values ?"));
        message.add(Window::Message::btnContinue);
        message.add(Window::Message::btnCancel, true);

        if (message.showModal() == Window::Message::btnContinue)
        {
            auto& study = *studyptr;
            study.parameters.resetAdqPatchParameters();
            refresh();
            MarkTheStudyAsModified();
            return;
        }
    }
}

void AdequacyPatchOptions::onInternalMotion(wxMouseEvent&)
{
    Component::Panel::OnMouseMoveFromExternalComponent();
}

void AdequacyPatchOptions::refresh()
{
    auto studyptr = Data::Study::Current::Get();
    if (!studyptr)
        return;
    // The current study
    const auto& study = *studyptr;

    // Adequacy patch
    std::string buttonType = "specify";
    // Include adequacy patch
    updateButton(pBtnAdequacyPatch, study.parameters.adqPatch.enabled, buttonType);
    // Include hurdle cost for CSR
    updateButton(pBtnAdequacyPatchIncludeHurdleCostCsr,
                 study.parameters.adqPatch.curtailmentSharing.includeHurdleCost,
                 buttonType);
    // NTC from physical areas outside adequacy patch (area type 1) to physical areas inside
    // adequacy patch (area type 2). Used in the first step of adequacy patch local matching rule.
    buttonType = "ntc";
    updateButton(pBtnNTCfromOutToInAdqPatch,
                 study.parameters.adqPatch.localMatching.setToZeroOutsideInsideLinks,
                 buttonType);
    // NTC between physical areas outside adequacy patch (area type 1). Used in the first step of
    // adequacy patch local matching rule.
    updateButton(pBtnNTCfromOutToOutAdqPatch,
                 study.parameters.adqPatch.localMatching.setToZeroOutsideOutsideLinks,
                 buttonType);
    // Price taking order (PTO) for adequacy patch
    buttonType = "pto";
    bool isPTOload
      = (study.parameters.adqPatch.curtailmentSharing.priceTakingOrder == AdqPatchPTO::isLoad) ? true : false;
    updateButton(pBtnAdequacyPatchPTO, isPTOload, buttonType);

    // Threshold values
    {
        if (pThresholdCSRStart)
            pThresholdCSRStart->SetValue(
              wxString() << study.parameters.adqPatch.curtailmentSharing.thresholdInitiate);
        if (pThresholdLMRviolations)
            pThresholdLMRviolations->SetValue(
              wxString() << study.parameters.adqPatch.curtailmentSharing.thresholdDisplayViolations);
    }
}

void AdequacyPatchOptions::onPopupMenu(Component::Button&, wxMenu& menu, void*, const PopupInfo& info)
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
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectModeInclude),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("ignore"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectModeIgnore),
                 nullptr,
                 this);
}

void AdequacyPatchOptions::onPopupMenuNTC(Component::Button&,
                                            wxMenu& menu,
                                            void*,
                                            const PopupInfo& info)
{
    pTargetRef = &info.rval;
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxString() << wxT("set to null"),
                          "images/16x16/light_orange.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectModeInclude),
                 nullptr,
                 this);
    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxString() << wxT("local values (") << info.text << wxT(")"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectModeIgnore),
                 nullptr,
                 this);
}

void AdequacyPatchOptions::onPopupMenuPTO(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxString() << wxT("DENS"), "images/16x16/tag.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectPtoIsDens),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Load"), "images/16x16/tag.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectPtoIsLoad),
                 nullptr,
                 this);
}

void AdequacyPatchOptions::onPopupMenuSpecify(Component::Button&,
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
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectModeInclude),
                 nullptr,
                 this);
    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("false"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(AdequacyPatchOptions::onSelectModeIgnore),
                 nullptr,
                 this);
}

void AdequacyPatchOptions::onSelectModeInclude(wxCommandEvent&)
{
    if (pTargetRef and !*pTargetRef)
    {
        *pTargetRef = true;
        MarkTheStudyAsModified();
        refresh();
    }
    pTargetRef = nullptr;
}

void AdequacyPatchOptions::onSelectModeIgnore(wxCommandEvent&)
{
    if (pTargetRef and *pTargetRef)
    {
        *pTargetRef = false;
        MarkTheStudyAsModified();
        refresh();
    }
    pTargetRef = nullptr;
}

void AdequacyPatchOptions::onSelectPtoIsDens(wxCommandEvent&)
{
    auto study = Data::Study::Current::Get();
    if ((!(!study))
        && (study->parameters.adqPatch.curtailmentSharing.priceTakingOrder != AdqPatchPTO::isDens))
    {
        study->parameters.adqPatch.curtailmentSharing.priceTakingOrder = AdqPatchPTO::isDens;
        refresh();
        MarkTheStudyAsModified();
    }
}

void AdequacyPatchOptions::onSelectPtoIsLoad(wxCommandEvent&)
{
    auto study = Data::Study::Current::Get();
    if ((!(!study))
        && (study->parameters.adqPatch.curtailmentSharing.priceTakingOrder != AdqPatchPTO::isLoad))
    {
        study->parameters.adqPatch.curtailmentSharing.priceTakingOrder = AdqPatchPTO::isLoad;
        refresh();
        MarkTheStudyAsModified();
    }
}

wxTextCtrl* AdequacyPatchOptions::insertEdit(wxWindow* parent,
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

void AdequacyPatchOptions::onEditThresholds(wxCommandEvent& evt)
{
    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

    int id = evt.GetId();

    if (pThresholdCSRStart && id == pThresholdCSRStart->GetId())
    {
        String text;
        wxStringToString(pThresholdCSRStart->GetValue(), text);

        float newthreshold;
        if (not text.to(newthreshold))
        {
            logs.error() << "impossible to update the seed for '"
                         << "Initiate curtailment sharing rule"
                         << "'";
        }
        else
        {
            if (newthreshold != study.parameters.adqPatch.curtailmentSharing.thresholdInitiate)
            {
                study.parameters.adqPatch.curtailmentSharing.thresholdInitiate = newthreshold;
                MarkTheStudyAsModified();
            }
        }
        return;
    }

    if (pThresholdLMRviolations && id == pThresholdLMRviolations->GetId())
    {
        String text;
        wxStringToString(pThresholdLMRviolations->GetValue(), text);

        float newthreshold;
        if (not text.to(newthreshold))
        {
            logs.error() << "impossible to update the seed for '"
                         << "Display local matching rule violations"
                         << "'";
        }
        else
        {
            if (newthreshold
                != study.parameters.adqPatch.curtailmentSharing.thresholdDisplayViolations)
            {
                study.parameters.adqPatch.curtailmentSharing.thresholdDisplayViolations = newthreshold;
                MarkTheStudyAsModified();
            }
        }
        return;
    }
}

}
