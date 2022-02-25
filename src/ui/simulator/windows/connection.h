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
#ifndef __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
#define __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__

#include <antares/wx-wrapper.h>
#include "../toolbox/components/datagrid/component.h"
#include "../toolbox/input/connection.h"
#include "../toolbox/components/datagrid/renderer/connection.h"
#include "../toolbox/components/button.h"
#include <wx/sizer.h>

#include "../application/menus.h"

namespace Antares
{
namespace Window
{
/*!
**
*/

// Forward declaration
class Interconnection;

class linkGrid
{
public:
    linkGrid() = default;
    virtual ~linkGrid() = default;
    virtual void add(wxBoxSizer* sizer,
                     wxWindow* parent,
                     Interconnection* intercoWindow,
                     Toolbox::InputSelector::Connections* notifier) = 0;
};

class linkParametersGrid : public linkGrid
{
public:
    linkParametersGrid() = default;
    ~linkParametersGrid() override = default;
    void add(wxBoxSizer* sizer,
             wxWindow* parent,
             Interconnection* intercoWindow,
             Toolbox::InputSelector::Connections* notifier) override;
};

class linkNTCgrid : public linkGrid
{
public:
    linkNTCgrid() = default;
    ~linkNTCgrid() override = default;
    void add(wxBoxSizer* sizer,
             wxWindow* parent,
             Interconnection* intercoWindow,
             Toolbox::InputSelector::Connections* notifier) override;
};

class NTCusage : public wxFrame, public Yuni::IEventObserver<NTCusage>
{
public:
    NTCusage(wxWindow* parent,
        Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)>& onPopup,
        wxFlexGridSizer* sizer_flex_grid)
    {
        onTransmissionCapacitiesUsageChanges.connect(this, &NTCusage::update);
        button_ = new Component::Button(parent, wxT("Transmission capacities"), "images/16x16/light_green.png");
        button_->menu(true);
        onPopup.bind(this, &NTCusage::onPopupMenuTransmissionCapacities);
        button_->onPopupMenu(onPopup);
        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    ~NTCusage() = default;

    void setCurrentLink(Data::AreaLink* link)
    {
        currentLink_ = link;
    }

    void update(const Data::AreaLink* link)
    {
        switch (link->transmissionCapacities)
        {
        case Data::tncEnabled:
            button_->caption(wxT("Use transmission capacities"));
            button_->image("images/16x16/light_green.png");
            break;
        case Data::tncIgnore:
            button_->caption(wxT("Set transmission capacities to null"));
            button_->image("images/16x16/light_orange.png");
            break;
        case Data::tncInfinite:
            button_->caption(wxT("Set transmission capacities to infinite"));
            button_->image("images/16x16/infinity.png");
            break;
        }
    }

    bool isEmpty()
    {
        return !button_;
    }

private:
    void onPopupMenuTransmissionCapacities(Component::Button&, wxMenu& menu, void*)
    {
        wxMenuItem* it;

        it = Menu::CreateItem(&menu,
            wxID_ANY,
            wxT("Use transmission capacities"),
            "images/16x16/light_green.png",
            wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(NTCusage::onSelectTransCapInclude),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to null"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(NTCusage::onSelectTransCapIgnore),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to infinite"), "images/16x16/infinity.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(NTCusage::onSelectTransCapInfinite),
            nullptr,
            this);
    }

    void onSelectTransCapInclude(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->transmissionCapacities = Data::tncEnabled;
        onTransmissionCapacitiesUsageChanges(currentLink_);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
    }

    void onSelectTransCapIgnore(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->transmissionCapacities = Data::tncIgnore;
        onTransmissionCapacitiesUsageChanges(currentLink_);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
    }

    void onSelectTransCapInfinite(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncInfinite;
        onTransmissionCapacitiesUsageChanges(currentLink_);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
    }

public:
    static Yuni::Event<void(const Antares::Data::AreaLink*)> onTransmissionCapacitiesUsageChanges;
private:
    Component::Button* button_ = nullptr;
    Data::AreaLink* currentLink_ = nullptr;
};

class Interconnection : public wxScrolledWindow, public Yuni::IEventObserver<Interconnection>
{
public:
    //! \name Constructor & Destructor
    //@{
    Interconnection(wxWindow* parent,
                    Toolbox::InputSelector::Connections* notifier,
                    linkGrid* link_grid);
    //! Destructor
    virtual ~Interconnection();
    //@}

private:
    /*!
    ** \brief Event: The selection of the connection has changed
    */
    void onConnectionChanged(Data::AreaLink* link);

    // void onPopupMenuTransmissionCapacities(Component::Button&, wxMenu& menu, void*);
    void onSelectTransCapInclude(wxCommandEvent&);
    void onSelectTransCapIgnore(wxCommandEvent&);
    void onSelectTransCapInfinite(wxCommandEvent&);

    void onPopupMenuAssetType(Component::Button&, wxMenu& menu, void*);
    void onSelectAssetTypeAC(wxCommandEvent&);
    void onSelectAssetTypeDC(wxCommandEvent&);
    void onSelectAssetTypeGas(wxCommandEvent&);
    void onSelectAssetTypeVirt(wxCommandEvent&);
    void onSelectAssetTypeOther(wxCommandEvent&);

    void onPopupMenuHurdlesCosts(Component::Button&, wxMenu& menu, void*);
    void onSelectIncludeHurdlesCosts(wxCommandEvent&);
    void onSelectIgnoreHurdlesCosts(wxCommandEvent&);

    void onPopupMenuLink(Component::Button&, wxMenu& menu, void*);
    void onEditCaption(wxCommandEvent&);
    void onButtonEditCaption(void*);

    void onStudyLinkChanged(Data::AreaLink* link);

    bool checkLinkView(Data::AreaLink* link);
    void updateLinkView(Data::AreaLink* link);
    void finalizeView();

    // For a given link, update transmission capacity usage 
    // void updateTransmissionCapacityUsage(const Data::AreaLink* link);
    // For a given link, update hurdle costs usage
    void updateHurdleCostsUsage(const Data::AreaLink* link);
    // For a given link, update asset type
    void updateAssetType(const Data::AreaLink* link);
    // For a given link, update its caption
    void updateLinkCaption(const Data::AreaLink* link);

    void updateLoopFlowUsage(const Data::AreaLink* link);
    void updatePhaseShifter(const Data::AreaLink* link);

private:
    //! Pointer to the current link
    Data::AreaLink* pLink;
    //! Button which display the name of the current link
    Component::Button* pLinkName;
    //! Hudrles costs
    Component::Button* pHurdlesCost;
    //! Loop flow
    Component::Button* pLoopFlow;
    // !Phase shifter
    Component::Button* pPhaseShift;
    //! Copper Plate
    // Component::Button* pCopperPlate;
    NTCusage* ntc_usage_ = nullptr;
    //! Asset type
    Component::Button* pAssetType;
    //! Caption
    wxWindow* pCaptionText;
    //! No Link
    wxWindow* pNoLink;
    //! Link data
    wxWindow* pLinkData;

    //! The main grid sizer
    wxSizer* pGridSizer;
    //!
    wxWindow* pLabelCaption;
    wxSizer* pCaptionDataSizer;

}; // class Interconnection

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
