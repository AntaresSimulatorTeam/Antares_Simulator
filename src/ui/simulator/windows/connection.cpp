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

#include "connection.h"
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include "../toolbox/validator.h"
#include "../toolbox/create.h"
#include <ui/common/component/panel.h>
#include <wx/sizer.h>
#include "../application/menus.h"
#include <wx/textdlg.h>
#include <wx/statline.h>
#include <antares/memory/new_check.hxx>

using namespace Yuni;
using namespace Antares::MemoryUtils;

namespace Antares
{
namespace Window
{
void linkParametersGrid::add(wxBoxSizer* sizer,
                             wxWindow* parent,
                             Interconnection* intercoWindow,
                             Toolbox::InputSelector::Connections* notifier)
{
    auto* connParam = new_check_allocation<Component::Datagrid::Renderer::connectionParameters>(
      intercoWindow, notifier);
    auto* component = new_check_allocation<Component::Datagrid::Component>(parent, connParam);
    sizer->Add(component, 1, wxALL | wxEXPAND | wxFIXED_MINSIZE);
}

void linkNTCgrid::add(wxBoxSizer* sizer,
                      wxWindow* parent,
                      Interconnection* intercoWindow,
                      Toolbox::InputSelector::Connections* notifier)
{
    // Size proportion of the current sizer's child among all sizer's children.
    // Here, all children have the same proportion.
    int gridSizeProportion = 1;
    // Size of the border around a grid, inside the sizer.
    int borderSizeAroundGrid = 5;
    Component::Datagrid::Component* gridDirect = nullptr;
    Component::Datagrid::Component* gridIndirect = nullptr;
    // Grid for direct NTC
    auto* connectionDirect
      = new_check_allocation<Component::Datagrid::Renderer::connectionNTCdirect>(intercoWindow,
                                                                                 notifier);
    gridDirect = new_check_allocation<Component::Datagrid::Component>(
      parent, connectionDirect, wxT("Direct"));

    sizer->Add(
      gridDirect, gridSizeProportion, wxALL | wxEXPAND | wxFIXED_MINSIZE, borderSizeAroundGrid);

    // Vertical separator
    sizer->Add(new_check_allocation<wxStaticLine>(
                 parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL),
               0,
               wxALL | wxEXPAND);

    // Grid for indirect NTC
    auto* connectionIndirect
      = new_check_allocation<Component::Datagrid::Renderer::connectionNTCindirect>(intercoWindow,
                                                                                   notifier);
    gridIndirect = new_check_allocation<Component::Datagrid::Component>(
      parent, connectionIndirect, wxT("Indirect"));

    sizer->Add(
      gridIndirect, gridSizeProportion, wxALL | wxEXPAND | wxFIXED_MINSIZE, borderSizeAroundGrid);

    // Synchronize the scroll bars of the two grids
    gridDirect->setOtherGrid(gridIndirect);
    gridIndirect->setOtherGrid(gridDirect);
}

// Events to update a link property in all Interconnection objects (upper banner for any link view) 
Yuni::Event<void(const Antares::Data::AreaLink*)> onTransmissionCapacitiesUsageChanges;
Yuni::Event<void(const Antares::Data::AreaLink*)> onHurdleCostsUsageChanges;
Yuni::Event<void(const Antares::Data::AreaLink*)> onAssetTypeChanges;
Yuni::Event<void(const Antares::Data::AreaLink*)> onLinkCaptionChanges;


Interconnection::Interconnection(wxWindow* parent,
                                 Toolbox::InputSelector::Connections* notifier,
                                 linkGrid* link_grid) :
 wxScrolledWindow(parent),
 pLink(nullptr),
 pLinkName(),
 pHurdlesCost(nullptr),
 pLoopFlow(nullptr),
 pPhaseShift(nullptr),
 pCopperPlate(nullptr),
 pAssetType(nullptr)
{
    auto* mainsizer = new_check_allocation<wxBoxSizer>(wxVERTICAL);
    SetSizer(mainsizer);

    pNoLink = Component::CreateLabel(this, wxT("No link selected"));
    pLinkData = new_check_allocation<Component::Panel>(this);
    mainsizer->Add(pNoLink, 1, wxALL | wxALIGN_CENTER);
    mainsizer->Add(pLinkData, 1, wxALL | wxEXPAND);
    mainsizer->Hide(pLinkData);

    auto* sizer_vertical = new_check_allocation<wxBoxSizer>(wxVERTICAL);
    pLinkData->SetSizer(sizer_vertical);

    wxFlexGridSizer* sizer_flex_grid = new_check_allocation<wxFlexGridSizer>(0, 0, 10);
    pGridSizer = sizer_flex_grid;
    sizer_flex_grid->AddGrowableCol(1, 0);
    auto* gridHZ = new_check_allocation<wxBoxSizer>(wxHORIZONTAL);
    gridHZ->AddSpacer(20);
    gridHZ->Add(sizer_flex_grid, 0, wxALL | wxEXPAND);
    sizer_vertical->Add(gridHZ, 0, wxALL | wxEXPAND, 6);

    wxStaticText* label;
    Component::Button* button;
    Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)> onPopup;

    // Binding constraints
    {
        label = Component::CreateLabel(pLinkData, wxT("Link"), false, true);
        button = new_check_allocation<Component::Button>(
          pLinkData, wxT("local values"), "images/16x16/link.png");
        button->menu(true);
        button->bold(true);
        onPopup.bind(this, &Interconnection::onPopupMenuLink);
        button->onPopupMenu(onPopup);
        pLinkName = button;
        sizer_flex_grid->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        sizer_flex_grid->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }
    // Caption
    onLinkCaptionChanges.connect(this, &Interconnection::updateLinkCaption);
    {
        label = Component::CreateLabel(pLinkData, wxT("Caption"), false, true);
        button = new_check_allocation<Component::Button>(pLinkData,
                                                         wxT(""),
                                                         "images/16x16/document.png",
                                                         this,
                                                         &Interconnection::onButtonEditCaption);
        auto* lhz = new_check_allocation<wxBoxSizer>(wxHORIZONTAL);
        pCaptionText = Component::CreateLabel(pLinkData, wxEmptyString);
        sizer_flex_grid->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        lhz->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        lhz->AddSpacer(2);
        lhz->Add(pCaptionText, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        sizer_flex_grid->Add(lhz, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

        pLabelCaption = label;
        pCaptionDataSizer = lhz;
    }

    // Hurdle costs
    onHurdleCostsUsageChanges.connect(this, &Interconnection::updateHurdleCostsUsage);
    {
        label = Component::CreateLabel(pLinkData, wxT("Local values"), false, true);
        button = new_check_allocation<Component::Button>(
          pLinkData, wxT("local values"), "images/16x16/light_green.png");
        button->menu(true);
        onPopup.bind(this, &Interconnection::onPopupMenuHurdlesCosts);
        button->onPopupMenu(onPopup);
        sizer_flex_grid->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        sizer_flex_grid->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pHurdlesCost = button;
    }

    // Transmission capacities
    onTransmissionCapacitiesUsageChanges.connect(this, &Interconnection::updateTransmissionCapacityUsage);
    {
        button = new_check_allocation<Component::Button>(
          pLinkData, wxT("Transmission capacities"), "images/16x16/light_green.png");
        button->menu(true);
        onPopup.bind(this, &Interconnection::onPopupMenuTransmissionCapacities);
        button->onPopupMenu(onPopup);
        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pCopperPlate = button;
    }
    // Asset Type
    onAssetTypeChanges.connect(this, &Interconnection::updateAssetType);
    {
        button = new_check_allocation<Component::Button>(
          pLinkData, wxT("Asset type"), "images/16x16/light_green.png");
        button->menu(true);
        onPopup.bind(this, &Interconnection::onPopupMenuAssetType);
        button->onPopupMenu(onPopup);
        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pAssetType = button;
    }
    // Loop flow
    {
        button = new_check_allocation<Component::Button>(
          pLinkData, wxT("loop flow"), "images/16x16/light_green.png");
        sizer_flex_grid->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pLoopFlow = button;
    }
    // Phase Shifter
    {
        button = new_check_allocation<Component::Button>(
          pLinkData, wxT("phase shifter"), "images/16x16/light_green.png");
        sizer_flex_grid->Add(button, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        pPhaseShift = button;
    }

    sizer_vertical->AddSpacer(4);

    auto* subGridPanel = new_check_allocation<Component::Panel>(pLinkData);
    subGridPanel->SetSize(1, 1);
    subGridPanel->SetBackgroundColour(wxColour(200, 200, 200));
    sizer_vertical->Add(subGridPanel, 0, wxALL | wxEXPAND, 4);

    // Interconnection grid
    auto* sub_sizer_horizontal = new_check_allocation<wxBoxSizer>(wxHORIZONTAL);
    link_grid->add(sub_sizer_horizontal, pLinkData, this, notifier);
    sizer_vertical->Add(sub_sizer_horizontal, 1, wxALL | wxEXPAND);

    sizer_vertical->Layout();

    mainsizer->Layout();

    if (notifier)
        notifier->onConnectionChanged.connect(this, &Interconnection::onConnectionChanged);
    // inspector
    OnStudyLinkChanged.connect(this, &Interconnection::onStudyLinkChanged);
}

Interconnection::~Interconnection()
{
    pLink = nullptr;
    destroyBoundEvents();
}

void Interconnection::onConnectionChanged(Data::AreaLink* link)
{
    auto* sizer = GetSizer();
    if (not sizer)
        return;

    if (not pLinkName || not pHurdlesCost || not pCopperPlate)
    {
        pLink = nullptr;
        sizer->Hide(pLinkData);
        sizer->Show(pNoLink);
        return;
    }

    if (not link)
    {
        if (not pLink) // already well set - avoid useless refresh
            return;
        pLinkName->caption(wxEmptyString);
        pLink = nullptr;
        sizer->Hide(pLinkData);
        sizer->Show(pNoLink);
    }
    else
    {
        assert(link->from);
        assert(link->with);

        pLink = link;
        sizer->Show(pLinkData);
        sizer->Hide(pNoLink);

        pLinkName->caption(wxStringFromUTF8(link->from->name)
                           << wxT("  /  ") << wxStringFromUTF8(link->with->name));

        updateLinkCaption(link);

        updateHurdleCostsUsage(link);

        updateLoopFlowUsage(link);

        updatePhaseShifter(link);

        updateTransmissionCapacityUsage(link);

        updateAssetType(link);
    }

    sizer->Layout();
    this->FitInside(); // ask the sizer about the needed size
    this->SetScrollRate(5, 5);
}

void Interconnection::updatePhaseShifter(const Data::AreaLink* link)
{
    if (link->usePST)
    {
        pPhaseShift->caption(wxT("Tune PST"));
        pPhaseShift->image("images/16x16/light_green.png");
    }
    else
    {
        pPhaseShift->caption(wxT("Ignore PST "));
        pPhaseShift->image("images/16x16/light_orange.png");
    }
}

void Interconnection::updateLoopFlowUsage(const Data::AreaLink* link)
{
    if (link->useLoopFlow)
    {
        pLoopFlow->caption(wxT("Account for loop flows"));
        pLoopFlow->image("images/16x16/light_green.png");
    }
    else
    {
        pLoopFlow->caption(wxT("Ignore loop flows"));
        pLoopFlow->image("images/16x16/light_orange.png");
    }
}

void Interconnection::updateLinkCaption(const Data::AreaLink* link)
{
    if (link->comments.empty())
    {
        pGridSizer->Hide(pLabelCaption);
        pGridSizer->Hide(pCaptionDataSizer);
    }
    else
    {
        pCaptionText->SetLabel(wxStringFromUTF8(link->comments));
        pGridSizer->Show(pLabelCaption);
        pGridSizer->Show(pCaptionDataSizer);
    }
}

void Interconnection::updateTransmissionCapacityUsage(const Data::AreaLink* link)
{
    switch (link->transmissionCapacities)
    {
    case Data::tncEnabled:
        pCopperPlate->caption(wxT("Use transmission capacities"));
        pCopperPlate->image("images/16x16/light_green.png");
        break;
    case Data::tncIgnore:
        pCopperPlate->caption(wxT("Set transmission capacities to null"));
        pCopperPlate->image("images/16x16/light_orange.png");
        break;
    case Data::tncInfinite:
        pCopperPlate->caption(wxT("Set transmission capacities to infinite"));
        pCopperPlate->image("images/16x16/infinity.png");
        break;
    }
}

void Interconnection::updateHurdleCostsUsage(const Data::AreaLink* link)
{
    if (link->useHurdlesCost)
    {
        pHurdlesCost->caption(wxT("Use hurdles costs"));
        pHurdlesCost->image("images/16x16/light_green.png");
    }
    else
    {
        pHurdlesCost->caption(wxT("Ignore hurdles costs"));
        pHurdlesCost->image("images/16x16/light_orange.png");
    }
}

void Interconnection::updateAssetType(const Data::AreaLink* link)
{
    switch (link->assetType)
    {
    case Data::atAC:
        pAssetType->caption(wxT("Asset type: AC"));
        pAssetType->image("images/16x16/light_green.png");
        break;
    case Data::atDC:
        pAssetType->caption(wxT("Asset type: DC"));
        pAssetType->image("images/16x16/light_orange.png");
        break;
    case Data::atGas:
        pAssetType->caption(wxT("Asset type: Gas"));
        pAssetType->image("images/16x16/light_orange.png");
        break;
    case Data::atVirt:
        pAssetType->caption(wxT("Asset type: Virtual"));
        pAssetType->image("images/16x16/light_orange.png");
        break;
    case Data::atOther:
        pAssetType->caption(wxT("Asset type: other"));
        pAssetType->image("images/16x16/light_orange.png");
        break;
    }
}

void Interconnection::onPopupMenuTransmissionCapacities(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(&menu,
                          wxID_ANY,
                          wxT("Use transmission capacities"),
                          "images/16x16/light_green.png",
                          wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectTransCapInclude),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to null"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectTransCapIgnore),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to infinite"), "images/16x16/infinity.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectTransCapInfinite),
                 nullptr,
                 this);
}

void Interconnection::onSelectTransCapInclude(wxCommandEvent&)
{
    if (pLink && pLink->transmissionCapacities != Data::tncEnabled)
    {
        pLink->transmissionCapacities = Data::tncEnabled;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onTransmissionCapacitiesUsageChanges(pLink);
    }
}

void Interconnection::onSelectTransCapIgnore(wxCommandEvent&)
{
    if (pLink && pLink->transmissionCapacities != Data::tncIgnore)
    {
        pLink->transmissionCapacities = Data::tncIgnore;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onTransmissionCapacitiesUsageChanges(pLink);
    }
}

void Interconnection::onSelectTransCapInfinite(wxCommandEvent&)
{
    if (pLink && pLink->transmissionCapacities != Data::tncInfinite)
    {
        pLink->transmissionCapacities = Data::tncInfinite;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
    }
}

void Interconnection::onPopupMenuAssetType(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to AC"), "images/16x16/light_green.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectAssetTypeAC),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to DC"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectAssetTypeDC),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to Gas"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectAssetTypeGas),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to Virt"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectAssetTypeVirt),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Set to other"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectAssetTypeOther),
                 nullptr,
                 this);
}

void Interconnection::onSelectAssetTypeAC(wxCommandEvent&)
{
    if (pLink && pLink->assetType != Data::atAC)
    {
        pLink->assetType = Data::atAC;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onAssetTypeChanges(pLink);
        pLink->color[0] = 112;
        pLink->color[1] = 112;
        pLink->color[2] = 112;
        pLink->style = Data::stPlain;
        pLink->linkWidth = 1;
    }
}

void Interconnection::onSelectAssetTypeDC(wxCommandEvent&)
{
    if (pLink && pLink->assetType != Data::atDC)
    {
        pLink->assetType = Data::atDC;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onAssetTypeChanges(pLink);
        pLink->color[0] = 0;
        pLink->color[1] = 255;
        pLink->color[2] = 0;
        pLink->style = Data::stDash;
        pLink->linkWidth = 2;
    }
}

void Interconnection::onSelectAssetTypeGas(wxCommandEvent&)
{
    if (pLink && pLink->assetType != Data::atGas)
    {
        pLink->assetType = Data::atGas;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onAssetTypeChanges(pLink);
        pLink->color[0] = 0;
        pLink->color[1] = 128;
        pLink->color[2] = 255;
        pLink->style = Data::stPlain;
        pLink->linkWidth = 3;
    }
}

void Interconnection::onSelectAssetTypeVirt(wxCommandEvent&)
{
    if (pLink && pLink->assetType != Data::atVirt)
    {
        pLink->assetType = Data::atVirt;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onAssetTypeChanges(pLink);
        pLink->color[0] = 255;
        pLink->color[1] = 0;
        pLink->color[2] = 128;
        pLink->style = Data::stDotDash;
        pLink->linkWidth = 2;
    }
}

void Interconnection::onSelectAssetTypeOther(wxCommandEvent&)
{
    if (pLink && pLink->assetType != Data::tncInfinite)
    {
        pLink->assetType = Data::atOther;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onAssetTypeChanges(pLink);
        pLink->color[0] = 255;
        pLink->color[1] = 128;
        pLink->color[2] = 0;
        pLink->style = Data::stDot;
        pLink->linkWidth = 2;
    }
}

void Interconnection::onPopupMenuHurdlesCosts(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Use hurdles costs"), "images/16x16/light_green.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectIncludeHurdlesCosts),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Ignore"), "images/16x16/light_orange.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onSelectIgnoreHurdlesCosts),
                 nullptr,
                 this);
}

void Interconnection::onSelectIncludeHurdlesCosts(wxCommandEvent&)
{
    if (pLink && not pLink->useHurdlesCost)
    {
        pLink->useHurdlesCost = true;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onHurdleCostsUsageChanges(pLink);
    }
}

void Interconnection::onSelectIgnoreHurdlesCosts(wxCommandEvent&)
{
    if (pLink && pLink->useHurdlesCost)
    {
        pLink->useHurdlesCost = false;
        onConnectionChanged(pLink);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
        onHurdleCostsUsageChanges(pLink);
    }
}

void Interconnection::onPopupMenuLink(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Edit caption"), "images/16x16/document.png", wxEmptyString);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Interconnection::onEditCaption),
                 nullptr,
                 this);
}

void Interconnection::onEditCaption(wxCommandEvent&)
{
    onButtonEditCaption(nullptr);
}

void Interconnection::onButtonEditCaption(void*)
{
    if (not pLink)
        return;

    wxTextEntryDialog dialog(this,
                             wxT("Please enter the new link's caption :"),
                             wxT("Caption"),
                             wxStringFromUTF8(pLink->comments),
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
        String text;
        wxStringToString(dialog.GetValue(), text);
        text.trim();
        if (text != pLink->comments)
        {
            pLink->comments = text;
            MarkTheStudyAsModified();
            onConnectionChanged(pLink);
            OnInspectorRefresh(nullptr);
            onLinkCaptionChanges(pLink);
        }
    }
}

void Interconnection::onStudyLinkChanged(Data::AreaLink* link)
{
    if (link == pLink && link)
        onConnectionChanged(pLink);
}

} // namespace Window
} // namespace Antares
