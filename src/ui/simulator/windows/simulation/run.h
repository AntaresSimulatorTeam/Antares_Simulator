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
#ifndef __ANTARES_APPLICATION_WINDOWS_SIMULATION_RUN_H__
# define __ANTARES_APPLICATION_WINDOWS_SIMULATION_RUN_H__

# include <antares/wx-wrapper.h>
# include <yuni/thread/thread.h>
# include <antares/study.h>

# include <wx/dialog.h>
# include <wx/sizer.h>
# include <wx/checkbox.h>
# include <wx/choice.h>
# include <wx/stattext.h>
# include <wx/timer.h>
# include "../../toolbox/components/button.h"



namespace Antares
{
namespace Window
{
namespace Simulation
{


	class Run final : public wxDialog
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param parent     The parent windows
		** \param preproOnly Set the 'ts-generator only' mode by default
		*/
		explicit Run(wxWindow* parent, bool preproOnly = false);
		//! Destructor
		virtual ~Run();
		//@}


		/*!
		** \brief Update all visual components and internal variables about the resources status
		*/
		void estimateMemoryUsage();


	private:
		void gridAppend(wxFlexGridSizer& sizer, const wxString& title, const wxString& key, wxWindow* value, bool bold = false);
		void gridAppend(wxFlexGridSizer& sizer, const wxString& title, const wxString& key, const wxString& value);
		void gridAppend(wxFlexGridSizer& sizer, wxWindow* key, wxWindow* value);
		void gridAppend(wxFlexGridSizer& sizer, wxWindow* key, wxSizer* value);
		void gridAppend(wxFlexGridSizer& sizer, const wxString& key, wxWindow* value);
		void gridAppend(wxFlexGridSizer& sizer, const wxString& key, const wxString& value);

		//! Event: Cancel the operation
		void onCancel(void*);
		//! Event: Run the simulation for real
		void onRun(void*);

		void evtOnPreprocessorsOnlyClick(wxCommandEvent& evt);

		/*!
		** \brief Prepare the popup menu for displaying the list of all modes
		*/
		void prepareMenuSolverMode(Antares::Component::Button&, wxMenu& menu, void*);

		/*!
		** \brief Menu item event
		*/
		void onSelectMode(wxCommandEvent& evt);

		/*!
		** \brief Create a temporary file where the comments of the simulation will be written
		** \return An absolute filename
		*/
		bool createCommentsFile(YString& filename) const;

		void updateMonteCarloYears();

		void updateNbCores();

		/*!
		** \brief Check for low memory resources and warn the user about it
		**
		** \return 1: Abort. -1: The user has been notified but wishes to continue. 0 nothing
		*/
		int checkForLowResources();

		//! Mouse move
		void onInternalMotion(wxMouseEvent&);

	private:
		//! The main control parent
		wxWindow* pBigDaddy;
		//! Solver mode
		Antares::Component::Button* pBtnMode;
		wxCheckBox* pOrtoolsCheckBox;

		wxTextCtrl* pSimulationName;
		wxTextCtrl* pSimulationComments;
		wxCheckBox* pIgnoreWarnings;
		wxStaticText* pNbCores;
		wxStaticText* pTitleSimCores;
		wxStaticText* pLblEstimation;
		wxStaticText* pLblEstimationAvailable;
		wxStaticText* pLblDiskEstimation;
		wxStaticText* pLblDiskEstimationAvailable;
		wxCheckBox* pPreproOnly;
		wxStaticText* pMonteCarloYears;
		wxButton* pBtnRun;
		wxSizerItem *pOptionSpacer;

		wxTimer* pTimer;
		bool pWarnAboutMemoryLimit;
		bool pWarnAboutDiskLimit;

		bool pAlreadyWarnedNoMCYear;
		uint pFeatureIndex;
		std::map<long, uint> pMappingSolverMode;

		Yuni::Thread::IThread::Ptr pThread;

	}; // class Run





} // namespace Simulation
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_SIMULATION_RUN_H__
