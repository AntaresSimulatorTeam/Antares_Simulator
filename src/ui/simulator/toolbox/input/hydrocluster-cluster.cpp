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

#include "../../application/study.h"
#include "../../application/main.h"
#include "../../application/wait.h"
#include "../../windows/inspector.h"
#include "../resources.h"
#include "../create.h"
#include "../components/button.h"
#include "../../windows/message.h"
#include "../../application/menus.h"
#include <antares/study/scenario-builder/updater.hxx>
#include <wx/wupdlock.h>
#include <wx/sizer.h>
#include "hydrocluster-cluster.h"

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
HydroclusterCluster::HydroclusterCluster(wxWindow* parent, InputSelector::Area* area) :
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

    // Connect to the global event
    // Avoid SegV
    Forms::ApplWnd::Instance()->onApplicationQuit.connect(this,
                                                          &HydroclusterCluster::onApplicationOnQuit);
    OnStudyClosed.connect(this, &HydroclusterCluster::onStudyClosed);
    OnStudyEndUpdate.connect(this, &HydroclusterCluster::onStudyEndUpdate);

    if (area)
        area->onAreaChanged.connect(this, &HydroclusterCluster::areaHasChanged);

    OnStudyHydroclusterClusterRenamed.connect(this, &HydroclusterCluster::onStudyHydroclusterClusterRenamed);
    OnStudyHydroclusterClusterGroupChanged.connect(
      this, &HydroclusterCluster::onStudyHydroclusterClusterGroupChanged);
    OnStudyHydroclusterClusterCommonSettingsChanged.connect(
      this, &HydroclusterCluster::onStudyHydroclusterClusterCommonSettingsChanged);
}

HydroclusterCluster::~HydroclusterCluster()
{
    destroyBoundEvents();
}

void HydroclusterCluster::internalBuildSubControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    Antares::Component::Button* btn;

    wxBoxSizer* toolSZ = new wxBoxSizer(wxHORIZONTAL);
    toolSZ->AddSpacer(10);

    // Create a new hydrocluster cluster
    btn = new Antares::Component::Button(this,
                                         wxT("Create a cluster"),
                                         "images/16x16/hydrocluster_add.png",
                                         this,
                                         &HydroclusterCluster::internalAddPlant);

    toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

    // Delete
    btn = new Antares::Component::Button(this,
                                         wxT("Delete"),
                                         "images/16x16/hydrocluster_remove.png",
                                         this,
                                         &HydroclusterCluster::internalDeletePlant);
    btn->dropDown(true);
    btn->onPopupMenu(this, &HydroclusterCluster::onDeleteDropdown);
    toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

    // vertical line
    Antares::Component::AddVerticalSeparator(this, toolSZ);

    // Clone
    btn = new Antares::Component::Button(
      this, wxT("Clone"), "images/16x16/paste.png", this, &HydroclusterCluster::internalClonePlant);
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

    pDataSourceAZ = pRnListbox->addDatasource<HydroclusterClustersByAlphaOrder>();
    pDataSourceZA = pRnListbox->addDatasource<HydroclusterClustersByAlphaReverseOrder>();

    if (pAreaNotifier)
    {
        pAreaNotifier->onAreaChanged.connect(pDataSourceAZ,
                                             &HydroclusterClustersByAlphaOrder::onAreaChanged);
        pAreaNotifier->onAreaChanged.connect(pDataSourceZA,
                                             &HydroclusterClustersByAlphaReverseOrder::onAreaChanged);
    }

    sizer->Add(pRnListbox, 1, wxALL | wxEXPAND);
    sizer->SetItemMinSize(pRnListbox, 100, 200);
    pRnListbox->onItemSelected.connect(this, &HydroclusterCluster::onRnSelected);

    // Update the layout
    GetSizer()->Layout();
}

void HydroclusterCluster::update()
{
    pRnListbox->invalidate();
    onClusterChanged(nullptr);
    updateInnerValues();
}

void HydroclusterCluster::updateWhenGroupChanges()
{
    // Warn the selected data source (A-Z or Z-A sorting) that a cluster's group changed
    ClustersByOrder* dataSource = dynamic_cast<ClustersByOrder*>(pRnListbox->datasource());
    if (dataSource)
        dataSource->hasGroupChanged(true);

    pRnListbox->forceRedraw();
    onClusterChanged(nullptr);
    updateInnerValues();
}

void HydroclusterCluster::updateInnerValues()
{
    if (pRnListbox)
        pRnListbox->updateHtmlContent();

    if (pArea)
    {
        double total = 0.;
        uint unitCount = 0;
        pArea->hydrocluster.list.retrieveTotalCapacityAndUnitCount(total, unitCount);

        // The total - installed capacity
        pTotalMW->SetLabel(wxString() << unitCount << wxT(" units, ") << total << wxT(" MW"));
    }
    else
        pTotalMW->SetLabel(wxEmptyString);

    // The layout must be updated since the label has been changed
    GetSizer()->Layout();
}

void HydroclusterCluster::areaHasChanged(Antares::Data::Area* area)
{
    if (pArea != area)
    {
        pArea = area;
        pLastSelectedHydroclusterCluster = nullptr;
        if (pRnListbox)
            pRnListbox->forceUpdate();
        update();
        if (pRnListbox)
            pRnListbox->Refresh();
    }
}

void HydroclusterCluster::onStudyEndUpdate()
{
    if (pRnListbox)
        pRnListbox->forceUpdate();
}

void HydroclusterCluster::onStudyHydroclusterClusterRenamed(Antares::Data::HydroclusterCluster* cluster)
{
    if (cluster->parentArea == pArea)
        updateInnerValues();
}

void HydroclusterCluster::evtPopupDelete(wxCommandEvent&)
{
    Yuni::Bind<void()> callback;
    callback.bind(this, &HydroclusterCluster::internalDeletePlant, (void*)nullptr);
    Dispatcher::GUI::Post(callback);
}

void HydroclusterCluster::evtPopupDeleteAll(wxCommandEvent&)
{
    Yuni::Bind<void()> callback;
    callback.bind(this, &HydroclusterCluster::internalDeleteAll, (void*)nullptr);
    Dispatcher::GUI::Post(callback);
}

void HydroclusterCluster::internalDeletePlant(void*)
{
    // Nothing is/was selected. Aborting.
    if (!pArea || !pLastSelectedHydroclusterCluster || not Data::Study::Current::Valid())
        return;

    // The hydrocluster cluster to delete
    auto* toDelete = pLastSelectedHydroclusterCluster->hydroclusterAggregate();
    if (not toDelete)
        return;

    auto& mainFrm = *Forms::ApplWnd::Instance();

    wxString messageText = wxT("");

    // If the pointer has been, it is guaranteed to be valid
    Window::Message message(&mainFrm,
                            wxT("Hydrocluster cluster"),
                            wxT("Delete a hydrocluster cluster"),
                            wxString()
                              << wxT("Do you really want to delete the hydrocluster cluster '")
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
        Window::Inspector::RemoveHydroclusterCluster(toDelete);
        pLastSelectedHydroclusterCluster = nullptr;
        onClusterChanged(nullptr);

        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(
          *study); // this will create a temp file, and save it during destructor call

        if (pArea->hydrocluster.list.remove(toDelete->id()))
        {
            // We __must__ update the scenario builder data
            // We may delete an area and re-create a new one with the same
            // name (or rename) for example, but the data related to the old
            // area must be gone.

            update();
            Refresh();
            MarkTheStudyAsModified();
            updateInnerValues();
            pArea->hydrocluster.list.rebuildIndex();
            pArea->hydrocluster.prepareAreaWideIndexes();
            study->uiinfo->reload();
        }
        else
            logs.error() << "Impossible to delete the cluster '" << toDelete->name() << "'";

        // The components are now allow to refresh themselves
        OnStudyEndUpdate();
    }
}

void HydroclusterCluster::internalDeleteAll(void*)
{
    // Nothing is/was selected. Aborting.
    if (!pArea)
        return;

    if (pArea->hydrocluster.list.empty())
    {
        // The selected has been obviously invalidated
        pLastSelectedHydroclusterCluster = nullptr;
        return;
    }

    Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

    auto study = Data::Study::Current::Get();

    // If the pointer has been, it is guaranteed to be valid
    Window::Message message(
      &mainFrm,
      wxT("Hydrocluster cluster"),
      wxT("Delete all hydrocluster clusters"),
      wxString() << wxT("Do you really want to delete all hydrocluster clusters from the area '")
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
        pLastSelectedHydroclusterCluster = nullptr;
        onClusterChanged(nullptr);

        pArea->hydrocluster.reset();

        update();
        Refresh();
        MarkTheStudyAsModified();
        updateInnerValues();

        study->uiinfo->reloadAll();

        // The components are now allow to refresh themselves
        OnStudyEndUpdate();
    }
}

void HydroclusterCluster::internalAddPlant(void*)
{
    WIP::Locker wip;
    auto study = Data::Study::Current::Get();

    if (!(!study) && pArea)
    {
        onClusterChanged(nullptr);

        uint indx = 1;

        // Trying to find an uniq name
        Antares::Data::ClusterName sFl;
        sFl.clear() << "new cluster";
        while (pArea->hydrocluster.list.find(sFl))
        {
            ++indx;
            sFl.clear() << "new cluster " << indx;
        }

        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(*study);

        // Creating a new cluster
        auto cluster = std::make_shared<Antares::Data::HydroclusterCluster>(pArea);
        logs.info() << "adding new hydrocluster cluster " << pArea->id << '.' << sFl;
        cluster->setName(sFl);
        cluster->reset();
        pArea->hydrocluster.list.add(cluster);
        pArea->hydrocluster.list.rebuildIndex();
        pArea->hydrocluster.prepareAreaWideIndexes();

        // Update the list
        update();
        Refresh();
        onClusterChanged(cluster.get());
        MarkTheStudyAsModified();
        updateInnerValues();

        pArea->invalidate();

        study->uiinfo->reload();
    }
}

void HydroclusterCluster::internalClonePlant(void*)
{
    // Nothing is/was selected. Aborting.
    if (!pArea || !pLastSelectedHydroclusterCluster)
        return;

    if (!pArea->hydrocluster.list.find(pLastSelectedHydroclusterCluster->hydroclusterAggregate()->id()))
    {
        // The selected has been obviously invalidated
        pLastSelectedHydroclusterCluster = nullptr;
        // Inform the user
        logs.error() << "Please select a hydrocluster cluster.";
        return;
    }

    WIP::Locker wip;
    const Antares::Data::HydroclusterCluster& selectedPlant
      = *pLastSelectedHydroclusterCluster->hydroclusterAggregate();

    auto study = Data::Study::Current::Get();
    if (!(!study) && pArea)
    {
        onClusterChanged(nullptr);

        uint indx = 2;

        // Trying to find an uniq name
        Antares::Data::ClusterName copy = selectedPlant.name();

        Data::ClusterName::Size sepPos = copy.find_last_of(' ');
        if (sepPos != YString::npos)
        {
            Data::ClusterName suffixChain(copy, sepPos + 1);
            int suffixNumber = suffixChain.to<int>();
            if (suffixNumber > 0)
            {
                Data::ClusterName suffixLess(copy, 0, sepPos);
                copy = suffixLess;
            }
        }

        copy += ' ';

        Antares::Data::ClusterName sFl;
        sFl << copy << indx; // lowercase
        while (pArea->hydrocluster.list.find(sFl))
        {
            ++indx;
            sFl.clear() << copy << indx;
        }

        // We have to rebuild the scenario builder data, if required
        ScenarioBuilderUpdater updaterSB(*study);

        // Creating a new cluster
        auto cluster = std::make_shared<Antares::Data::HydroclusterCluster>(pArea);
        cluster->setName(sFl);
        cluster->reset();
        // Reset to default values
        cluster->copyFrom(selectedPlant);

        pArea->hydrocluster.list.add(cluster);
        pArea->hydrocluster.list.rebuildIndex();
        pArea->hydrocluster.prepareAreaWideIndexes();

        // Update the list
        update();
        Refresh();

        onClusterChanged(cluster.get());
        MarkTheStudyAsModified();
        updateInnerValues();

        pArea->invalidate();

        study->uiinfo->reload();
    }
}

void HydroclusterCluster::onApplicationOnQuit()
{
    // Avoid SegV at exit
    pArea = nullptr;
    onClusterChanged(nullptr);
    pRnListbox->clear();
}

void HydroclusterCluster::onStudyClosed()
{
    // Avoid SegV at exit
    pArea = nullptr;
    onClusterChanged(nullptr);
    pRnListbox->clear();
}

void HydroclusterCluster::onRnSelected(Component::HTMLListbox::Item::IItem::Ptr item)
{
    Yuni::Bind<void()> callback;
    callback.bind(this, &HydroclusterCluster::delayedSelection, item);
    Dispatcher::GUI::Post(callback, 30);
}

void HydroclusterCluster::delayedSelection(Component::HTMLListbox::Item::IItem::Ptr item)
{
    auto a = dynamic_pointer_cast<Component::HTMLListbox::Item::HydroclusterClusterItem>(item);
    if (a)
    {
        // Lock the window to prevent flickering
        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

        pLastSelectedHydroclusterCluster = a;
        auto* cluster = a->hydroclusterAggregate();

        WIP::Locker wip;
        wxWindowUpdateLocker updater(&mainFrm);
        onClusterChanged(cluster);
        Window::Inspector::SelectHydroclusterCluster(cluster);
        updateInnerValues();
    }
}

void HydroclusterCluster::onDeleteDropdown(Antares::Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Delete the selected cluster"), "images/16x16/thermal_remove.png");

    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(HydroclusterCluster::evtPopupDelete),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Delete all"), "images/16x16/empty.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(HydroclusterCluster::evtPopupDeleteAll),
                 nullptr,
                 this);
}

void HydroclusterCluster::onStudyHydroclusterClusterGroupChanged(Antares::Data::Area* area)
{
    if (area && area == pArea)
    {
        updateWhenGroupChanges();
        MarkTheStudyAsModified();
        Refresh();
    }
}

void HydroclusterCluster::onStudyHydroclusterClusterCommonSettingsChanged()
{
    updateInnerValues();
    Refresh();
}

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares
