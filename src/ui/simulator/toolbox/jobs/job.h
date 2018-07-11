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
#ifndef __ANTARES_TOOLBOX_JOB_H__
# define __ANTARES_TOOLBOX_JOB_H__

# include <antares/wx-wrapper.h>
# include <antares/logs.h>
# include <yuni/core/slist/slist.h>
# include <yuni/thread/thread.h>
# include <yuni/thread/timer.h>

# include <wx/dialog.h>
# include <wx/frame.h>
# include <wx/animate.h>
# include <wx/stattext.h>
# include <wx/timer.h>
# include <wx/stattext.h>
# include <wx/regex.h>
# include "../components/progressbar.h"
# include "../create.h"
# include <ui/common/component/panel.h>

# include <list>



namespace Antares
{
namespace Toolbox
{
namespace Jobs
{

	// Forward declarations
	class JobThread;
	class TimerElapsedTime;
	class TimerRemainingTime;
	class PartList;



	class Job : public wxDialog, public Yuni::IEventObserver<Job>
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		*/
		Job(const wxString& title, const wxString& subTitle, const char* icon);
		/*!
		** \brief Destructor
		*/
		virtual ~Job();
		//@}

		//! \name Progression
		//@{
		bool displayProgression() const {return pDisplayProgression;}
		void displayProgression(const bool v) {pDisplayProgression = v;}
		//@}

		//! \name Cancelation
		//@{
		bool canCancel() const {return pCanCancel;}
		void canCancel(const bool v) {pCanCancel = v;}
		//@}

		//! \name Result
		//@{
		bool result() const {return pResult;}
		void result(const bool v) {pResult = v;}
		//@}

		//! \name Execution flow
		//@{

		/*!
		** \brief Get if the job is running
		*/
		bool isRunning() const {return !(!pJobIsRunning);}

		/*!
		** \brief Run the job
		*/
		bool run();

		/*!
		** \brief Stop the job gracefully
		*/
		void stop();
		//@}


	protected:
		/*!
		** \brief Execute the real task
		*/
		virtual bool executeTask() = 0; // Please override

		/*!
		** \brief Update the progress value
		**
		** \param progress The progression in percent
		*/
		void updateTheProgressValue(double progress);

		/*!
		** \brief Update the message displayed for the user
		** \param msg The new message
		** \param mustBeInterpreted True if the message must be interpreted (format: `[antares][notice] msg`)
		*/
		void updateTheMessage(const wxString& msg, bool mustBeInterpreted = true);


		/*!
		** \brief Event: Before the thread is launched
		**
		** This method will be fired before the thread is launched when
		** the job is asked to run.
		** \see run()
		*/
		virtual void onBeforeRunning() {}

		/*!
		** \brief Event: Ask if the job is allow to run
		**
		** This method is fired before running the job. It is actually thefirst check
		** done by the method `run()`
		** \see run()
		*/
		virtual bool onRunQuery() {return true;}

		/*!
		** \brief Event: The user ask to cancel the operation
		*/
		virtual void onCancel() {/* do nothing */}

		/*!
		** \brief Gracefully Stop all Gui components
		*/
		void stopAllGuiComponents();

		/*!
		** \brief Dispatch a log entry, grabbed from from the standard output
		*/
		void manageLogLevelMessage(enum LogLevel lvl, const wxString& msg);


	protected:
		/*!
		** \brief Event: The user asks to cancel the operation
		*/
		void onCancel(void*);

		void evtOnClose(wxCloseEvent& evt);

		void onLogMessage(int, const Yuni::String& message);

	protected:
		/*!
		** \brief Create the main panel, which contains all important controls
		*/
		wxSizer* createMainPnl(wxWindow* parent);

		/*!
		** \brief Create the bottom panel, which contains buttons to cancel and to continue
		*/
		wxSizer* createPnlButtons(wxWindow* parent);

		wxSizer* createPnlLoading(wxWindow* parent);

		/*!
		** \brief Display (or not) the gauge
		*/
		void displayGauge(const bool visible = true);

		void displayMessage(const wxString& line);


		void onUIUpdateProgression(uint value);
		void onUIUpdateMessage();
		void onUIUpdateLabelErrors();

		void onUIUpdateElapsedTime(const Yuni::String& text);
		void onUIUpdateRemainingTime(const Yuni::String& text);

		void evtOnInit(wxInitDialogEvent& event);

		void deleteAllThreads();

		void recomputeWindowSize();

		void disableAllComponents();

	private:
		//! Title of the window
		const wxString& pTitle;
		const wxString& pSubTitle;
		const char* pIcon;
		bool pDisplayProgression;
		bool pCanCancel;
		bool pResult;
		long pMemoryFootprint;

		//! Support for the gauge
		wxSizer* pGaugeSizer;
		wxSizer* pGaugeParentSizer;
		wxSizer* pRemainingSizer;
		Component::ProgressBar* pGauge;
		wxSizer* pCancelSizer;
		//! Text
		wxStaticText* pProgrText;
		wxStaticText* pProgrSubText;
		wxStaticText* pElapsedTimeText;
		wxStaticText* pRemainingTimeText;
		wxStaticText* pReadWriteStats;
		//! List of warning
		Yuni::String::ListPtr pWarningList;
		Yuni::String::ListPtr pErrorList;

		Yuni::Atomic::Int<> pJobIsRunning;
		Yuni::Atomic::Int<> pGUICanUpdate;

		// Event table
		DECLARE_EVENT_TABLE()

	private:
		//! Timer for refreshing the `elapsed time` since the launch of the job
		TimerElapsedTime*   pTimerElapsedTime;
		//! Tier for refreshing the estimated`remaining time` to complete the job
		// (only if a progress is available)
		TimerRemainingTime* pTimerRemainingTime;

		mutable Yuni::Thread::IThread::Ptr pThread;
		wxAnimationCtrl* pAnim;
		wxStaticText* pLblErrors;
		wxString pLastNoticeMessage;
		wxButton* pBtnCancel;

		wxRegEx pLogRegex;
		wxString pLogEntryLevelTmp;
		wxString pLogEntryTmp;

		bool pCatchLogEvents;

		wxString pMessage;
		wxString pSubMessage;

		Yuni::Mutex pMutex;
		Yuni::Mutex pErrorMutex;

		PartList* pPartList;

		wxColour pDefaultBGColor;
		Component::Panel* pPanelButtons;
		Component::Panel* pPanelHeader;

		wxWindow* pWndCancelOperation;

		//! Timer for updating labels
		wxTimer* pTimerMessageUpdater;
		wxTimer* pTimerReadWriteStats;

		// Our friends !
		friend class JobThread;
		friend class TimerElapsedTime;
		friend class TimerRemainingTime;

	}; // class Job





} // namespace Jobs
} // namespace Toolbox
} // namespace Antares


#endif // __ANTARES_TOOLBOX_JOB_H__
