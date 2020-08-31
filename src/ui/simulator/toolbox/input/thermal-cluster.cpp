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
#include "thermal-cluster.h"
#include "../components/captionpanel.h"
#include "../../application/study.h"
#include "../../application/main.h"
#include "../../application/wait.h"
#include "../../windows/inspector.h"
#include <assert.h>
#include "../resources.h"
#include "../create.h"
#include "../validator.h"
#include "../components/htmllistbox/datasource/thermal-cluster.h"
#include "../components/htmllistbox/item/thermal-cluster.h"
#include "../components/button.h"
#include "../../windows/message.h"
#include "../../application/menus.h"
#include <antares/study/scenario-builder/updater.hxx>
#include <wx/wupdlock.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <ui/common/lock.h>

using namespace Yuni;




namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{


	ThermalCluster::ThermalCluster(wxWindow* parent, InputSelector::Area* area) :
		AInput(parent),
		pArea(nullptr),
		pTotalMW(nullptr),
		pImageList(16, 16),
		pAreaNotifier(area)
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
		Forms::ApplWnd::Instance()->onApplicationQuit.connect(this, &ThermalCluster::onApplicationOnQuit);
		OnStudyClosed.connect(this, &ThermalCluster::onStudyClosed);
		OnStudyEndUpdate.connect(this, &ThermalCluster::onStudyEndUpdate);

		if (area)
			area->onAreaChanged.connect(this, &ThermalCluster::areaHasChanged);

		OnStudyThermalClusterRenamed.connect(this, &ThermalCluster::onStudyThermalClusterRenamed);
		OnStudyThermalClusterGroupChanged.connect(this, &ThermalCluster::onStudyThermalClusterGroupChanged);
		OnStudyThermalClusterCommonSettingsChanged.connect(this, &ThermalCluster::onStudyThermalClusterCommonSettingsChanged);
	}


	ThermalCluster::~ThermalCluster()
	{
		destroyBoundEvents();
	}


	namespace HTMLLsDatasourcesTh = Component::HTMLListbox::Datasource::ThermalClusters;


	void ThermalCluster::internalBuildSubControls()
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		SetSizer(sizer);

		Antares::Component::Button* btn;

		//
		wxBoxSizer* toolSZ = new wxBoxSizer(wxHORIZONTAL);
		toolSZ->AddSpacer(10);

		// Create a new thermal cluster
		btn = new Antares::Component::Button(this, wxT("Create a cluster"), "images/16x16/thermal_add.png",
			this, &ThermalCluster::internalAddPlant);
		toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL|wxALL);

		// Delete
		btn = new Antares::Component::Button(this, wxT("Delete"), "images/16x16/thermal_remove.png",
			this, &ThermalCluster::internalDeletePlant);
		btn->dropDown(true);
		btn->onPopupMenu(this, &ThermalCluster::onDeleteDropdown);
		toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL|wxALL);

		// vertical line
		Antares::Component::AddVerticalSeparator(this, toolSZ);

		// Clone
		btn = new Antares::Component::Button(this, wxT("Clone"), "images/16x16/paste.png",
			this, &ThermalCluster::internalClonePlant);
		toolSZ->Add(btn, 0, wxALIGN_CENTER_VERTICAL|wxALL);


		toolSZ->AddStretchSpacer();

		pTotalMW = Component::CreateLabel(this, wxT("Total : 0 MW"));
		toolSZ->Add(pTotalMW, 0, wxRIGHT /*| wxALIGN_RIGHT*/ | wxALIGN_CENTER_VERTICAL);
		toolSZ->AddSpacer(15);
		toolSZ->Layout();

		sizer->AddSpacer(2);
		sizer->Add(toolSZ, 0, wxALL|wxEXPAND);
		sizer->AddSpacer(2);

		// The listbox
		pThListbox = new Component::HTMLListbox::Component(this);
		HTMLLsDatasourcesTh::ByAlphaOrder* dsAZ;
		dsAZ = pThListbox->addDatasource<HTMLLsDatasourcesTh::ByAlphaOrder>();
		HTMLLsDatasourcesTh::ByAlphaReverseOrder* dsZA;
		dsZA = pThListbox->addDatasource<HTMLLsDatasourcesTh::ByAlphaReverseOrder>();
		if (pAreaNotifier)
		{
			pAreaNotifier->onAreaChanged.connect(dsAZ, &HTMLLsDatasourcesTh::ByAlphaOrder::onAreaChanged);
			pAreaNotifier->onAreaChanged.connect(dsZA, &HTMLLsDatasourcesTh::ByAlphaReverseOrder::onAreaChanged);
		}
		sizer->Add(pThListbox, 1, wxALL|wxEXPAND);
		sizer->SetItemMinSize(pThListbox, 100, 200);
		pThListbox->onItemSelected.connect(this, &ThermalCluster::onThSelected);

		// Update the layout
		GetSizer()->Layout();
	}


	void ThermalCluster::update()
	{
		pThListbox->invalidate();
		onThermalClusterChanged(nullptr);
		updateInnerValues();
	}



	void ThermalCluster::updateInnerValues()
	{
		if (pThListbox)
			pThListbox->updateHtmlContent();

		if (pArea)
		{
			double total = 0.;
			uint unitCount = 0;
			pArea->thermal.list.retrieveTotalCapacityAndUnitCount(total, unitCount);

			// The total - installed capacity
			pTotalMW->SetLabel(wxString() << unitCount << wxT(" units, ") << total << wxT(" MW"));
		}
		else
			pTotalMW->SetLabel(wxEmptyString);

		// The layout must be updated since the label has been changed
		GetSizer()->Layout();
	}




	void ThermalCluster::areaHasChanged(Antares::Data::Area* area)
	{
		if (pArea != area)
		{
			pArea = area;
			pLastSelectedThermalCluster = nullptr;
			if (pThListbox)
				pThListbox->forceUpdate();
			update();
			if (pThListbox)
				pThListbox->Refresh();
		}
	}


	void ThermalCluster::onStudyEndUpdate()
	{
		if (pThListbox)
			pThListbox->forceUpdate();
	}


	void ThermalCluster::renameAggregate(Antares::Data::ThermalCluster* cluster, const wxString& newName, const bool broadcast)
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
			onThermalClusterChanged(cluster);
		OnStudyThermalClusterRenamed(cluster);
		Window::Inspector::Refresh();
	}


	void ThermalCluster::onStudyThermalClusterRenamed(Antares::Data::ThermalCluster* cluster)
	{
		if (cluster->parentArea == pArea)
			updateInnerValues();
	}


	void ThermalCluster::evtPopupDelete(wxCommandEvent&)
	{
		Yuni::Bind<void ()> callback;
		callback.bind(this, &ThermalCluster::internalDeletePlant, (void*)nullptr);
		Dispatcher::GUI::Post(callback);
	}


	void ThermalCluster::evtPopupDeleteAll(wxCommandEvent&)
	{
		Yuni::Bind<void ()> callback;
		callback.bind(this, &ThermalCluster::internalDeleteAll, (void*)nullptr);
		Dispatcher::GUI::Post(callback);
	}


	void ThermalCluster::internalDeletePlant(void*)
	{
		// Nothing is/was selected. Aborting.
		if (!pArea || !pLastSelectedThermalCluster || not Data::Study::Current::Valid())
			return;

		// The thermal cluster to delete
		auto* toDelete = pLastSelectedThermalCluster->thermalAggregate();
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
		Window::Message message(&mainFrm, wxT("Thermal cluster"),
			wxT("Delete a thermal cluster"),
			wxString() << wxT("Do you really want to delete the thermal cluster '")
			<< wxStringFromUTF8(toDelete->name())
			<< wxT("' ?")
			<< messageText);
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
			pLastSelectedThermalCluster = nullptr;
			onThermalClusterChanged(nullptr);

			// We have to rebuild the scenario builder data, if required
			ScenarioBuilderUpdater updaterSB(*study); // this will create a temp file, and save it during destructor call

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


				//delete associated constraints
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
	}


	void ThermalCluster::internalDeleteAll(void*)
	{
		// Nothing is/was selected. Aborting.
		if (!pArea)
			return;

		if (pArea->thermal.list.empty())
		{
			// The selected has been obviously invalidated
			pLastSelectedThermalCluster = nullptr;
			return;
		}

		Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

		auto study = Data::Study::Current::Get();

		// If the pointer has been, it is guaranteed to be valid
		Window::Message message(&mainFrm, wxT("Thermal cluster"),
			wxT("Delete all thermal clusters"),
			wxString() << wxT("Do you really want to delete all thermal clusters from the area '")
			<< wxStringFromUTF8(pArea->name)
			<< wxT("' ?"));
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
			pLastSelectedThermalCluster = nullptr;
			onThermalClusterChanged(nullptr);




			//delete associated constraints

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




	void ThermalCluster::internalAddPlant(void*)
	{
		WIP::Locker wip;
		auto study = Data::Study::Current::Get();

		if (!(!study) && pArea)
		{
			onThermalClusterChanged(nullptr);

			uint indx = 1;

			// Trying to find an uniq name
			Antares::Data::ThermalClusterName sFl;
			sFl.clear() << "new cluster";
			while (pArea->thermal.list.find(sFl))
			{
				++indx;
				sFl.clear() << "new cluster " << indx;
			}

			// We have to rebuild the scenario builder data, if required
			ScenarioBuilderUpdater updaterSB(*study);

			// Creating a new cluster
			Antares::Data::ThermalCluster* cluster = new Antares::Data::ThermalCluster(pArea);
			logs.info() << "adding new thermal cluster " << pArea->id << '.' << sFl;
			cluster->name(sFl);
			cluster->reset();
			pArea->thermal.list.add(cluster);
			pArea->thermal.list.mapping[cluster->id()] = cluster;
			pArea->thermal.list.rebuildIndex();
			pArea->thermal.prepareAreaWideIndexes();

			// Update the list
			update();
			Refresh();

			onThermalClusterChanged(cluster);
			MarkTheStudyAsModified();
			updateInnerValues();

			pArea->invalidate();

			study->uiinfo->reload();
		}
	}


	void ThermalCluster::internalClonePlant(void*)
	{
		// Nothing is/was selected. Aborting.
		if (!pArea || !pLastSelectedThermalCluster)
			return;

		if (!pArea->thermal.list.find(pLastSelectedThermalCluster->thermalAggregate()->id()))
		{
			// The selected has been obviously invalidated
			pLastSelectedThermalCluster = nullptr;
			// Inform the user
			logs.error() << "Please select a thermal cluster.";
			return;
		}

		WIP::Locker wip;
		const Antares::Data::ThermalCluster& selectedPlant = *pLastSelectedThermalCluster->thermalAggregate();

		auto study = Data::Study::Current::Get();
		if (!(!study) && pArea)
		{
			onThermalClusterChanged(nullptr);

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

			onThermalClusterChanged(cluster);
			MarkTheStudyAsModified();
			updateInnerValues();

			pArea->invalidate();

			study->uiinfo->reload();
		}
	}



	void ThermalCluster::onApplicationOnQuit()
	{
		// Avoid SegV at exit
		pArea = nullptr;
		onThermalClusterChanged(nullptr);
		pThListbox->clear();
	}


	void ThermalCluster::onStudyClosed()
	{
		// Avoid SegV at exit
		pArea = nullptr;
		onThermalClusterChanged(nullptr);
		pThListbox->clear();
	}



	void ThermalCluster::onThSelected(Component::HTMLListbox::Item::IItem::Ptr item)
	{
		Yuni::Bind<void ()> callback;
		callback.bind(this, &ThermalCluster::delayedSelection, item);
		Dispatcher::GUI::Post(callback, 30);
	}


	void ThermalCluster::delayedSelection(Component::HTMLListbox::Item::IItem::Ptr item)
	{
		typedef Component::HTMLListbox::Item::ThermalCluster::Ptr ThPtr;
		ThPtr a = Component::HTMLListbox::Item::IItem::Ptr::DynamicCast<ThPtr>(item);
		if (a)
		{
			// Lock the window to prevent flickering
			Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

			pLastSelectedThermalCluster = a;
			auto* cluster = a->thermalAggregate();

			WIP::Locker wip;
			wxWindowUpdateLocker updater(&mainFrm);
			onThermalClusterChanged(cluster);
			Window::Inspector::SelectThermalCluster(cluster);
			updateInnerValues();


			//Selecting Binding constraints containing the cluster

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
		}


	}


	void ThermalCluster::onDeleteDropdown(Antares::Component::Button&, wxMenu& menu, void*)
	{
		wxMenuItem* it;

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Delete the selected cluster"),
			"images/16x16/thermal_remove.png");
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(ThermalCluster::evtPopupDelete), nullptr, this);

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Delete all"),
			"images/16x16/empty.png");
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(ThermalCluster::evtPopupDeleteAll), nullptr, this);
	}


	void ThermalCluster::onStudyThermalClusterGroupChanged(Antares::Data::Area* area)
	{
		if (area && area == pArea)
		{
			update();
			MarkTheStudyAsModified();
			Refresh();
		}
	}


	void ThermalCluster::onStudyThermalClusterCommonSettingsChanged()
	{
		updateInnerValues();
		Refresh();
	}






} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

