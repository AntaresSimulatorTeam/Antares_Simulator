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

#include <antares/antares.h>
#include "output.h"
#include <yuni/io/directory.h>
#include "../../toolbox/components/captionpanel.h"
#include "../../toolbox/create.h"
#include "../../application/main.h"
#include "../../application/menus.h"
#include "../../toolbox/resources.h"
#include "../../windows/inspector.h"
#include "../../toolbox/resources.h"
#include "../../windows/message.h"
#include "../../windows/textinput/input.h"
#include <ui/common/dispatcher.h>
#include <ui/common/lock.h>
#include <ui/common/component/panel/group.h>
#include <ui/common/component/spotlight.h>
#include "window.h"
#include "job.h"
#include <set>
#include <wx/imaglist.h>
#include "spotlight-provider.h"
#include "mini-frame.h"

#include "provider/variables.h"
#include "provider/outputs.h"
#include "provider/output-comparison.h"

using namespace Yuni;

# define SEP  IO::Separator




namespace Antares
{
namespace Window
{
namespace OutputViewer
{


	static const char* const imageNumber[10] =
	{
		"images/16x16/0.png",
		"images/16x16/1.png",
		"images/16x16/2.png",
		"images/16x16/3.png",
		"images/16x16/4.png",
		"images/16x16/5.png",
		"images/16x16/6.png",
		"images/16x16/7.png",
		"images/16x16/8.png",
		"images/16x16/9.png",
	};

	static const char* const defaultVariable   = "LOAD";
	static const char* const defaultVariableID = "load";



	enum CallbackType
	{
		ctNone = 0,
		ctCSVFile,
		ctSummary,
		ctComments,
		ctLogs
	};


	typedef std::pair<Layer*, wxWindow*>  PairLayerWindow;






	Component::Component(wxWindow* parent, bool parentIsStandaloneWindow) :
		Antares::Component::Panel(parent),
		pCurrentSelectionType(stNone),
		pCurrentLOD(lodAllMCYears),
		pCurrentYear(1),
		pHasYearByYear(false),
		pHasConcatenedDataset(false),
		pCurrentVariable(defaultVariable),
		pCurrentVariableID(defaultVariableID),
		pCurrentClusters(nullptr),
		pSizerForBtnOutputs(nullptr),
		pWindowForBtnOutputs(nullptr),
		pSizerForPanelOutputs(nullptr),
		pWindowForPanelOutputs(nullptr),
		pCurrentLayerForMenu(nullptr),
		pLblAreaOrLinkName(nullptr),
		pSpotlight(nullptr),
		pControlsAlreadyCreated(false),
		pSplitter(nullptr),
		pSplitterWindow1(nullptr),
		pSplitterWindow2(nullptr),
		pSidebarCollapsed(false),
		pOldSidebarWidth(200),
		pParentWindow(nullptr),
		pLabelItemName(nullptr),
		pBtnValues(nullptr),
		pBtnDetails(nullptr),
		pBtnID(nullptr),
		pBtnHourly(nullptr),
		pBtnDaily(nullptr),
		pBtnWeekly(nullptr),
		pBtnMonthly(nullptr),
		pBtnAnnual(nullptr),
		pBtnCurrentYearNumber(nullptr),
		pBtnVariables(nullptr),
		pJobsRemaining(0),
		pOpenedInANewWindow(parentIsStandaloneWindow),
		pASelectionHasAlreadyBeenMade(false)
	{
		pYearsLimits[0] = 0;
		pYearsLimits[1] = 0;
		if (parentIsStandaloneWindow)
			pParentWindow = parent;

		// The output has been updated
		OnStudyUpdateOutputInfo.connect(this, &Component::updateFromExternalEvent);

		createAllControlsIfNeeded();
		OnStudyLoaded.connect(this, &Component::createAllControlsIfNeeded);
	}


	Component::~Component()
	{
		// Disconnect from the event
		OnStudyUpdateOutputInfo.remove(this);

		clear();
		// detach all events connected to this frame
		destroyBoundEvents();

		if (pCurrentClusters)
		{
			delete pCurrentClusters;
			pCurrentClusters = nullptr;
		}

		// To avoid corrupt vtable in some rare cases / misuses
		// (when children try to access to this object for example),
		// we should destroy all children as soon as possible.
		auto* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}


	void Component::displayMiniFrame(wxWindow* parent, Antares::Component::Spotlight::IProvider* provider,
		int width, bool searchInput, bool groups)
	{
		assert(provider);

		uint flags = 0;
		if (!searchInput)
			flags |= Antares::Component::Spotlight::optNoSearchInput;
		if (groups)
			flags |= Antares::Component::Spotlight::optGroups;

		Antares::Component::Spotlight::FrameShow(parent, provider, flags, (int) width);
	}



	void Component::emptyCache()
	{
		typedef Antares::Private::OutputViewerData::ContentMap  Map;

		pMutex.lock();
		auto end = pAlreadyPreparedContents.cend();
		for (auto i = pAlreadyPreparedContents.cbegin(); i != end; ++i)
			delete i->second;

		pAlreadyPreparedContents.clear();
		pMutex.unlock();
	}


	void Component::updateFromExternalEvent(const Data::Output::List& list, const Data::Output::Ptr selection)
	{
		update(list, selection);
	}


	void Component::createAllControlsIfNeeded()
	{
		// alias
		typedef Antares::Component::Notebook  Notebook;

		if (pControlsAlreadyCreated)
			return;

		auto* vz = new wxBoxSizer(wxVERTICAL);
		SetSizer(vz);

		// Space, for beauty
		vz->AddSpacer(2);

		{
			pSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER);

			auto* content = new Component::Panel(pSplitter);
			{
				wxBoxSizer* cvz = new wxBoxSizer(wxVERTICAL);
				wxSizer* hz;

				wxBoxSizer* hzori = new wxBoxSizer(wxHORIZONTAL);
				pSizerToolbar = hzori;
				Button* btn = nullptr;
				wxStaticBitmap* sbmp = nullptr;

				// Tabs
				{
					// sidebar collapse / expand
					{
						hzori->AddSpacer(2);
						btn = new Button(content, wxEmptyString, "images/16x16/sidebar_collapse.png",
							this, &Component::onToggleSidebar);
						btn->SetToolTip(wxT("Hide the sidebar"));
						hzori->Add(btn, 0, wxALL|wxEXPAND);
						hzori->AddSpacer(5);
						pBtnExpandCollapseSidebar = btn;
					}

					auto* grp = new Antares::Component::PanelGroup(content);
					hzori->Add(grp, 0, wxALL|wxEXPAND);
					hz = grp->subpanel->GetSizer();
					sbmp = Resources::StaticBitmapLoadFromFile(grp, wxID_ANY, "images/16x16/tab.png");
					grp->leftSizer->Add(sbmp, 0, wxALIGN_CENTER);

					// Plus - Add a new output
					{
						btn = new Button(grp->subpanel, wxT("New tab"), nullptr); //"images/16x16/plus.png");
						btn->onPopupMenu(this, &Component::onDropDownOutputs);
						btn->menu(true);
						btn->pushedColor(grp);
						hz->Add(btn, 0, wxALL|wxEXPAND);
					}
					// Plus - Add a new output
					{
						btn = new Button(grp->subpanel, wxEmptyString, "images/16x16/diff.png");
						btn->onPopupMenu(this, &Component::onDropDownComparison);
						btn->menu(true);
						btn->pushedColor(grp);
						btn->userdata(btn);
						hz->Add(btn, 0, wxALL|wxEXPAND);
					}

				}

				hzori->AddSpacer(5);

				// mc-all
				{
					auto* grp = new Antares::Component::PanelGroup(content);
					hzori->Add(grp, 0, wxALL|wxEXPAND);
					hz = grp->subpanel->GetSizer();
					sbmp = Resources::StaticBitmapLoadFromFile(grp, wxID_ANY, "images/16x16/calendar_current.png");
					grp->leftSizer->Add(sbmp, 0, wxALIGN_CENTER);

					btn = new Button(grp->subpanel, wxT("MC Synthesis"), nullptr);
					btn->menu(true);
					btn->onPopupMenu(this, &Component::allYearsOrYearByYearSelector);
					btn->pushedColor(grp);
					hz->Add(btn, 0, wxALL|wxEXPAND);
					pBtnAllYears = btn;

					pSizerYearsToolbar = hz;
					wxBoxSizer* ybySizer = new wxBoxSizer(wxHORIZONTAL);
					pSizerYearByYearToolbar = ybySizer;
					ybySizer->AddSpacer(4);
					btn = new Button(grp->subpanel, wxEmptyString, "images/16x16/calendar_previous.png",
						this, &Component::decrementYearByYear);
					btn->pushedColor(grp);
					btn->SetToolTip(wxT("Previous individual year"));
					ybySizer->Add(btn, 0, wxALL|wxEXPAND);
					btn = new Button(grp->subpanel, wxEmptyString, "images/16x16/calendar_next.png",
						this, &Component::incrementYearByYear);
					btn->pushedColor(grp);
					btn->SetToolTip(wxT("Next individual year"));
					ybySizer->Add(btn, 0, wxALL|wxEXPAND);

					btn = new Button(grp->subpanel, wxT("1 / 10"), nullptr, this, &Component::goToASpecificYear);
					btn->pushedColor(grp);
					// btn->menu(true);
					pBtnCurrentYearNumber = btn;
					ybySizer->Add(btn, 0, wxALL|wxEXPAND);

					hz->Add(ybySizer, 0, wxALL|wxEXPAND);

					pSizerVariables = new wxBoxSizer(wxHORIZONTAL);

					btn = new Button(grp->subpanel, wxT("CO2 EMIS (MWh)"), "images/16x16/variable.png",
						this, &Component::decrementYearByYear);
					btn->pushedColor(grp);
					btn->menu(true);
					btn->onPopupMenu(this, &Component::dropDownAllVariables);
					pBtnVariables = btn;
					pSizerVariables->Add(btn, 0, wxALL|wxEXPAND);

					hz->Add(pSizerVariables, 0, wxALL|wxEXPAND);

					// Should not be displayed by default
					pSizerYearsToolbar->Show(pSizerVariables, false);
					pSizerYearsToolbar->Show(pSizerYearByYearToolbar, false);
				}

				hzori->AddSpacer(5);

				// File Data (Values, details, id)
				{
					auto* grp = new Antares::Component::PanelGroup(content);
					hzori->Add(grp, 0, wxALL|wxEXPAND);
					hz = grp->subpanel->GetSizer();
					sbmp = Resources::StaticBitmapLoadFromFile(grp, wxID_ANY, "images/16x16/default.png");
					grp->leftSizer->Add(sbmp, 0, wxALIGN_CENTER);

					btn = new Button(grp->subpanel, wxT("General values"), nullptr, this, &Component::onSelectDataLevel);
					btn->pushed(true);
					btn->pushedColor(grp);
					btn->userdata(btn);
					pBtnValues = btn;
					hz->Add(btn, 0, wxALL|wxEXPAND);
					btn = new Button(grp->subpanel, wxT("Thermal plants"), nullptr, this, &Component::onSelectDataLevel);
					btn->pushedColor(grp);
					btn->userdata(btn);
					pBtnDetails = btn;
					hz->Add(btn, 0, wxALL|wxEXPAND);
					btn = new Button(grp->subpanel, wxT("Record years"), nullptr, this, &Component::onSelectDataLevel);
					btn->pushedColor(grp);
					btn->userdata(btn);
					hz->Add(btn, 0, wxALL|wxEXPAND);
					pBtnID = btn;
				}

				hzori->AddSpacer(5);

				// Calendar (hourly, daily, monthly...)
				{
					auto* grp = new Antares::Component::PanelGroup(content);
					hzori->Add(grp, 0, wxALL|wxEXPAND);
					sbmp = Resources::StaticBitmapLoadFromFile(grp, wxID_ANY, "images/16x16/logs.png");
					grp->leftSizer->Add(sbmp, 0, wxALIGN_CENTER);
					hz = grp->subpanel->GetSizer();

					btn = new Button(grp->subpanel, wxT("Hourly"),  nullptr, this, &Component::onSelectTimeLevel);
					btn->pushed(true);
					btn->pushedColor(grp);
					btn->userdata(btn);
					pBtnHourly = btn;
					hz->Add(btn, 0, wxALL|wxEXPAND);
					btn = new Button(grp->subpanel, wxT("Daily"),   nullptr, this, &Component::onSelectTimeLevel);
					btn->pushedColor(grp);
					btn->userdata(btn);
					hz->Add(btn, 0, wxALL|wxEXPAND);
					pBtnDaily = btn;
					btn = new Button(grp->subpanel, wxT("Weekly"),  nullptr, this, &Component::onSelectTimeLevel);
					btn->pushedColor(grp);
					btn->userdata(btn);
					hz->Add(btn, 0, wxALL|wxEXPAND);
					pBtnWeekly = btn;
					btn = new Button(grp->subpanel, wxT("Monthly"), nullptr, this, &Component::onSelectTimeLevel);
					btn->pushedColor(grp);
					btn->userdata(btn);
					pBtnMonthly = btn;
					hz->Add(btn, 0, wxALL|wxEXPAND);
					btn = new Button(grp->subpanel, wxT("Annual"), nullptr, this, &Component::onSelectTimeLevel);
					btn->pushedColor(grp);
					btn->userdata(btn);
					pBtnAnnual = btn;
					hz->Add(btn, 0, wxALL|wxEXPAND);
				}


				hzori->AddSpacer(10);
				cvz->Add(hzori, 0, wxALL|wxEXPAND);

				auto* pnlOutputs = new Component::Panel(content);
				// All available output
				// The controls for displying the outputs will be created later
				pWindowForBtnOutputs = pnlOutputs;
				pSizerForBtnOutputs  = new wxBoxSizer(wxHORIZONTAL);
				pnlOutputs->SetSizer(pSizerForBtnOutputs);
				cvz->AddSpacer(5);
				cvz->Add(pnlOutputs, 0, wxALL|wxEXPAND);

				pWindowForPanelOutputs = content;
				pSizerForPanelOutputs = new wxBoxSizer(wxHORIZONTAL);
				//cvz->AddSpacer(5);
				cvz->Add(pSizerForPanelOutputs, 1, wxALL|wxEXPAND);

				content->SetSizer(cvz);
			}

			// Listbox
			Antares::Component::Panel* listboxPanel = new Antares::Component::Panel(pSplitter);
			wxBoxSizer* lstboxpnl = new wxBoxSizer(wxVERTICAL);
			listboxPanel->SetSizer(lstboxpnl);

			// Current node / link
			{
				auto* grp = new Antares::Component::PanelGroup(listboxPanel);
				wxBoxSizer* hzori = new wxBoxSizer(wxHORIZONTAL);
				hzori->Add(grp, 1, wxALL|wxEXPAND);
				lstboxpnl->Add(hzori, 0, wxALL|wxEXPAND);
				lstboxpnl->AddSpacer(5);
				wxSizer* hz = grp->subpanel->GetSizer();

				// Back to input data !
				if (!pParentWindow)
				{
					auto* btn = new Button(grp->subpanel, wxT("Back to input"), "images/16x16/close.png",
						this, &Component::backToInputData);
					hz->Add(btn, 0, wxALL|wxEXPAND);

					// Separator
					Antares::Component::AddVerticalSeparator(grp->subpanel, hz, 2);
				}

				// New window
				{
					auto* btn = new Button(grp->subpanel, wxEmptyString, "images/16x16/new-window.png",
						this, &Component::openNewWindow);
					btn->SetToolTip(wxT("Open a new window"));
					hz->Add(btn, 0, wxALL|wxEXPAND);
					// Separator
					//Antares::Component::AddVerticalSeparator(grp->subpanel, hz, 2);
				}

				// final empty space
				hz->AddStretchSpacer();
			}

			enum
			{
				flags = Antares::Component::Spotlight::optGroups
			};
			pSpotlight = new Antares::Component::Spotlight(listboxPanel, flags);
			OnMapLayerChanged.connect(pSpotlight, &Antares::Component::Spotlight::onMapLayerChanged);
			OnMapLayerAdded.connect(pSpotlight, &Antares::Component::Spotlight::onMapLayerAdded);
			OnMapLayerRemoved.connect(pSpotlight, &Antares::Component::Spotlight::onMapLayerRemoved);
			OnMapLayerRenamed.connect(pSpotlight, &Antares::Component::Spotlight::onMapLayerRenamed);

			pSpotlight->onMapLayerAdded(new wxString("Districts"));
			pSpotlight->onMapLayerAdded(new wxString("Unknown"));
			lstboxpnl->Add(pSpotlight, 2, wxALL|wxEXPAND);
			//lstboxpnl->SetItemMinSize(p, 200, 100);

			pSplitterWindow1 = listboxPanel;
			pSplitterWindow2 = content;
			pSidebarCollapsed = false;

			pSplitter->SetMinimumPaneSize(200);
			pSplitter->SplitVertically(listboxPanel, content, 300);

			// The method SetSashPosition from the wxSplitterWindow is buggy.
			// We need to delay the call ourselves (GTK)
			Dispatcher::GUI::Post(this, &Component::bugFixResetSashPosition);

			// Empty space
			vz->Add(pSplitter, 1, wxALL|wxEXPAND);
			vz->Layout();
		}

		updateAreaOrLinkName();
		// The list will be destroyed when the study is closed.
		// Until the call of the current method, it is useless to perform
		// such operations.
		OnStudyClosed.remove(this);
		OnStudyClosed.connect(this, &Component::onStudyClosed);

		// Layout
		assert(GetSizer() != NULL);
		GetSizer()->Layout();

		pControlsAlreadyCreated = true;
	}

	void Component::updateLayerList()
	{
		auto study = Data::Study::Current::Get();
		auto layer = study->layers.begin();
		layer++;// we skip the layer "All" as iti is always present
		auto layerEnd = study->layers.end();
		for(; layer != layerEnd; layer++)
			pSpotlight->onMapLayerAdded(new wxString(layer->second));
	}

	void Component::backToInputData(void*)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;

		stopMerging();
		closeSubFrames();
		pCurrentLayerForMenu = nullptr;

		auto* mainFrm = Forms::ApplWnd::Instance();
		if (mainFrm)
			mainFrm->backToInputData();
	}


	void Component::onStudyClosed()
	{
		GUILocker locker;

		closeSubFrames();
		clear();

		if (pParentWindow)
		{
			pParentWindow->Close();
		}
		else
		{
			// When attached to the main window, we have to reset
			// the selections
			pCurrentSelectionType = stNone;
			pCurrentLOD = lodAllMCYears;
			pCurrentYear = 1;
			pHasYearByYear = false;
			pHasConcatenedDataset = false;
			pCurrentVariable = defaultVariable;;
			pCurrentVariableID = defaultVariableID;
			pYearsLimits[0] = 0;
			pYearsLimits[1] = 0;
		}
	}


	void Component::clear()
	{
		GUILocker locker;
		closeSubFrames();
		stopMerging();

		// Panels
		for (uint i = 0; i != (uint) pPanelAllOutputs.size(); ++i)
		{
			if (pPanelAllOutputs[i])
				pPanelAllOutputs[i]->Destroy();
		}

		pPanelAllOutputs.clear();
		pTabs.clear();

		pCurrentLayerForMenu = nullptr;
		pCurrentAreaOrLink.clear();
		pCurrentSelectionType = stNone;
		pOutputs.clear();
		outputSelectionsClear();
		pCurrentVariable.clear();
		pCurrentVariableID.clear();

		internalUpdate();
		emptyCache();

		// Removing all components
		auto* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
		pControlsAlreadyCreated = false;

		delete pCurrentClusters;
		pCurrentClusters = nullptr;

		// Resetting pointers, for code safety
		pSizerForPanelOutputs     = nullptr;
		pWindowForPanelOutputs    = nullptr;
		pBtnAllYears              = nullptr;
		pSizerYearsToolbar        = nullptr;
		pSizerYearByYearToolbar   = nullptr;
		pSizerVariables           = nullptr;
		pSizerToolbar             = nullptr;
		pLblAreaOrLinkName        = nullptr;
		pSpotlight                = nullptr;
		pSplitter                 = nullptr;
		pSplitterWindow1          = nullptr;
		pSplitterWindow2          = nullptr;
		pOldSidebarWidth          = 200;
		pBtnExpandCollapseSidebar = nullptr;
		pLabelItemName            = nullptr;
		pYearsLimits[0]           = 0;
		pYearsLimits[1]           = 1;

		pBtnValues  = nullptr;
		pBtnDetails = nullptr;
		pBtnID      = nullptr;

		pBtnHourly  = nullptr;
		pBtnDaily   = nullptr;
		pBtnWeekly  = nullptr;
		pBtnMonthly = nullptr;
		pBtnAnnual  = nullptr;

		pBtnCurrentYearNumber = nullptr;
		pBtnVariables = nullptr;

		// remove all mutex for file locking
		Antares::Private::OutputViewerData::ClearAllMutexForFileLocking();
	}


	void Component::update(const Data::Output::List& list, const Data::Output::Ptr& selection)
	{
		GUILocker locker;
		// Close any sub frames
		closeSubFrames();
		// Stop any merge currently in progress
		stopMerging();

		pCurrentLayerForMenu = nullptr;
		outputSelectionsClear();
		if (list.empty())
		{
			// Nothing to do
			clear();
			return;
		}

		// The copy operation is useless and dangerous if the two variables are actually
		// the same.
		if (&pOutputs != &list)
			pOutputs = list;
		outputSelectionsAdd(selection);

		if (!pControlsAlreadyCreated)
			createAllControlsIfNeeded();

		// Refresh all tabs
		refreshAllTabs();
	}


	void Component::onDropDownComparison(Button&, wxMenu&, void* userdata)
	{
		if (GUIIsLock() or !userdata)
			return;
		Button* btn = (Button*) userdata;
		displayMiniFrame(btn, new Provider::Comparison(*this), 120, false, false);
	}


	void Component::onDropDownOutputs(Button& button, wxMenu&, void* userdata)
	{
		if (GUIIsLock())
			return;

		// The current UI output info
		Layer* uiinfo = nullptr;
		// The current study output info
		Data::Output::Ptr outputSelected;

		if (userdata)
		{
			uiinfo = reinterpret_cast<Layer*>(userdata);
			outputSelected = uiinfo->selection;
			pCurrentLayerForMenu = uiinfo;
		}
		else
			pCurrentLayerForMenu = nullptr;

		if (uiinfo && uiinfo->isVirtual())
		{
			// In this case, there will be only one item "close the tab"
			displayMiniFrame(&button, new Provider::Outputs(*this, uiinfo), 150, false, false);
		}
		else
			displayMiniFrame(&button, new Provider::Outputs(*this, uiinfo), 380, true, false);
	}


	void Component::onDropDownDetachMenu(Button&, wxMenu& menu, void* userdata)
	{
		if (GUIIsLock())
			return;

		closeSubFrames();

		// The current UI output info
		Layer* uiinfo = nullptr;

		// Retrieving the layer
		if (userdata)
		{
			uiinfo = reinterpret_cast<Layer*>(userdata);
			pCurrentLayerForMenu = uiinfo;
		}
		else
			pCurrentLayerForMenu = nullptr;

		if (!uiinfo or uiinfo->isVirtual() or !pCurrentLayerForMenu)
			return;

		//wxWindow* form = (pParentWindow) ? pParentWindow : Forms::ApplWnd::Instance();
		auto* form = this;
		assert(form);

		if (!uiinfo->detached)
		{
			wxString caption;
			switch (pCurrentSelectionType)
			{
				case stArea: caption = wxT("Pin the area");break;
				case stLink: caption = wxT("Pin the link");break;
				default: return; // Uh ?
			}
			auto* it = Menu::CreateItem(&menu, wxID_ANY, caption,
				"images/16x16/pin.png", wxEmptyString);
			form->Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
				wxCommandEventHandler(Component::onDetachCurrentLayer), nullptr, form);
		}
		else
		{
			Layer* layer = pCurrentLayerForMenu;
			assert(layer);
			assert(layer->index < pTabs.size());
			Tab::Ptr tab = pTabs[layer->index];
			assert(!(!tab));

			if (tab->btnItem)
				displayMiniFrame(tab->btnItem, new SpotlightProvider(this, layer));
		}
	}


	void Component::onDetachCurrentLayer(wxCommandEvent&)
	{
		if (GUIIsLock())
			return;

		// Close any sub frames, just to be sure
		closeSubFrames();

		if (!pCurrentLayerForMenu)
			return;
		Layer& layer = *pCurrentLayerForMenu;
		if (layer.detached)
			return;

		// Copying the global selection
		layer.customSelectionType = pCurrentSelectionType;
		layer.customAreaOrLink = pCurrentAreaOrLink;
		layer.detached = true;
		// Update all buttons
		Dispatcher::GUI::Post(this, &Component::updateAreaOrLinkName);
	}


	void Component::createNewVirtualLayer(LayerType type)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		Layer* layer = new Layer(type);
		layer->index = (uint) pSelections.size();
		pSelections.push_back(layer);

		rebuildIndexes();
		refreshAllTabs();
	}


	void Component::selectAnotherOutput(const Data::Output::Ptr& selectedOutput)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		// Close any sub frames, just to be sure
		closeSubFrames();

		// Looking for the good output
		foreach (Data::Output::Ptr output, pOutputs)
		{
			if (output == selectedOutput)
			{
				if (pCurrentLayerForMenu)
				{
					// The layer of the current panel (which has the focus) is about
					// to be replaced.
					// However, we must keep some additional informations, like its
					// detached mode. Thus we have to make a copy of the layer.
					uint indexToUpdate = pCurrentLayerForMenu->index;
					auto* layer = new Layer(*pCurrentLayerForMenu);

					// Then to change the output selected
					layer->selection = output;
					// And to update other dependencies
					assert(indexToUpdate < (uint) pSelections.size());

					delete pSelections[indexToUpdate];
					pSelections[indexToUpdate] = layer;
					rebuildIndexes();
				}
				else
					outputSelectionsAdd(output);

				// reset
				pCurrentLayerForMenu = nullptr;

				// Refresh all tabs
				refreshAllTabs();
				refreshAllPanelsWithVirtualLayer();
				//Dispatcher::GUI::Post(this, &Component::refreshAllTabs);
				//Dispatcher::GUI::Post(this, &Component::refreshAllPanels);
				pSizerForPanelOutputs->Layout();
				//Refresh();
				return;
			}
		}

		pCurrentLayerForMenu = nullptr;
	}


	void Component::internalUpdate()
	{
		GUILocker locker;

		// Close any sub frames, just to be sure. At this point, no sub-frame
		// must remain. Otherwise the pointer might be invalid
		closeSubFrames();

		if (!pControlsAlreadyCreated or !pSizerForBtnOutputs)
			return;

		// reset the pointer
		pCurrentLayerForMenu = nullptr;

		if (IsGUIAboutToQuit())
			return;

		if (pSelections.empty())
		{
			if (!pBtnPanelAllOutputs.empty())
			{
				for (uint i = 0; i != pBtnPanelAllOutputs.size(); ++i)
				{
					if (pBtnPanelAllOutputs[i])
						pBtnPanelAllOutputs[i]->Destroy();
				}
				pBtnPanelAllOutputs.clear();
			}
			pTabs.clear();
			if (!pPanelAllOutputs.empty())
			{
				for (uint i = 0; i != pPanelAllOutputs.size(); ++i)
				{
					if (pPanelAllOutputs[i])
						pPanelAllOutputs[i]->Destroy();
				}
				pPanelAllOutputs.clear();
			}
			return;
		}

		bool forceRefreshCaption = (pTabs.size() != pSelections.size());

		// Looking for incremental changes
		// For the first step, we will remove all controls which are no
		// longer needed
		if (pTabs.size() > pSelections.size())
		{
			uint start = (uint) pSelections.size();
			uint end   = (uint) pBtnPanelAllOutputs.size();
			for (uint i = start; i < end; ++i)
			{
				if (pBtnPanelAllOutputs[i])
					pBtnPanelAllOutputs[i]->Destroy();
			}
			pTabs.resize(pSelections.size());
			pBtnPanelAllOutputs.resize(pSelections.size());
		}
		if (pPanelAllOutputs.size() > pSelections.size())
		{
			uint start = (uint) pSelections.size();
			uint end   = (uint) pPanelAllOutputs.size();
			for (uint i = start; i < end; ++i)
			{
				if (pPanelAllOutputs[i])
					pPanelAllOutputs[i]->Destroy();
			}
			pPanelAllOutputs.resize(pSelections.size());
		}

		// Current year number
		refreshCurrentYear();

		// The second step consists in adding the missing controls or to update
		// them according to the new output to display
		for (uint i = 0; i != pSelections.size(); ++i)
		{
			// The current UI output info
			Layer& uiinfo = *(pSelections[i]);
			assert(pSelections[i] && "invalid uiinfo");
			// The current control
			Tab::Ptr tab;

			if (i < pTabs.size())
			{
				// Ok the control already exists
				tab = pTabs[i];
				assert(tab && "invalid tab");
				assert(tab->btnSizer && "invalid sizer");
			}
			else
			{
				tab = new Tab(*this);
				pTabs.push_back(tab);

				// Obviously the control does not exist yet. We have to create it
				auto* support = new Antares::Component::Panel(pWindowForBtnOutputs);
				tab->support = support;
				auto* hz = new wxBoxSizer(wxHORIZONTAL);
				support->SetSizer(hz);

				// The panel support
				auto* tabpanel = new Antares::Component::Panel(support);
				Antares::Component::PanelGroup::SetDarkBackgroundColor(tabpanel);
				auto* tabsizer = new wxBoxSizer(wxHORIZONTAL);
				tab->btnSizer = tabsizer;
				tabpanel->SetSizer(tabsizer);
				tabsizer->AddSpacer(2);

				// The button for displaying all available outputs
				auto* btn = new Button(tabpanel, wxT("Loading"), "images/16x16/param_reset.png",
					this, &Component::backToInputData);
				btn->SetForegroundColour(wxColour(250, 250, 255));
				btn->menu(true);
				btn->onPopupMenu(this, &Component::onDropDownOutputs);
				tabsizer->Add(btn, 0, wxALL|wxEXPAND);
				tab->btnOutput = btn;

				// This button is always created, even if not required
				// (this is the case for example when the tab has a virtual
				// layer).
				// This component may be required in the following case :
				//  - Add a tab, displaying a real file
				//  - Add a virtual layer
				//  - Add a tab displaying another real file
				//  - Remove the tab with the virtual layer
				btn = new Button(tabpanel, wxT("Area or link"), "images/16x16/area.png",
					this, &Component::backToInputData);
				btn->SetForegroundColour(wxColour(250, 250, 255));
				btn->onPopupMenu(this, &Component::onDropDownDetachMenu);
				btn->menu(true);
				tab->btnItem = btn;
				tabsizer->Add(btn, 0, wxALL|wxEXPAND);
				tab->btnCustomSelection = btn;

				tabsizer->AddSpacer(2);
				hz->Add(tabpanel, 0, wxALL|wxEXPAND);
				hz->AddStretchSpacer();

				pSizerForBtnOutputs->Add(support, 1, wxALL|wxEXPAND);
			}

			// The button "area" (where it is possible to detach from the
			// global selection) may have to be hidden
			tab->btnSizer->Show(tab->btnItem, !uiinfo.isVirtual());
			tab->btnSizer->Layout();

			Antares::Private::OutputViewerData::Panel* panel;
			if (i < pPanelAllOutputs.size())
			{
				// Ok the control already exists
				panel = pPanelAllOutputs[i];
			}
			else
			{
				// Obviously the control does not exist yet. We have to create it
				assert(pWindowForPanelOutputs && "invalid parent");
				panel = new Antares::Private::OutputViewerData::Panel(this, pWindowForPanelOutputs);
				pSizerForPanelOutputs->Add(panel, 1, wxALL|wxEXPAND | wxFIXED_MINSIZE);
				pPanelAllOutputs.push_back(panel);
			}

			// Update the panel if required
			panel->index(i + 1);
			panel->layer(&uiinfo);

			Button* btn = tab->btnOutput;
			assert(btn && "invalid button");



			// We may have to update the control
			if (static_cast<void*>(&uiinfo) != btn->userdata())
			{
				// The current study info
				Data::Output::Ptr outputInfo = uiinfo.selection;

				// Image
				assert((i + 1 < 10) && "We only have 10 images");
				btn->image(imageNumber[i + 1]);
				// User data
				btn->userdata(&uiinfo);

				if (tab->btnCustomSelection)
					tab->btnCustomSelection->userdata(&uiinfo);

				if (!(!outputInfo))
					btn->caption(wxStringFromUTF8(outputInfo->title));
				else
					btn->caption(uiinfo.alternativeCaption());
			}
			else
			{
				if (forceRefreshCaption && !(!uiinfo.selection))
				{
					// The current study info
					Data::Output::Ptr outputInfo = uiinfo.selection;
					// Caption of the button
					btn->caption(wxStringFromUTF8(outputInfo->title));
					btn->image(imageNumber[i + 1]);
				}
			}

			// Updating the layout
			assert(btn->GetParent());
			assert(btn->GetParent()->GetParent());
			assert(btn->GetParent()->GetParent()->GetSizer());
			btn->GetParent()->GetParent()->GetSizer()->Layout();
		}

		// Refresh button which displays the area or link name
		updateAreaOrLinkName();

		// Update the layout
		Dispatcher::GUI::Layout(pSizerForBtnOutputs);
		Dispatcher::GUI::Layout(pSizerForPanelOutputs);

		pSizerYearsToolbar->Layout();
		pSizerToolbar->Layout();
		Layout();
		Refresh();
	}


	void Component::treeDataClear()
	{
		GUILocker locker;
		if (pSpotlight)
		{
			auto* provider = new SpotlightProviderGlobalSelection(this);
			pSpotlight->provider(provider);
		}
	}


	void Component::treeDataUpdate()
	{
		assert(wxIsMainThread() == true && "Must be ran from the main thread");
		GUILocker locker;

		pMutex.lock();
		if (pJobsRemaining)
		{
			// We must do nothing if this value is not null
			logs.debug() << "treeDataUpdate: sorry but impossible to update. nb jobs: "
				<< pJobsRemaining << ", node ";
			pMutex.unlock();
			return;
		}

		pHasYearByYear = false;
		pHasConcatenedDataset = false;

		auto* provider = new SpotlightProviderGlobalSelection(this);
		pSpotlight->provider(provider);


		// All clusters, for all areas
		delete pCurrentClusters;
		pCurrentClusters = new Antares::Private::OutputViewerData::ThermalNameSetPerArea();

		bool foundAnOutput = false;

		// Looking for the first real output (and not a virtual layer)
		for (uint i = 0; i != (uint) pSelections.size(); ++i)
		{
			auto* layer = pSelections[i];
			// We may not have a study output info for virtual layers
			if (not layer or not layer->selection)
				continue;

			// We will use the first available as reference
			const String& path = (layer->selection)->path;

			typedef Antares::Private::OutputViewerData::ContentMap  ContentMap;
			ContentMap::const_iterator ci = pAlreadyPreparedContents.find(path);
			if (ci != pAlreadyPreparedContents.end())
			{
				auto* content = ci->second;
				assert(content);

				if (content)
				{
					// We only want a non-empty content
					if (content->empty())
						continue;
					foundAnOutput = true;
					treeDataUpdateWL(*content);
				}
			}
			break;
		}

		// unlock the mutex
		pMutex.unlock();

		if (!foundAnOutput)
			noSimulationData();
	}


	void Component::treeDataUpdateWL(const Content& content)
	{
		GUILocker locker;

		typedef Antares::Private::OutputViewerData::Content::AreaNameSet  AreaNameSet;
		typedef Antares::Private::OutputViewerData::Content::LinkNameSet  LinkNameSet;

		bool economy = !(content.economy.empty());
		bool hasFoundSomething = false;
		AreaNameSet uncommonAreas;
		LinkNameSet uncommonLinks;

		const AreaNameSet* areas;
		const LinkNameSet* links;
		if (economy)
		{
			areas = &(content.economy.areas);
			links = &(content.economy.links);
		}
		else
		{
			areas = &(content.adequacy.areas);
			links = &(content.adequacy.links);
		}

		assert(areas && "invalid list of areas");
		assert(links && "invalid list of links");

		auto* provider = new SpotlightProviderGlobalSelection(this);

		// year by year
		checkYearByYearMode();

		//if (economy)
		//	provider->addEconomy();
		//else
		//	provider->addAdequacy();

		// Browsing all areas, adding only areas available in all outputs
		if (not areas->empty())
		{
			auto areaEnd = areas->end();

			// All groups only
			// We want all groups before all areas
			for (auto areaIt = areas->begin(); areaIt != areaEnd; ++areaIt)
			{
				if (areaIt->first() == '@')
				{
					if (!checkAreaIsCommonToAllOutputs(*areaIt))
						uncommonAreas.insert(*areaIt);
					else
					{
						hasFoundSomething = true;
						provider->addSetName(*areaIt);
					}
				}
			}
			for (auto areaIt = areas->begin(); areaIt != areaEnd; ++areaIt)
			{
				if (areaIt->first() != '@')
				{
					if (!checkAreaIsCommonToAllOutputs(*areaIt))
						uncommonAreas.insert(*areaIt);
					else
					{
						hasFoundSomething = true;
						provider->addAreaName(*areaIt);
					}
				}
			}
		}

		// Browsing all links, adding only links available in all outputs
		if (not links->empty())
		{
			auto linkEnd = links->end();
			for (auto linkIt = links->begin(); linkIt != linkEnd; ++linkIt)
			{
				if (!checkLinkIsCommonToAllOutputs(*linkIt))
					uncommonLinks.insert(*linkIt);
				else
				{
					hasFoundSomething = true;
					provider->addLinkName(*linkIt);
				}
			}
		}

		// Clusters
		if (not content.clusters.empty() && pCurrentClusters)
		{
			auto areaEnd = content.clusters.end();
			for (auto areaI = content.clusters.begin(); areaI != areaEnd; ++areaI)
			{
				auto& areaname = areaI->first;
				auto end = areaI->second.end();
				auto i = areaI->second.begin();
				for (; i != end; ++i)
					(*pCurrentClusters)[areaname].insert(*i);
			}
		}

		// If nothing, adding a label for notifying the user that there is no problem
		// but no data
		if (!hasFoundSomething)
			provider->addNoCommonItem();

		// Adding (at the end), all areas / links that are not available
		// in all outputs
		if (not uncommonAreas.empty() or not uncommonLinks.empty())
		{
			if (not uncommonAreas.empty())
			{
				auto areaEnd = uncommonAreas.end();
				for (auto areaIt = uncommonAreas.begin(); areaIt != areaEnd; ++areaIt)
					provider->addUncommonAreaName(*areaIt);
			}

			if (not uncommonLinks.empty())
			{
				auto linkEnd = uncommonLinks.end();
				for (auto linkIt = uncommonLinks.begin(); linkIt != linkEnd; ++linkIt)
					provider->addUncommonLinkName(*linkIt);
			}
		}

		// Flushing
		pSpotlight->provider(provider);
	}


	void Component::noSimulationData()
	{
		if (pSpotlight)
		{
			auto* provider = new SpotlightProviderGlobalSelection(this);
			provider->addText("(No simulation data)");
			pSpotlight->provider(provider);
		}

		pHasYearByYear = false;
		pHasConcatenedDataset = false;
	}


	void Component::treeDataWaiting()
	{
		if (pSpotlight)
		{
			auto* provider = new SpotlightProviderGlobalSelection(this);
			provider->addText("loading...");
			pSpotlight->provider(provider);
		}
	}



	void Component::copyFrom(const Component& source)
	{
		if (&source == this)
			return;

		GUILocker locker;

		pOutputs    = source.pOutputs;

		if (not pSelections.empty())
		{
			for (uint i = 0; i != (uint) pSelections.size(); ++i)
				delete pSelections[i];
			pSelections.clear();
		}

		for (uint i = 0; i != (uint) source.pSelections.size(); ++i)
		{
			assert(source.pSelections[i]);
			pSelections.push_back(new Layer(*(source.pSelections[i])));
		}

		if (!pControlsAlreadyCreated)
			createAllControlsIfNeeded();

		// Retrieving filter values
		// Data level
		pBtnValues  -> pushed(source.pBtnValues->pushed());
		pBtnDetails -> pushed(source.pBtnDetails->pushed());
		pBtnID      -> pushed(source.pBtnID->pushed());
		// Time level
		pBtnHourly  -> pushed(source.pBtnHourly->pushed());
		pBtnDaily   -> pushed(source.pBtnDaily->pushed());
		pBtnWeekly  -> pushed(source.pBtnWeekly->pushed());
		pBtnMonthly -> pushed(source.pBtnMonthly->pushed());
		pBtnAnnual  -> pushed(source.pBtnAnnual->pushed());

		// Current selection
		pCurrentAreaOrLink    = source.pCurrentAreaOrLink;
		pCurrentSelectionType = source.pCurrentSelectionType;
		pCurrentLOD           = source.pCurrentLOD;
		pCurrentYear          = source.pCurrentYear;
		pHasYearByYear        = source.pHasYearByYear;
		pHasConcatenedDataset = source.pHasConcatenedDataset;
		pYearsLimits[0]       = source.pYearsLimits[0];
		pYearsLimits[1]       = source.pYearsLimits[1];
		pCurrentVariable      = source.pCurrentVariable;
		pCurrentVariableID    = source.pCurrentVariableID;

		// Visual Update
		pBtnVariables->caption(wxStringFromUTF8(pCurrentVariable));

		updateButtonView();

		// Starting the refresh
		internalUpdate();
		Dispatcher::GUI::Post(this, &Component::updateAreaOrLinkName);
		// Start the merge
		Dispatcher::GUI::Post(this, &Component::mergeOutputs);
	}


	void Component::openNewWindow(void*)
	{
		if (GUIIsLock())
			return;

		auto* mainFrm = Forms::ApplWnd::Instance();
		if (mainFrm)
		{
			// Close any sub-frames
			closeSubFrames();

			auto* window = new OutputViewer::Window(mainFrm, this);
			window->Show();

			// The layout must be updated (especially on Windows)
			auto* sizer = window->GetSizer();
			if (sizer)
				sizer->Layout();
		}
	}


	void Component::outputSelectionsClear()
	{
		pCurrentLayerForMenu = nullptr;

		if (!pPanelAllOutputs.empty())
		{
			// invalid all panels
			for (uint i = 0; i != pPanelAllOutputs.size(); ++i)
			{
				assert(pPanelAllOutputs[i] && "invalid panel output");
				if (pPanelAllOutputs[i])
					pPanelAllOutputs[i]->layer(nullptr);
			}
		}
		if (!pTabs.empty())
		{
			// In a first time, we will remove all references, to avoid dead
			// pointers, just in case
			for (uint i = 0; i != pTabs.size(); ++i)
			{
				assert(!(!pTabs[i]));
				if (pTabs[i])
				{
					auto& tab = *(pTabs[i]);
					if (tab.btnOutput)
						tab.btnOutput->userdata(nullptr);
					if (tab.btnCustomSelection)
						tab.btnCustomSelection->userdata(nullptr);
				}
			}
		}

		// We will delete all selections
		if (!pSelections.empty())
		{
			for (uint i = 0; i != pSelections.size(); ++i)
				delete pSelections[i];
			pSelections.clear();
		}
	}


	uint Component::outputSelectionsAdd(Data::Output::Ptr output)
	{
		if (pSelections.size() >= 9)
		{
			Antares::Window::Message message(this, wxT("Output viewer"),
				wxT("Impossible to add a new tab"),
				wxT("You can not open more than 9 tabs per window"));
			message.add(Antares::Window::Message::btnContinue, true);
			message.showModal();
			return (uint) -1;
		}
		else
		{
			auto* layer = new Layer();
			layer->index = (uint) pSelections.size();
			layer->selection = output;
			pSelections.push_back(layer);
			return layer->index;
		}
	}


	const char* Component::imageForLayerPlaceOrder(const Layer* so) const
	{
		if (!so or pSelections.empty())
			return "images/16x16/empty.png";

		uint count = 0;
		const char* image = "images/16x16/empty.png";

		for (uint i = 0; i != pSelections.size(); ++i)
		{
			if (pSelections[i] == so)
			{
				++count;
				if (count > 1)
				{
					if (count == 2)
						image = "images/16x16/000.png";
				}
				else
					image = imageNumber[i + 1];
			}
		}
		return image;
	}


	const char* Component::imageForLayerPlaceOrder(const Data::Output::Ptr& so) const
	{
		if (!so or pSelections.empty())
			return "images/16x16/empty.png";

		uint count = 0;
		const char* image = "images/16x16/empty.png";

		for (uint i = 0; i != pSelections.size(); ++i)
		{
			if (!(!pSelections[i]) && pSelections[i]->selection == so)
			{
				++count;
				if (count > 1)
				{
					if (count == 2)
						image = "images/16x16/000.png";
				}
				else
					image = imageNumber[i + 1];
			}
		}
		return image;
	}


	int Component::imageIndexForOutput(const Data::Output::Ptr& so) const
	{
		if (!so or pSelections.empty())
			return -2;

		uint count = 0;
		int index = -2;

		for (uint i = 0; i != pSelections.size(); ++i)
		{
			if (!(!pSelections[i]) && pSelections[i]->selection == so)
			{
				++count;
				if (count > 1)
				{
					if (count == 2)
						return -1;
				}
				else
					index = i;
			}
		}
		return index;
	}


	void Component::rebuildIndexes()
	{
		assert(wxIsMainThread() == true && "Must be ran from the main thread");

		uint count = (uint) pSelections.size();
		for (uint i = 0; i != count; ++i)
		{
			assert(pSelections[i]);
			if (pSelections[i])
				pSelections[i]->index = i;
		}
	}



	void Component::removeOutput(Layer* layer)
	{
		if (!layer or GUIIsLock())
			return;

		GUILocker locker;
		// Close any sub-frames
		closeSubFrames();

		uint index = 0;
		auto end = pSelections.end();
		for (auto i = pSelections.begin(); i != end; ++i, ++index)
		{
			assert(*i);
			if ((*i) == layer)
			{
				// Removing any reference to the layer to
				// avoid any race condition
				for (uint x = index; x < pPanelAllOutputs.size(); ++x)
				{
					assert(pPanelAllOutputs[x]);
					if (pPanelAllOutputs[x])
						pPanelAllOutputs[x]->layer(nullptr);
				}

				stopMerging();
				pSelections.erase(i);
				rebuildIndexes();
				// Refresh all tabs
				refreshAllTabs();
				refreshAllPanelsWithVirtualLayer();
				return;
			}
		}
	}


	void Component::onSelectDataLevel(void* userdata)
	{
		assert(wxIsMainThread() == true && "Must be ran from the main thread");
		assert(userdata && "Some user data are expected");

		if (GUIIsLock())
			return;

		GUILocker locker;
		// Close any sub frames, just to be sure
		closeSubFrames();

		pBtnValues  -> pushed((userdata == pBtnValues));
		pBtnDetails -> pushed((userdata == pBtnDetails));
		pBtnID      -> pushed((userdata == pBtnID));
		// refresh
		refreshAllPanels();
	}


	void Component::onSelectTimeLevel(void* userdata)
	{
		assert(wxIsMainThread() == true && "Must be ran from the main thread");
		assert(userdata && "Some user data are expected");

		if (GUIIsLock())
			return;

		GUILocker locker;
		// Close any sub frames, just to be sure
		closeSubFrames();

		pBtnHourly  -> pushed((userdata == pBtnHourly));
		pBtnDaily   -> pushed((userdata == pBtnDaily));
		pBtnWeekly  -> pushed((userdata == pBtnWeekly));
		pBtnMonthly -> pushed((userdata == pBtnMonthly));
		pBtnAnnual  -> pushed((userdata == pBtnAnnual));
		// refresh
		refreshAllPanels();
	}



	void Component::mergeOutputs()
	{
		// From the main thread only
		assert(wxIsMainThread() == true && "Must be ran from the main thread");

		GUILocker locker;
		// There is no need to flush the memory, especially when dealing
		// with several threads
		MemoryFlushLocker  memoryLocker;

		stopMerging();

		if (pSelections.empty())
		{
			treeDataClear();
			return; // nothing to do
		}

		// We will use a dictionary to avoid dupplicate scan
		// (however this case should often append)
		typedef std::set<String>  ScanSet;
		ScanSet scandirs;

		// We will look for all paths which wait for being analyzed
		// If no one remains, we will update the tree ctrl.
		pMutex.lock();
		// Launching all sub-process
		foreach (auto* layer, pSelections)
		{
			// We may not have a study output info for virtual layers
			if (!layer or !layer->selection)
				continue;
			// The path of the output
			const String& path = (layer->selection)->path;

			// We must analyze it if not already in cache
			if (pAlreadyPreparedContents.find(path) == pAlreadyPreparedContents.end())
				scandirs.insert(path);
		}
		pJobsRemaining = (uint) scandirs.size();
		pMutex.unlock();

		if (scandirs.empty())
		{
			Dispatcher::GUI::Post(this, &Component::treeDataUpdate);
		}
		else
		{
			// Notifying the user that something is running behind the
			// hood
			Dispatcher::GUI::Post(this, &Component::treeDataWaiting);

			// Alias
			typedef Antares::Private::OutputViewerData::Job  Job;
			typedef Antares::Private::OutputViewerData::JobPtr  JobPtr;

			// For all folders which remain to be analyzed
			auto end = scandirs.end();
			for (auto i = scandirs.begin(); i != end; ++i)
			{
				JobPtr job = new Job(*this, *i);
				pJobs.push_back(job);
				Dispatcher::Post((const Yuni::Job::IJob::Ptr&) job);
			}
		}
	}


	void Component::stopMerging()
	{
		assert(wxIsMainThread() == true && "Must be ran from the main thread");

		if (!pJobs.empty())
		{
			pMutex.lock();
			// Invalid value to avoid useless refresh while stopping all jobs
			pJobsRemaining = (uint) -1;
			pMutex.unlock();

			uint count = (uint) pJobs.size();

			for (uint i = 0; i != count; ++i)
			{
				auto& job = pJobs[i];
				if (!(!job))
					job->cancel();
			}
			pJobs.clear();

			pMutex.lock();
			// Keeping this value invalid
			pJobsRemaining = (uint) -1;
			pMutex.unlock();
		}

		// We must lock then unlock the mutex to avoid race condition
		// This will make sure that no remaining job is currently doing something
		pMutex.lock();
		pMutex.unlock();
	}


	void Component::createSubMenuForSwitchingOrders(wxMenu* menu, const Layer* currentLayer) const
	{
		// Disabled for now
		return;

		if (pSelections.size() > 1)
		{
			wxMenuItem* it;
			it = Menu::CreateItemWithSubMenu(menu, wxID_ANY, wxT("Change order with"), "images/16x16/switch.png");
			wxMenu* newMenu = it->GetSubMenu();

			foreach (auto* layer, pSelections)
			{
				const Data::Output::Ptr& other = layer->selection;
				wxMenuItem* it;
				const char* const image = imageForLayerPlaceOrder(layer);
				if (!(!other))
				{
					const wxString& title = wxStringFromUTF8(other->title);
					it = Menu::CreateItem(newMenu, wxID_ANY, title, image, wxEmptyString);
				}
				else
				{
					const wxString& title = layer->alternativeCaption();
					it = Menu::CreateItem(newMenu, wxID_ANY, title, image, wxEmptyString);
				}
				if (layer == currentLayer)
					it->Enable(false);
			}
		}
	}


	bool Component::checkAreaIsCommonToAllOutputs(const Data::AreaName& name)
	{
		for (uint i = 0; i != pSelections.size(); ++i)
		{
			auto* layer = pSelections[i];
			// We may not have a study output info for virtual layers
			if (!layer or !layer->selection)
				continue;

			// We will use the first available as reference
			const String& path = (layer->selection)->path;

			typedef Antares::Private::OutputViewerData::Content     Content;
			typedef Antares::Private::OutputViewerData::ContentMap  ContentMap;

			auto ci = pAlreadyPreparedContents.find(path);
			if (ci == pAlreadyPreparedContents.end())
				return false;

			const Content* content = ci->second;
			assert(content && "Invalid output view content");
			if (!content)
				continue;

			if (!content->economy.areas.count(name) && !content->adequacy.areas.count(name))
				return false;
		}
		return true;
	}


	void Component::checkYearByYearMode()
	{
		// Reset
		pYearsLimits[0] = (uint) -1;
		pYearsLimits[1] = 0;
		pHasYearByYear = false;
		pHasConcatenedDataset = false;

		foreach (auto* layer, pSelections)
		{
			// We may not have a study output info for virtual layers
			if (!layer or !layer->selection)
				continue;

			// We will use the first available as reference
			const String& path = (layer->selection)->path;

			typedef Antares::Private::OutputViewerData::Content     Content;
			typedef Antares::Private::OutputViewerData::ContentMap  ContentMap;

			auto ci = pAlreadyPreparedContents.find(path);
			if (ci == pAlreadyPreparedContents.end())
				continue;

			const Content* content = ci->second;
			assert(content && "Invalid output view content");
			if (!content)
				continue;

			if (content->hasYearByYear)
			{
				assert(content->ybyInterval[0] > 0 && content->ybyInterval[0] < 500000);
				assert(content->ybyInterval[1] > 0 && content->ybyInterval[1] < 500000);

				pHasYearByYear = true;
				pHasConcatenedDataset = true;
				if (content->ybyInterval[0] < pYearsLimits[0])
					pYearsLimits[0] = content->ybyInterval[0];
				if (content->ybyInterval[1] > pYearsLimits[1])
					pYearsLimits[1] = content->ybyInterval[1];
			}
			else
			{
				if (content->hasConcatenedYbY)
					pHasConcatenedDataset = true;
			}
		}
	}


	bool Component::checkLinkIsCommonToAllOutputs(const Data::AreaLinkName& name)
	{
		for (uint i = 0; i != pSelections.size(); ++i)
		{
			Layer* layer = pSelections[i];
			// We may not have a study output info for virtual layers
			if (not layer or not layer->selection)
				continue;

			// We will use the first available as reference
			const String& path = (layer->selection)->path;

			typedef Antares::Private::OutputViewerData::Content     Content;
			typedef Antares::Private::OutputViewerData::ContentMap  ContentMap;

			ContentMap::const_iterator ci = pAlreadyPreparedContents.find(path);
			if (ci == pAlreadyPreparedContents.end())
				return false;

			const Content* content = ci->second;
			assert(content && "Invalid output view content");
			if (!content)
				continue;

			if (!content->economy.links.count(name) && !content->adequacy.links.count(name))
				return false;
		}
		return true;
	}



	void Component::bugFixResetSashPosition()
	{
		assert(pSplitter);
		pSplitter->SetSashPosition(250);
	}


	void Component::updateAreaOrLinkName()
	{
		if (pTabs.empty())
			return;

		GUILocker locker;
		assert(pTabs.size() == pSelections.size());
		for (uint i = 0; i != pTabs.size(); ++i)
		{
			if (!pTabs[i])
				continue;
			Tab& tab = *(pTabs[i]);
			Layer* layer = pSelections[i];
			if (layer && layer->detached)
			{
				String s;
				s << "* " << layer->customAreaOrLink;
				tab.updateAreaOrLinkName(layer->customSelectionType, s);
			}
			else
				tab.updateAreaOrLinkName(pCurrentSelectionType, pCurrentAreaOrLink);
		}

		// Rebuild the layout
		assert(pSizerForBtnOutputs);
		pSizerForBtnOutputs->Layout();
		RefreshAllControls(pWindowForBtnOutputs);
	}

	void Component::updateButtonView()
	{
		switch (pCurrentLOD)
		{
			case lodAllMCYears:
				{
					pBtnAllYears->caption(wxT("MC Synthesis"));
					pSizerYearsToolbar->Show(pSizerYearByYearToolbar, false);
					pSizerYearsToolbar->Show(pSizerVariables, false);
					break;
				}
			case lodDetailledResults:
				{
					pBtnAllYears->caption(wxT("Year-by-year"));
					pSizerYearsToolbar->Show(pSizerYearByYearToolbar, true);
					pSizerYearsToolbar->Show(pSizerVariables, false);
					refreshCurrentYear();
					break;
				}
			case lodDetailedResultsWithConcatenation:
				{
					pBtnAllYears->caption(wxT("Variable per variable"));
					pBtnVariables->caption(wxStringFromUTF8(pCurrentVariable));
					pSizerYearsToolbar->Show(pSizerYearByYearToolbar, false);
					pSizerYearsToolbar->Show(pSizerVariables, true);
					break;
				}
		}
		pSizerYearsToolbar->Layout();
	}


	void Component::updateGlobalSelection()
	{
		assert(pBtnAllYears);
		assert(pSizerYearsToolbar);
		assert(pBtnVariables);
		assert(pSizerToolbar);

		GUILocker locker;
		// Close any sub-frames
		closeSubFrames();

		updateButtonView();

		updateAreaOrLinkName();
		// Update
		//Dispatcher::GUI::Post(this, &Component::refreshAllTabs);
		Dispatcher::GUI::Post(this, &Component::refreshAllPanels);

		switch (pCurrentSelectionType)
		{
			case stArea:
					{
						auto study = Data::Study::Current::Get();
						if (!pOpenedInANewWindow && !(!study))
						{
							auto* area = study->areas.find(pCurrentAreaOrLink);
							if (area)
								Antares::Window::Inspector::SelectArea(area);
						}
						break;
					}
			case stLink:
					{
						break;
					}
			default:
					{
						break;
					}
		}

		pSizerToolbar->Layout();
		Layout();
		Refresh();
	}


	void Component::refreshPanel(uint index)
	{
		GUILocker locker;
		assert(index < (uint) pSelections.size());
		assert(index < (uint) pPanelAllOutputs.size());

		if (index < (uint) pSelections.size())
		{
			auto* panel = pPanelAllOutputs[index];
			if (panel)
			{
				panel->forceRefresh();

				auto* sizer = panel->GetSizer();
				if (sizer)
					sizer->Layout();
			}
		}
	}


	void Component::refreshAllPanels()
	{
		GUILocker locker;

		// For better efficiency and because virtual layers need to gather informations
		// from other panels, we should refresh them after all others
		bool hasVirtualLayers = false;

		for (uint i = 0; i != (uint) pSelections.size(); ++i)
		{
			assert(i < pPanelAllOutputs.size() && "integrity check failed");
			auto* panel = pPanelAllOutputs[i];
			if (panel)
			{
				auto* layer = panel->layer();
				if (layer && layer->isVirtual())
				{
					hasVirtualLayers = true;
					continue;
				}

				panel->forceRefresh();

				auto* sizer = panel->GetSizer();
				if (sizer)
					sizer->Layout();
			}
		}

		if (hasVirtualLayers)
		{
			for (uint i = 0; i != (uint) pSelections.size(); ++i)
			{
				assert(i < pPanelAllOutputs.size() && "integrity check failed");
				auto* panel = pPanelAllOutputs[i];
				if (panel)
				{
					auto* layer = panel->layer();
					if (layer && layer->isVirtual())
					{
						panel->forceRefresh();

						auto* sizer = panel->GetSizer();
						if (sizer)
							sizer->Layout();
					}
				}
			}
		}
	}


	void Component::refreshAllPanelsWithVirtualLayer()
	{
		GUILocker locker;
		for (uint i = 0; i != (uint) pPanelAllOutputs.size(); ++i)
		{
			auto* panel = pPanelAllOutputs[i];
			if (panel)
			{
				auto* layer = panel->layer();
				if (layer && layer->isVirtual())
				{
					panel->forceRefresh();

					auto* sizer = panel->GetSizer();
					if (sizer)
						sizer->Layout();
				}
			}
		}
	}


	void Component::scrollAllPanels(wxScrolledWindow* component)
	{
		pOnScrollAllPanels(component);
	}


	void Component::closeSubFrames()
	{
		Antares::Component::Spotlight::FrameClose();
	}


	void Component::refreshAllTabs()
	{
		GUILocker locker;
		// Ask for a refresh and a merge
		internalUpdate();
		mergeOutputs();
	}


	void Component::onToggleSidebar(void*)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		if (pSidebarCollapsed)
		{
			pSplitter->SplitVertically(pSplitterWindow1, pSplitterWindow2, pOldSidebarWidth);
			pBtnExpandCollapseSidebar->SetToolTip(wxT("Hide the sidebar"));
			pBtnExpandCollapseSidebar->image("images/16x16/sidebar_collapse.png");
		}
		else
		{
			pOldSidebarWidth = pSplitter->GetSashPosition();
			pSplitter->Unsplit(pSplitterWindow1);
			pBtnExpandCollapseSidebar->SetToolTip(wxT("Show the sidebar"));
			pBtnExpandCollapseSidebar->image("images/16x16/sidebar_expand.png");
		}

		pSidebarCollapsed = !pSidebarCollapsed;
	}


	void Component::allYearsOrYearByYearSelector(Button&, wxMenu&, void*)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		closeSubFrames();
		displayMiniFrame(pBtnAllYears, new SpotlightProviderMCAll(this), 220, false, false);
	}


	void Component::incrementYearByYear(void*)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		// The upper bound
		const uint bound = pYearsLimits[1];

		if (pCurrentYear < bound)
			++pCurrentYear;
		else
		{
			if (pCurrentYear > bound)
				pCurrentYear = bound;
			else
				return;
		}

		// Update
		Dispatcher::GUI::Post(this, &Component::refreshAllTabs);
		Dispatcher::GUI::Post(this, &Component::refreshAllPanels);
	}


	void Component::goToASpecificYear(void*)
	{
		if (GUIIsLock())
			return;

		wxString out;
		out << pCurrentYear; // Default value

		wxString caption;
		caption << wxT("Year number (");
		caption << pYearsLimits[0] << wxT("..") << pYearsLimits[1] << wxT(") ");

		if (Antares::Window::AskForInput(out, wxT("Individual year"), caption))
		{
			if (out.empty())
				return;

			// Lock the GUI
			{
				GUILocker locker;

				String valuestr;
				wxStringToString(out, valuestr);
				valuestr.trim();
				valuestr.toLower();

				uint value;
				if (valuestr.to(value))
				{
					if (value >= pYearsLimits[0]  && value <= pYearsLimits[1])
					{
						pCurrentYear = value;

						// Update all visualized data
						Dispatcher::GUI::Post(this, &Component::refreshAllTabs);
						Dispatcher::GUI::Post(this, &Component::refreshAllPanels);
					}
					return;
				}
			}

			Antares::Window::Message message(this, wxT("Output viewer"),
				wxT("Individual year"), wxT("The value is invalid"));
			message.add(Antares::Window::Message::btnContinue, true);
			message.showModal();
		}
	}


	void Component::decrementYearByYear(void*)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		// The lower bound
		uint bound = pYearsLimits[0];

		if (pCurrentYear > bound)
			--pCurrentYear;
		else
		{
			if (pCurrentYear > bound)
				pCurrentYear = bound;
			else
				return;
		}

		Dispatcher::GUI::Post(this, &Component::refreshAllTabs);
		Dispatcher::GUI::Post(this, &Component::refreshAllPanels);
	}



	void Component::dropDownAllVariables(Button& button, wxMenu&, void*)
	{
		if (GUIIsLock())
			return;

		GUILocker locker;
		closeSubFrames();
		displayMiniFrame(&button, new Provider::Variables(this), 350);
	}


	void Component::refreshCurrentYear()
	{
		wxString c;
		c << pCurrentYear << wxT(" / ") << pYearsLimits[1];
		pBtnCurrentYearNumber->caption(c);
	}


	void Component::ConvertVarNameToID(String& id, const String& name)
	{
		id.clear();

		bool previousSpace = true;
		const String::const_utf8iterator end = name.utf8end();
		for (String::const_utf8iterator i = name.utf8begin(); i != end; ++i)
		{
			char c = String::ToLower((char)(*i));
			if (!c or (not String::IsDigit(c) and not String::IsAlpha(c)))
			{
				if (not previousSpace)
				{
					id += '-';
					previousSpace = true;
				}
			}
			else
			{
				id += c;
				previousSpace = false;
			}
		}

		id.trimRight('-');
	}


	void Component::selectCellAllPanels(uint x, uint y)
	{
		pOnSelectCellAllPanels(x, y);
	}





} // namespace OutputViewer
} // namespace Window
} // namespace Antares

