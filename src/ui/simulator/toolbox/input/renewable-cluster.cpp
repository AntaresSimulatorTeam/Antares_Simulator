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

// #include <antares/wx-wrapper.h>
// #include "thermal-cluster.h"
// #include "../components/captionpanel.h"
#include "../../application/study.h"
#include "../../application/main.h"
#include "../../application/wait.h"
// #include "../../windows/inspector.h"
// #include <assert.h>
#include "../resources.h"
#include "../create.h"
// #include "../validator.h"
// #include "../components/htmllistbox/datasource/thermal-cluster.h"
// #include "../components/htmllistbox/item/thermal-cluster.h"
#include "../components/button.h"
// #include "../../windows/message.h"
// #include "../../application/menus.h"
// #include <antares/study/scenario-builder/updater.hxx>
#include <wx/wupdlock.h>
#include <wx/sizer.h>
// #include <wx/stattext.h>
// #include <wx/statline.h>
// #include <wx/bmpbuttn.h>
// #include <ui/common/lock.h>

#include "renewable-cluster.h"
#include "../components/htmllistbox/item/renewable-cluster.h"
#include "../components/htmllistbox/datasource/renewable-cluster.h"

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
    RenewableCluster::RenewableCluster(wxWindow* parent, InputSelector::Area* area) :
        AInput(parent), pArea(nullptr), pTotalMW(nullptr), pImageList(16, 16), pAreaNotifier(area)
{
    SetSize(300, 330);

    // Loading resources images
    wxBitmap* bmp = Resources::BitmapLoadFromFile("images/16x16/empty.png");
    pImageList.Add(*bmp);
    delete bmp;
    bmp = Resources::BitmapLoadFromFile("images/16x16/arrow_right.png");
    pImageList.Add(*bmp);
    delete bmp;

    // Create needed controls
    internalBuildSubControls();
    update();

    /*
    // Connect to the global event
    // Avoid SegV
    Forms::ApplWnd::Instance()->onApplicationQuit.connect(this,
                                                          &ThermalCluster::onApplicationOnQuit);
    OnStudyClosed.connect(this, &ThermalCluster::onStudyClosed);
    OnStudyEndUpdate.connect(this, &ThermalCluster::onStudyEndUpdate);
    */

    if (area)
        area->onAreaChanged.connect(this, &RenewableCluster::areaHasChanged);

    /*
    OnStudyThermalClusterRenamed.connect(this, &ThermalCluster::onStudyThermalClusterRenamed);
    OnStudyThermalClusterGroupChanged.connect(this,
                                              &ThermalCluster::onStudyThermalClusterGroupChanged);
    OnStudyThermalClusterCommonSettingsChanged.connect(
      this, &ThermalCluster::onStudyThermalClusterCommonSettingsChanged);
    */
}

RenewableCluster::~RenewableCluster()
{
    destroyBoundEvents();
}

namespace HTMLLsDatasourcesRn = Component::HTMLListbox::Datasource::RenewableClusters;


void RenewableCluster::internalBuildSubControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    Antares::Component::Button* btn;

    wxBoxSizer* toolSZ = new wxBoxSizer(wxHORIZONTAL);
    toolSZ->AddSpacer(10);

    // Create a new renewable cluster
    btn = new Antares::Component::Button(this,
                                         wxT("Create a cluster"),
                                         "images/16x16/thermal_add.png",
                                         this,
                                         &RenewableCluster::internalAddPlant);

    toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

    // Delete
    btn = new Antares::Component::Button(this,
                                         wxT("Delete"),
                                         "images/16x16/thermal_remove.png",
                                         this,
                                         &RenewableCluster::internalDeletePlant);
    btn->dropDown(true);
    btn->onPopupMenu(this, &RenewableCluster::onDeleteDropdown);
    toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

    // vertical line
    Antares::Component::AddVerticalSeparator(this, toolSZ);

    // Clone
    btn = new Antares::Component::Button(
      this, wxT("Clone"), "images/16x16/paste.png", this, &RenewableCluster::internalClonePlant);
    toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

    toolSZ->AddStretchSpacer();

    pTotalMW = Component::CreateLabel(this, wxT("Total : 0 MW"));
    toolSZ->Add(pTotalMW, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL);
    toolSZ->AddSpacer(15);
    toolSZ->Layout();

    sizer->AddSpacer(2);
    sizer->Add(toolSZ, 0, wxALL | wxEXPAND);
    sizer->AddSpacer(2);

    // The listbox
    pRnListbox = new Component::HTMLListbox::Component(this);
    
    HTMLLsDatasourcesRn::ByAlphaOrder* dsAZ;
    dsAZ = pRnListbox->addDatasource<HTMLLsDatasourcesRn::ByAlphaOrder>();
    HTMLLsDatasourcesRn::ByAlphaReverseOrder* dsZA;
    dsZA = pRnListbox->addDatasource<HTMLLsDatasourcesRn::ByAlphaReverseOrder>();

    if (pAreaNotifier)
    {
        pAreaNotifier->onAreaChanged.connect(dsAZ,
                                             &HTMLLsDatasourcesRn::ByAlphaOrder::onAreaChanged);
        pAreaNotifier->onAreaChanged.connect(
          dsZA, &HTMLLsDatasourcesRn::ByAlphaReverseOrder::onAreaChanged);
    }
    
    sizer->Add(pRnListbox, 1, wxALL | wxEXPAND);
    sizer->SetItemMinSize(pRnListbox, 100, 200);
    pRnListbox->onItemSelected.connect(this, &RenewableCluster::onRnSelected);

    // Update the layout
    GetSizer()->Layout();
}

void RenewableCluster::update()
{
    pRnListbox->invalidate();
    onClusterChanged(nullptr);
    updateInnerValues();
}

void RenewableCluster::updateInnerValues()
{
    if (pRnListbox)
        pRnListbox->updateHtmlContent();

    if (pArea)
    {
        double total = 0.;
        uint unitCount = 0;
        // pArea->thermal.list.retrieveTotalCapacityAndUnitCount(total, unitCount);
        pArea->renewable.list.retrieveTotalCapacity(total);

        // The total - installed capacity
        // pTotalMW->SetLabel(wxString() << unitCount << wxT(" units, ") << total << wxT(" MW"));
        pTotalMW->SetLabel(wxString() << total << wxT(" MW"));
    }
    else
        pTotalMW->SetLabel(wxEmptyString);

    // The layout must be updated since the label has been changed
    GetSizer()->Layout();
}

void RenewableCluster::areaHasChanged(Antares::Data::Area* area)
{
    if (pArea != area)
    {
        pArea = area;
        pLastSelectedRenewableCluster = nullptr;
        if (pRnListbox)
            pRnListbox->forceUpdate();
        update();
        if (pRnListbox)
            pRnListbox->Refresh();
    }
}

/*
void RenewableCluster::onStudyEndUpdate()
{
    if (pRnListbox)
        pRnListbox->forceUpdate();
}

void RenewableCluster::renameAggregate(Antares::Data::ThermalCluster* cluster,
                                     const wxString& newName,
                                     const bool broadcast)
{
    WIP::Locker wip;
    if (cluster && pArea && Data::Study::Current::Valid())
    {
        Antares::Data::ThermalClusterName newPlantName;
        wxStringToString(newName, newPlantName);

        Data::Study::Current::Get()->thermalClusterRename(cluster, newPlantName);
        MarkTheStudyAsModified();
    }
    if (broadcast)
        onClusterChanged(cluster);
    OnStudyThermalClusterRenamed(cluster);
    Window::Inspector::Refresh();
}

void RenewableCluster::onStudyThermalClusterRenamed(Antares::Data::ThermalCluster* cluster)
{
    if (cluster->parentArea == pArea)
        updateInnerValues();
}

void RenewableCluster::evtPopupDelete(wxCommandEvent&)
{
    Yuni::Bind<void()> callback;
    callback.bind(this, &ThermalCluster::internalDeletePlant, (void*)nullptr);
    Dispatcher::GUI::Post(callback);
}

void RenewableCluster::evtPopupDeleteAll(wxCommandEvent&)
{
    Yuni::Bind<void()> callback;
    callback.bind(this, &ThermalCluster::internalDeleteAll, (void*)nullptr);
    Dispatcher::GUI::Post(callback);
}
*/

void RenewableCluster::internalDeletePlant(void*)
{
    /*
    // Nothing is/was selected. Aborting.
    if (!pArea || !pLastSelectedRenewableCluster || not Data::Study::Current::Valid())
        return;

    // The thermal cluster to delete
    auto* toDelete = pLastSelectedRenewableCluster->thermalAggregate();
    if (not toDelete)
        return;

    auto& mainFrm = *Forms::ApplWnd::Instance();

    wxString messageText = wxT("");
    uint selectedConstraintsCount = Antares::Window::Inspector::SelectionBindingConstraintCount();
    if (selectedConstraintsCount > 0)
    {
        messageText << "\nSelected: ";
        messageText << selectedConstraintsCount;
        messageText << " Constraint";
        if (selectedConstraintsCount > 1)
            messageText << "s";
    }

    // If the pointer has been, it is guaranteed to be valid
    Window::Message message(&mainFrm,
                            wxT("Thermal cluster"),
                            wxT("Delete a thermal cluster"),
                            wxString()
                              << wxT("Do you really want to delete the thermal cluster '")
                              << wxStringFromUTF8(toDelete->name()) << wxT("' ?") << messageText);
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() == Window::Message::btnYes)
    {
        WIP::Locker wip;
        // Prevent any unwanted refresh
        OnStudyBeginUpdate();

        // Because we may need to update this afterwards
        auto study = Data::Study::Current::Get();
        study->scenarioRulesLoadIfNotAvailable();

        // Update the list
        Window::Inspector::RemoveThermalCluster(toDelete);
        pLastSelectedRenewableCluster = nullptr;
        onClusterChanged(nullptr);

        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(
          *study); // this will create a temp file, and save it during destructor call

        if (pArea->thermal.list.remove(toDelete->id()))
        {
            // We __must__ update the scenario builder data
            // We may delete an area and re-create a new one with the same
            // name (or rename) for example, but the data related to the old
            // area must be gone.

            update();
            Refresh();
            MarkTheStudyAsModified();
            updateInnerValues();
            pArea->thermal.list.rebuildIndex();
            pArea->thermal.prepareAreaWideIndexes();
            study->uiinfo->reload();

            // delete associated constraints
            Antares::Data::BindConstList::iterator BC = study->bindingConstraints.begin();
            int BCListSize = study->bindingConstraints.size();

            if (BCListSize)
            {
                logs.info() << "deleting the constraints ";

                WIP::Locker wip;
                for (int i = 0; i < BCListSize; i++)
                {
                    if (Window::Inspector::isConstraintSelected((*BC)->name()))
                        study->bindingConstraints.remove(*BC);
                    else
                        ++BC;
                }

                study->uiinfo->reloadBindingConstraints();
                OnStudyConstraintDelete(nullptr);
            }
        }
        else
            logs.error() << "Impossible to delete the cluster '" << toDelete->name() << "'";

        // The components are now allow to refresh themselves
        OnStudyEndUpdate();
    }
    */
}

/*
void RenewableCluster::internalDeleteAll(void*)
{
    // Nothing is/was selected. Aborting.
    if (!pArea)
        return;

    if (pArea->thermal.list.empty())
    {
        // The selected has been obviously invalidated
        pLastSelectedRenewableCluster = nullptr;
        return;
    }

    Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

    auto study = Data::Study::Current::Get();

    // If the pointer has been, it is guaranteed to be valid
    Window::Message message(
      &mainFrm,
      wxT("Thermal cluster"),
      wxT("Delete all thermal clusters"),
      wxString() << wxT("Do you really want to delete all thermal clusters from the area '")
                 << wxStringFromUTF8(pArea->name) << wxT("' ?"));
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() == Window::Message::btnYes)
    {
        WIP::Locker wip;
        // Prevent any unwanted refresh
        OnStudyBeginUpdate();

        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(*Data::Study::Current::Get());

        // invalidating the parent area
        pArea->invalidate();

        // Update the list
        Window::Inspector::SelectArea(pArea);
        pLastSelectedRenewableCluster = nullptr;
        onClusterChanged(nullptr);

        // delete associated constraints

        int BCListSize = study->bindingConstraints.size();

        if (BCListSize)
        {
            logs.info() << "deleting the constraints ";

            WIP::Locker wip;
            study->bindingConstraints.remove(pArea);
        }

        pArea->thermal.reset();

        update();
        Refresh();
        MarkTheStudyAsModified();
        updateInnerValues();

        study->uiinfo->reloadAll();

        // The components are now allow to refresh themselves
        OnStudyEndUpdate();
    }
}
*/

void RenewableCluster::internalAddPlant(void*)
{
    WIP::Locker wip;
    auto study = Data::Study::Current::Get();

    if (!(!study) && pArea)
    {
        onClusterChanged(nullptr);

        uint indx = 1;

        // Trying to find an uniq name
        Antares::Data::RenewableClusterName sFl;
        sFl.clear() << "new cluster";
        while (pArea->renewable.list.find(sFl))
        {
            ++indx;
            sFl.clear() << "new cluster " << indx;
        }

        /*
        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(*study);
        */

        // Creating a new cluster
        Antares::Data::RenewableCluster* cluster = new Antares::Data::RenewableCluster(pArea);
        logs.info() << "adding new renewable cluster " << pArea->id << '.' << sFl;
        cluster->name(sFl);
        cluster->reset();
        pArea->renewable.list.add(cluster);
        pArea->renewable.list.mapping[cluster->id()] = cluster;
        pArea->renewable.list.rebuildIndex();
        pArea->renewable.prepareAreaWideIndexes();

        // Update the list
        update();
        Refresh();
        onClusterChanged(cluster);
        MarkTheStudyAsModified();
        updateInnerValues();

        pArea->invalidate();

        study->uiinfo->reload();
    }
}

void RenewableCluster::internalClonePlant(void*)
{
    /*
    // Nothing is/was selected. Aborting.
    if (!pArea || !pLastSelectedRenewableCluster)
        return;

    if (!pArea->thermal.list.find(pLastSelectedRenewableCluster->thermalAggregate()->id()))
    {
        // The selected has been obviously invalidated
        pLastSelectedRenewableCluster = nullptr;
        // Inform the user
        logs.error() << "Please select a thermal cluster.";
        return;
    }

    WIP::Locker wip;
    const Antares::Data::ThermalCluster& selectedPlant
      = *pLastSelectedRenewableCluster->thermalAggregate();

    auto study = Data::Study::Current::Get();
    if (!(!study) && pArea)
    {
        onClusterChanged(nullptr);

        uint indx = 2;

        // Trying to find an uniq name
        Antares::Data::ThermalClusterName copy = selectedPlant.name();

        Data::ThermalClusterName::Size sepPos = copy.find_last_of(' ');
        if (sepPos != YString::npos)
        {
            Data::ThermalClusterName suffixChain(copy, sepPos + 1);
            int suffixNumber = suffixChain.to<int>();
            if (suffixNumber > 0)
            {
                Data::ThermalClusterName suffixLess(copy, 0, sepPos);
                copy = suffixLess;
            }
        }

        copy += ' ';

        Antares::Data::ThermalClusterName sFl;
        sFl << copy << indx; // lowercase
        while (pArea->thermal.list.find(sFl))
        {
            ++indx;
            sFl.clear() << copy << indx;
        }

        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(*study);

        // Creating a new cluster
        auto* cluster = new Antares::Data::ThermalCluster(pArea);
        cluster->name(sFl);
        cluster->reset();
        // Reset to default values
        cluster->copyFrom(selectedPlant);

        pArea->thermal.list.add(cluster);
        pArea->thermal.list.mapping[cluster->id()] = cluster;
        pArea->thermal.list.rebuildIndex();
        pArea->thermal.prepareAreaWideIndexes();

        // Update the list
        update();
        Refresh();

        onClusterChanged(cluster);
        MarkTheStudyAsModified();
        updateInnerValues();

        pArea->invalidate();

        study->uiinfo->reload();
    }
    */
}

/*
void RenewableCluster::onApplicationOnQuit()
{
    // Avoid SegV at exit
    pArea = nullptr;
    onClusterChanged(nullptr);
    pRnListbox->clear();
}

void RenewableCluster::onStudyClosed()
{
    // Avoid SegV at exit
    pArea = nullptr;
    onClusterChanged(nullptr);
    pRnListbox->clear();
}
*/

void RenewableCluster::onRnSelected(Component::HTMLListbox::Item::IItem::Ptr item)
{
    Yuni::Bind<void()> callback;
    callback.bind(this, &RenewableCluster::delayedSelection, item);
    Dispatcher::GUI::Post(callback, 30);
}


void RenewableCluster::delayedSelection(Component::HTMLListbox::Item::IItem::Ptr item)
{
    typedef Component::HTMLListbox::Item::RenewableCluster::Ptr RnPtr;
    RnPtr a = Component::HTMLListbox::Item::IItem::Ptr::DynamicCast<RnPtr>(item);
    if (a)
    {
        // Lock the window to prevent flickering
        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

        pLastSelectedRenewableCluster = a;
        auto* cluster = a->thermalAggregate();

        WIP::Locker wip;
        wxWindowUpdateLocker updater(&mainFrm);
        onClusterChanged(cluster);
        // Window::Inspector::SelectThermalCluster(cluster);
        updateInnerValues();

        // Selecting Binding constraints containing the cluster

        /*
        Data::BindingConstraint::Set constraintlist;

        auto study = Data::Study::Current::Get();

        const Data::BindConstList::iterator cEnd = study->bindingConstraints.end();
        for (Data::BindConstList::iterator i = study->bindingConstraints.begin(); i != cEnd; ++i)
        {
            // alias to the current constraint
            Data::BindingConstraint* constraint = *i;

            if (constraint->contains(cluster))
                constraintlist.insert(constraint);
        }
        Window::Inspector::AddBindingConstraints(constraintlist);
        */
    }
}

void RenewableCluster::onDeleteDropdown(Antares::Component::Button&, wxMenu& menu, void*)
{
    /*
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Delete the selected cluster"), "images/16x16/thermal_remove.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(ThermalCluster::evtPopupDelete),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Delete all"), "images/16x16/empty.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(ThermalCluster::evtPopupDeleteAll),
                 nullptr,
                 this);
    */
}

/*
void RenewableCluster::onStudyThermalClusterGroupChanged(Antares::Data::Area* area)
{
    if (area && area == pArea)
    {
        update();
        MarkTheStudyAsModified();
        Refresh();
    }
}

void RenewableCluster::onStudyThermalClusterCommonSettingsChanged()
{
    updateInnerValues();
    Refresh();
}
*/

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares
