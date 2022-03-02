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
Yuni::Event<void(const Antares::Data::AreaLink*)> onHurdleCostsUsageChanges;
Yuni::Event<void(const Antares::Data::AreaLink*)> onAssetTypeChanges;


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
 pHurdlesCost(nullptr),
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
    sizer_flex_grid->AddGrowableCol(1, 0);
    auto* sizer_horizontal = new_check_allocation<wxBoxSizer>(wxHORIZONTAL);
    sizer_horizontal->AddSpacer(20);
    sizer_horizontal->Add(sizer_flex_grid, 0, wxALL | wxEXPAND);
    sizer_vertical->Add(sizer_horizontal, 0, wxALL | wxEXPAND, 6);

    Component::Button* button;
    Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)> onPopup;

    // Label "Link"
    wxStaticText* label = Component::CreateLabel(pLinkData, wxT("Link"), false, true);
    sizer_flex_grid->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    // Link caption button
    captionButton_ = new captionButton(pLinkData, sizer_flex_grid);

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

    // Link transmission capacities usage button
    ntcUsageButton_ = new ntcUsageButton(pLinkData, sizer_flex_grid);

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
    
    // Loop flow usage button
    loopFlowUsageButton_ = new loopFlowUsageButton(pLinkData, sizer_flex_grid);

    // Phase Shifter usage button
    phaseShifterUsageButton_ = new phaseShifterUsageButton(pLinkData, sizer_flex_grid);

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
    if (checkLinkView(link))
        updateLinkView(link);
}

bool Interconnection::checkLinkView(Data::AreaLink* link)
{
    auto* sizer = GetSizer();
    if (not sizer)
        return false;

    if (captionButton_->isEmpty() || not pHurdlesCost || ntcUsageButton_->isEmpty())
    {
        pLink = nullptr;
        sizer->Hide(pLinkData);
        sizer->Show(pNoLink);
        return false;
    }

    if (not link)
    {
        if (not pLink) // already well set - avoid useless refresh
            return false;
        captionButton_->setCaption(wxEmptyString);
        pLink = nullptr;
        sizer->Hide(pLinkData);
        sizer->Show(pNoLink);
        finalizeView();
        return false;
    }

    return true;
}


void Interconnection::updateLinkView(Data::AreaLink* link)
{
    assert(link->from);
    assert(link->with);

    auto* sizer = GetSizer();
    if (not sizer)
        return;

    pLink = link;
    sizer->Show(pLinkData);
    sizer->Hide(pNoLink);

    wxString linkCaption = wxStringFromUTF8(link->from->name) << wxT("  /  ") << wxStringFromUTF8(link->with->name);
    captionButton_->setCaption(linkCaption);
    captionButton_->update(link);

    updateHurdleCostsUsage(link);

    loopFlowUsageButton_->update(link);

    phaseShifterUsageButton_->update(link);

    ntcUsageButton_->update(link);

    updateAssetType(link);

    finalizeView();
}

void Interconnection::finalizeView()
{
    auto* sizer = GetSizer();
    if (not sizer)
        return;

    sizer->Layout();
    this->FitInside(); // ask the sizer about the needed size
    this->SetScrollRate(5, 5);
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
    if (!pLink)
        return;

    pLink->assetType = Data::atAC;
    onAssetTypeChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
    pLink->color[0] = 112;
    pLink->color[1] = 112;
    pLink->color[2] = 112;
    pLink->style = Data::stPlain;
    pLink->linkWidth = 1;
}

void Interconnection::onSelectAssetTypeDC(wxCommandEvent&)
{
    if (!pLink)
        return;

    pLink->assetType = Data::atDC;
    onAssetTypeChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
    pLink->color[0] = 0;
    pLink->color[1] = 255;
    pLink->color[2] = 0;
    pLink->style = Data::stDash;
    pLink->linkWidth = 2;
}

void Interconnection::onSelectAssetTypeGas(wxCommandEvent&)
{
    if (!pLink)
        return;

    pLink->assetType = Data::atGas;
    onAssetTypeChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
    pLink->color[0] = 0;
    pLink->color[1] = 128;
    pLink->color[2] = 255;
    pLink->style = Data::stPlain;
    pLink->linkWidth = 3;
}

void Interconnection::onSelectAssetTypeVirt(wxCommandEvent&)
{
    if (!pLink)
        return;

    pLink->assetType = Data::atVirt;
    onAssetTypeChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
    pLink->color[0] = 255;
    pLink->color[1] = 0;
    pLink->color[2] = 128;
    pLink->style = Data::stDotDash;
    pLink->linkWidth = 2;
}

void Interconnection::onSelectAssetTypeOther(wxCommandEvent&)
{
    if (!pLink)
        return;

    pLink->assetType = Data::atOther;
    onAssetTypeChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
    pLink->color[0] = 255;
    pLink->color[1] = 128;
    pLink->color[2] = 0;
    pLink->style = Data::stDot;
    pLink->linkWidth = 2;
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
    if (!pLink)
        return;

    pLink->useHurdlesCost = true;
    onHurdleCostsUsageChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
}

void Interconnection::onSelectIgnoreHurdlesCosts(wxCommandEvent&)
{
    if (!pLink)
        return;

    pLink->useHurdlesCost = false;
    onHurdleCostsUsageChanges(pLink);
    MarkTheStudyAsModified();
    OnInspectorRefresh(nullptr);
}

void Interconnection::onStudyLinkChanged(Data::AreaLink* link)
{
    if (link == pLink && link)
        onConnectionChanged(pLink);
}

} // namespace Window
} // namespace Antares
