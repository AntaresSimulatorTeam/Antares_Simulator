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
#ifndef __ANTARES_APPLICATION_EXT_SOURCE_PERFORMER_H__
# define __ANTARES_APPLICATION_EXT_SOURCE_PERFORMER_H__

# include <antares/wx-wrapper.h>
# include <antares/study.h>
# include <yuni/thread/thread.h>
# include <wx/timer.h>
# include <wx/dialog.h>
# include <antares/study/action/action.h>
# include "../components/progressbar.h"



namespace Antares
{
namespace Window
{


	class PerformerDialog :public wxDialog
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		PerformerDialog(wxWindow* parent, const Antares::Action::Context::Ptr& context,
			const Antares::Action::IAction::Ptr& root);

		//! Destructor
		virtual ~PerformerDialog();
		//@}

		/*!
		** \brief Open the actions dialog
		*/
		void closeThenOpenActionsDialog();

		/*!
		** \brief Close the window
		*/
		void askForClosingTheWindow();

		void notifyHasBeenModified();

		void notifyProgression(int progress, int max);

		void updateGUI();

	private:
		//! Event: The user asked to cancel the operation
		void onCancel(void*);

		void delegateUserInfo(const Yuni::String& text);

		void closeWindow();

	private:
		//! Flag to know if the study has been modified
		bool pHasBeenModified;

		//! The target study
		Antares::Action::Context::Ptr pContext;
		//! The tree of the actions to perform
		Antares::Action::IAction::Ptr pActions;
		//! Gauge
		Component::ProgressBar* pGauge;
		//! Cancel button
		wxButton* pBtnCancel;

		wxStaticText* pLblMessage;

		Yuni::String pNextMessage;

		wxTimer* pTimer;

		Yuni::Thread::IThread* pThread;
		Yuni::Mutex pMutex;
		Yuni::Atomic::Int<> pGUINeedRefresh;
		Yuni::Atomic::Int<> pProgression;
		Yuni::Atomic::Int<> pProgressionMax;

	}; // class PerformerDialog






} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_EXT_SOURCE_PERFORMER_H__
