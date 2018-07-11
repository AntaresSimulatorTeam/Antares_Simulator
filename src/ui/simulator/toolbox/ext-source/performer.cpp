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

#include <yuni/yuni.h>
#include "performer.h"

#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../../application/study.h"
#include "../../application/main/main.h"
#include "../../toolbox/components/map/component.h"
#include "../../toolbox/components/wizardheader.h"
#include "../../windows/inspector.h"
#include <ui/common/lock.h>

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/statline.h>
#include "action-panel.h"
#include "window.h"

using namespace Yuni;



namespace Antares
{
namespace Window
{


	class MessageFlusherTimer final : public wxTimer
	{
	public:
		MessageFlusherTimer(PerformerDialog& form) :
			wxTimer(),
			pForm(form)
		{
		}

		virtual ~MessageFlusherTimer()
		{
		}

		virtual void Notify()
		{
			pForm.updateGUI();
		}

	private:
		//! Text to display
		PerformerDialog& pForm;

	}; // class MessageFlusherTimer



	class ThreadUpdater final : public Yuni::Thread::IThread
	{
	public:
		typedef Antares::Action::IAction::Ptr  IActionPtr;
		typedef Antares::Action::Context::Ptr  ContextPtr;

	public:
		ThreadUpdater(PerformerDialog& form, ContextPtr& context, IActionPtr& action) :
			pForm(form),
			pRootAction(action),
			pContext(context),
			pFormAlreadyClosed(false)
		{
		}

		virtual ~ThreadUpdater()
		{
		}


	protected:
		virtual bool onExecute()
		{
			// Perform the copy/paste
			performCopyPaste();
			// Close the window
			if (!pFormAlreadyClosed)
				Dispatcher::GUI::Post(&pForm, &PerformerDialog::askForClosingTheWindow);
			// The thread can be stopped
			return false;
		}


		void performCopyPaste()
		{
			if (prepare(pRootAction))
			{
				if (shouldAbort())
					return;

				// Notify that we have brought modifications
				Dispatcher::GUI::Post(&pForm, &PerformerDialog::notifyHasBeenModified);

				uint stopInterval = 4;
				for (uint i = 0; i != pList.size(); ++i)
				{
					pList[i]->perform(*pContext, false);

					// Checking from time to time if we have to abort
					if (!(--stopInterval))
					{
						stopInterval = 4;
						if (shouldAbort())
							return;
					}

					if (!(i % 4))
						pForm.notifyProgression((int) i, (int) pList.size());

				} // all action
			}
		}


		bool prepare(Antares::Action::IAction::Ptr action)
		{
			if (action->prepareStack(pList))
			{
				logs.debug() << "[study-action] Got " << pList.size() << " actions to execute";
				if (!pList.empty())
					return true;
			}
			else
			{
				pFormAlreadyClosed = true;
				pForm.closeThenOpenActionsDialog();
			}
			return false;
		}

	private:
		//! Form
		PerformerDialog& pForm;
		//! The root action
		IActionPtr pRootAction;
		//! Context
		ContextPtr pContext;
		//! List of actions
		Antares::Action::IAction::Vector pList;
		//! Form already closed
		bool pFormAlreadyClosed;
		//!
		bool pHasBeenModified;

	}; // class ThreadUpdater











	PerformerDialog::PerformerDialog(wxWindow* parent, const Antares::Action::Context::Ptr& context,
			const Antares::Action::IAction::Ptr& root) :
		wxDialog(parent, wxID_ANY, wxT("Import"), wxDefaultPosition, wxDefaultSize),
		pHasBeenModified(false),
		pContext(context),
		pActions(root),
		pGauge(nullptr),
		pBtnCancel(nullptr),
		pLblMessage(nullptr),
		pTimer(nullptr),
		pThread(nullptr)
	{
		// IO statistics
		Statistics::Reset();

		// Status bar
		{
			// Reference to the main form
			Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
			mainFrm.SetStatusText(wxT("  Pasting..."));

			mainFrm.beginUpdateLogs();
		}

		wxColour defaultBgColor = GetBackgroundColour();
		SetBackgroundColour(wxColour(255, 255, 255));

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		wxWindow* header = Toolbox::Components::WizardHeader::Create(this,
			wxT("Pasting..."), "images/32x32/paste.png", wxEmptyString, 500, false);
		header->SetBackgroundColour(GetBackgroundColour());
		sizer->Add(header, 0, wxALL|wxEXPAND);

		sizer->AddSpacer(5);

		pContext->updateMessageUI.bind(this, &PerformerDialog::delegateUserInfo);

		{
			pLblMessage = Component::CreateLabel(this, wxEmptyString);
			wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
			hz->AddSpacer(20);
			hz->Add(pLblMessage, 0, wxALL|wxALIGN_CENTER_VERTICAL);
			hz->AddSpacer(20);
			sizer->Add(hz, 0, wxALL|wxEXPAND);
			sizer->AddSpacer(2);
		}

		{
			pGauge = new Component::ProgressBar(this);
			wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
			hz->Add(20, 1);
			hz->Add(pGauge, 1, wxALL|wxEXPAND);
			hz->Add(20, 1);
			sizer->Add(hz, 0, wxALL|wxEXPAND);
			sizer->AddSpacer(35);
		}

		Component::Panel* panel = new Component::Panel(this);
		wxBoxSizer* sizerBar = new wxBoxSizer(wxHORIZONTAL);
		sizerBar->AddStretchSpacer();
		panel->SetSizer(sizerBar);
		panel->SetBackgroundColour(defaultBgColor);

		// Button Close
		{
			pBtnCancel = Component::CreateButton(panel, wxT(" Cancel "), this, &PerformerDialog::onCancel);
			sizerBar->Add(pBtnCancel, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE_VERTICAL|wxALL, 8);

			sizerBar->Add(15, 5);
			pBtnCancel->SetDefault();
			pBtnCancel->SetFocus();
		}

		sizer->Add(new wxStaticLine(this), 0, wxALL|wxEXPAND);
		sizer->Add(panel, 0, wxALL|wxEXPAND);


		sizer->Layout();
		SetSizer(sizer);
		sizer->Fit(this);

		wxSize p = GetSize();
		if (p.GetWidth() < 430)
			p.SetWidth(430);
		SetSize(p);

		Centre(wxBOTH);

		// Block all matrices
		OnStudyBeginUpdate();

		Refresh();

		// Lock the GUI
		GUIBeginUpdate();

		// Start the process
		pThread = new ThreadUpdater(*this, pContext, pActions);
		pThread->start();

		pTimer = new MessageFlusherTimer(*this);
		pTimer->Start(200);
	}


	PerformerDialog::~PerformerDialog()
	{
		// assert
		assert(pContext);

		delete pTimer;
		if (pThread)
		{
			pThread->stop();
			delete pThread;
		}

		// IO statistics
		Statistics::DumpToLogs();
	}


	void PerformerDialog::closeThenOpenActionsDialog()
	{
		Enable(false);
		Dispatcher::GUI::Post(this, &PerformerDialog::closeWindow);
		auto* form = new ApplyActionsDialog(nullptr, pContext, pActions);
		Dispatcher::GUI::ShowModal(form);
	}



	void PerformerDialog::onCancel(void*)
	{
		if (pThread)
		{
			if (pTimer)
				pTimer->Stop();
			pLblMessage->SetLabel(wxT("Aborting..."));
			// ASync close
			if (pBtnCancel)
				pBtnCancel->Enable(false);
			Enable(false);
			pThread->gracefulStop();
		}
	}


	/*
	void PerformerDialog::executeNext()
	{
		if (pIndex >= pList.size())
		{
			// This event should never happen...
			pShouldStop = true;
			return;
		}

		pList[pIndex]->perform(*pContext, false);
		pHasBeenModified = true;
		++pIndex;

		if (pIndex == pList.size() || pShouldStop)
		{
			pGauge->value(100.);
			if (pBtnCancel)
				pBtnCancel->Enable(false);

			Enable(false);
			Dispatcher::GUI::Post(this, &PerformerDialog::askForClosingTheWindow);
		}
		else
		{
			// All copy/paste operations must be performed in the main thread,
			// since the GUI uses the same study.
			// Those operations must be delayed at least from time to time,
			// to let the GUI refresh its components.
			static uint haveToDelay = 3; // to force a refresh at the begining
			if (!(--haveToDelay))
			{
				// The operation will be delayed. The delay seems stupid,
				// but it will be enough to let the gui refresh its components
				haveToDelay = 6;

				// Updating the Progression, only when the GUI will be able to do so
				if (pIndex >= pList.size())
					pGauge->value(100.);
				else
					pGauge->value((double)(100. * pIndex) / pList.size());

				pLblMessage->SetLabel(wxStringFromUTF8(pNextMessage));

				// callback to the next operation
				Bind<void ()> callback;
				callback.bind(this, &PerformerDialog::executeNext);
				Dispatcher::GUI::Post(callback, 2/ms/);
			}
			else
				executeNext();
		}
	}
	*/

	void PerformerDialog::delegateUserInfo(const Yuni::String& text)
	{
		pMutex.lock();
		pNextMessage = text;
		pMutex.unlock();
		pGUINeedRefresh = 1;
	}


	void PerformerDialog::notifyProgression(int progress, int max)
	{
		pProgression = progress;
		pProgressionMax = max;
		pGUINeedRefresh = 1;
	}



	void PerformerDialog::askForClosingTheWindow()
	{
		if (pTimer)
			pTimer->Stop();
		Enable(false);
		if (pLblMessage)
			pLblMessage->SetLabel(wxT("Updating the study..."));
		pGauge->value(100.);
		if (pBtnCancel)
			pBtnCancel->Enable(false);

		Refresh();

		Dispatcher::GUI::Post(this, &PerformerDialog::closeWindow);
	}


	void PerformerDialog::updateGUI()
	{
		if (pGUINeedRefresh)
		{
			pGauge->value((100. * pProgression) / pProgressionMax);
			pMutex.lock();
			pLblMessage->SetLabel(wxStringFromUTF8(pNextMessage));
			pMutex.unlock();
			pGauge->Refresh();
			pLblMessage->Refresh();
		}
	}


	void PerformerDialog::notifyHasBeenModified()
	{
		pHasBeenModified = true;
	}


	void PerformerDialog::closeWindow()
	{
		if (pThread)
			pThread->wait();

		// Reference to the main fomr
		auto& mainFrm = *Forms::ApplWnd::Instance();
		// Reconnecting the log mecanism
		mainFrm.endUpdateLogs();

		if (!(!pContext))
		{
			// Alias to the current study
			auto& study = pContext->study;

			// remove the delegate for code safety
			pContext->updateMessageUI.unbind();

			if (pHasBeenModified)
			{
				// Unselect from the inspector
				Antares::Window::Inspector::Unselect();
				// ReAdjust all interconnections
				study.areas.fixOrientationForAllInterconnections(study.bindingConstraints);
				// reload the UI infos
				study.uiinfo->reloadAll();
				// Mark the study as modified
				MarkTheStudyAsModified();

				// updating the map
				// There is no good reason to have a nullptr pointer here but just in case...
				auto* map = mainFrm.map();
				if (map)
				{
					map->loadFromAttachedStudy();
					map->selectOnly(pContext->autoselectAreas, pContext->autoselectLinks);
				}

				// Reload runtime data
				auto currentStudy = Data::Study::Current::Get();
				if (currentStudy != Data::Study::Ptr(&study)) // TODO use smartptr for Action::Context::study
					currentStudy->uiinfo->reloadAll();
			}
		}

		// Block all matrices
		OnStudyEndUpdate();
		// Status bar
		mainFrm.resetDefaultStatusBarText();

		GUIEndUpdate();

		Dispatcher::GUI::Close(this);
	}






} // namespace Window
} // namespace Antares
