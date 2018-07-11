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
#include <antares/wx-wrapper.h>
#include <yuni/thread/thread.h>
#include <yuni/core/math.h>

#include "job.h"
#ifndef YUNI_OS_WINDOWS
# include <unistd.h>
#endif
#include <iostream>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/generic/dirctrlg.h>
#include <wx/font.h>
#include <wx/stdpaths.h>
#include <wx/font.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/frame.h>
#include <wx/statline.h>

#include "../components/wizardheader.h"
#include "../resources.h"
#include "../../application/main.h"
#include "../../windows/message.h"
#include "../components/refresh.h"
#include <antares/study/progression/progression.h>
#include <antares/io/statistics.h>


#define PROGRESSBAR_MAX_RANGE    1000000
#define PROGRESSBAR_MAX_RANGE_F  1000000.0

using namespace Yuni;







namespace Antares
{
namespace Toolbox
{
namespace Jobs
{


namespace // anonymous
{


	typedef std::pair<wxSizer*, wxStaticText*>  StdPair;


	enum JobIDS
	{
		idMSourceFolder = 1000,
		idMTargetFolder,
		idMLogs,
	};


	StdPair CreateStdInfo(wxWindow* parent, const wxString& defText)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		// Label
		wxStaticText* lbl = Component::CreateLabel(parent, defText, false, false, -1);
		// Assigning its new color
		const wxColour c = parent->GetBackgroundColour();
		const wxColour newC(
			(unsigned char) Math::MinMax<int>(c.Red()   - 110, 0, 255),
			(unsigned char) Math::MinMax<int>(c.Green() - 110, 0, 255),
			(unsigned char) Math::MinMax<int>(c.Blue()  - 110, 0, 255) );
		lbl->SetForegroundColour(newC);

		// Sizer
		auto* sw = new wxBoxSizer(wxHORIZONTAL);
		sw->Add(20, 1);
		sw->Add(lbl, 1, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL);

		return StdPair(sw, lbl);
	}


} // anonymous namespace




	BEGIN_EVENT_TABLE(Job, wxDialog)
		EVT_CLOSE(Job::evtOnClose)
		EVT_INIT_DIALOG(Job::evtOnInit)
	END_EVENT_TABLE()




	class Part final
	{
	public:
		Part() :
			weight(),
			value()
		{}

		uint weight;
		uint value;
	};



	class PartList final : public Yuni::Policy::ObjectLevelLockableNotRecursive<PartList>
	{
	public:
		typedef Yuni::Policy::ObjectLevelLockableNotRecursive<PartList> ThreadingPolicy;

		static Solver::Progression::Section CStrToSection(const AnyString& section)
		{
			if (section == "mc")
				return Solver::Progression::sectYear;
			if (section == "output")
				return Solver::Progression::sectOutput;
			if (section == "thermal")
				return Solver::Progression::sectTSGThermal;
			if (section == "wind")
				return Solver::Progression::sectTSGWind;
			if (section == "hydro")
				return Solver::Progression::sectTSGHydro;
			if (section == "load")
				return Solver::Progression::sectTSGLoad;
			if (section == "solar")
				return Solver::Progression::sectTSGSolar;
			if (section == "import timeseries")
				return Solver::Progression::sectImportTS;
			return Solver::Progression::sectYear;
		}


	public:
		PartList() :
			pGlobalValue(0),
			pMaxValue(1),
			pCurrentTaskPercent(0.)
		{}

		double set(uint year, const AnyString& section, double value)
		{
			const Solver::Progression::Section s = CStrToSection(section);

			// locking
			ThreadingPolicy::MutexLocker locker(*this);
			Part& part = pParts[year][s];

			//
			pGlobalValue -= (uint64) part.value;
			pCurrentTaskPercent = value;
			if (pCurrentTaskPercent >= 100.)
				pCurrentTaskPercent = 0.;
			part.value    = (uint)((double) part.weight * (value / 100.));
			pGlobalValue += (uint64) part.value;

			// return the percent
			return (pGlobalValue * 100.) / pMaxValue;
		}


		void progression(double& total, double& currentTask) const
		{
			ThreadingPolicy::MutexLocker locker(*this);
			total = (pGlobalValue * 100.) / pMaxValue;
			currentTask = pCurrentTaskPercent;
		}

		void loadFromFile(const AnyString& filename)
		{
			ThreadingPolicy::MutexLocker locker(*this);
			pMaxValue = 0;
			IO::File::Stream file;
			if (file.open(filename))
			{
				CString<255, false> line;
				String str;
				String::Size p;
				uint year;
				while (file.readline(line))
				{
					line.trimRight("\n\r ");
					p = line.find(' ');

					if (p >= line.size() - 1)
						continue;

					str.assign(line.c_str() + p + 1, line.size() - p - 1);
					line.resize(p);
					if (line == "post")
						year = (uint) -1;
					else
					{
						if (!line.to<uint>(year))
							continue;
					}
					addWL(year, str);
				}
			}

			// Avoid division by 0
			if (!pMaxValue)
				pMaxValue = (uint64) -1;
		}

	protected:
		template<class StringT>
		void addWL(uint year, const StringT& section)
		{
			Solver::Progression::Section s = CStrToSection(section);
			Part& part = pParts[year][s];
			switch (s)
			{
				case Solver::Progression::sectYear:
					part.weight = 100000;
					break;
				case Solver::Progression::sectImportTS:
					part.weight = 40000;
					break;
				default:
					part.weight = 10000;
			}
			pMaxValue += part.weight;
		}


	private:
		//! The curren progression
		uint64 pGlobalValue;
		//! The maximum reachable value
		uint64 pMaxValue;
		//!
		double pCurrentTaskPercent;
		//! All parts
		std::map<uint, std::map<Solver::Progression::Section, Part> > pParts;
	};



	class JobThread final: public Yuni::Thread::IThread
	{
	public:
		JobThread(Job& job) :
			pJob(job)
		{
		}

		virtual ~JobThread()
		{
			// Required for code robustness (to avoid corruption of the vtable)
			stop();
		}


	protected:
		virtual bool onExecute() override
		{
			if (&pJob)
			{
				// waite while the task is being executed
				while (not pJob.executeTask())
					suspend(300); // arbitrary value

				pJob.pMutex.lock();
				pJob.pGUICanUpdate = 0;
				pJob.pMutex.unlock();
				suspend(100);

				// Notify to the interface that the task is complete
				Dispatcher::GUI::Post(&pJob, &Job::stop);

				// Wait as long as necessary
				//suspend(20000);
			}
			return false;
		}

		virtual void onStop() override
		{
			pJob.pGUICanUpdate = 0;
		}

		virtual void onKill() override
		{
			logs.warning() << " [thread] the thread has been killed";
		}

	private:
		//! Reference to the job
		Job& pJob;

	}; // class JobThread



	class TimerElapsedTime final : public Yuni::Thread::Timer
	{
	public:
		TimerElapsedTime(Job& j, PartList& p)
			:Yuni::Thread::Timer(850), pElapsedTime(0), pPercent(0),
			pUseProgression(false),
			pJob(j),
			pParts(p)
		{
			pStartTime = wxDateTime::Now();
		}

		virtual ~TimerElapsedTime()
		{
			// for code robustness and to prevent against vtable corruption
			stop();
		}

		void progression(bool v)
		{
			pMutex.lock();
			pUseProgression = v;
			pMutex.unlock();
		}

		void retrieveProperties(uint& elapsedTime, double& percent)
		{
			pMutex.lock();
			elapsedTime = pElapsedTime;
			percent = pPercent;
			pMutex.unlock();
		}

	protected:
		virtual bool onInterval(uint)
		{
			// On re-entrant operations
			wxString s(wxT("Elapsed time: "));
			const wxTimeSpan timeSpan = wxDateTime::Now() - pStartTime;
			s += timeSpan.Format();
			pElapsedTime = (uint) timeSpan.GetSeconds().GetValue();

			// Locking
			pMutex.lock();

			if (pUseProgression)
			{
				// Progression
				double total;
				double currentTask;
				pParts.progression(total, currentTask);
				pPercent = total;
				pMutex.unlock();

				if (currentTask > 0.)
				{
					s << wxT("  (sub task ") << wxString::Format(wxT("%.2f"), currentTask)
						<< wxT("% complete)");
				}
			}
			else
				pMutex.unlock();

			// Sending event
			Bind<void ()>  callback;
			String ys;
			wxStringToString(s, ys);
			callback.bind(&pJob, &Job::onUIUpdateElapsedTime, ys);
			Dispatcher::GUI::Post(callback);

			return true;
		}

	private:
		uint pElapsedTime;
		double pPercent;
		bool pUseProgression;
		Job& pJob;
		wxDateTime pStartTime;
		Yuni::Mutex pMutex;
		PartList& pParts;

	}; // class TimerElapsedTime



	class TimerRemainingTime final : public Yuni::Thread::Timer
	{
	public:
		TimerRemainingTime(Job& j, TimerElapsedTime* elapsedTime)
			:Yuni::Thread::Timer(5500),
			pJob(j),
			pTimerElapsedTime(elapsedTime)
		{
		}

		virtual ~TimerRemainingTime()
		{
			// for code robustness and to prevent against vtable corruption
			stop();
		}


	protected:
		virtual bool onInterval(uint)
		{
			CString<120,false> tmp;
			tmp = "Remaining time: ";
			uint elapsedTime = 0;
			double percent = 0.;
			pTimerElapsedTime->retrieveProperties(elapsedTime, percent);


			if (percent < 0.10 or elapsedTime < 10)
			{
				// We may have unexpected results
				tmp << "waiting for more information";
			}
			else
			{
				const double remaining = ((100. * elapsedTime / percent) - elapsedTime);
				if (remaining < 60.)
				{
					if (remaining < 30.)
						tmp << "less than 30 seconds";
					else
						tmp << "less than one minute";
				}
				else
				{
					if (percent > 0.3)
						tmp << "around ";
					else
						tmp << "probably around ";
					if (remaining < 3600.)
					{
						const uint min = (int)(remaining / 60.);
						tmp << min << ((min > 1) ? " minutes" : " minute");
					}
					else
					{
						if (remaining < 7200.)
						{
							const uint min = (uint)((remaining - 3600.) / 60.);
							tmp << "1 hour and " << min << ((min > 1) ? " minutes" : " minute");
						}
						else
						{
							uint min = (uint) Math::Round(remaining / 60.);
							uint hr  = (uint) Math::Trunc(min / 60.);
							if (hr > 23)
							{
								const uint days = (uint) Math::Trunc(hr / 24.);
								hr -= 24 * days;
								tmp << days << ((days > 1) ? " days" : " day");
								if (hr)
								{
									if (hr == 1)
										tmp << " and one hour";
									else
										tmp << " and " << hr << " hours";
								}
							}
							else
							{
								min -= hr * 60;
								tmp << hr << " hours and ";
								tmp << min << ((min > 1) ? " minutes" : " minute");
							}
						}
					}
				}
			}

			// Sending event
			Bind<void ()>  callback;
			callback.bind(&pJob, &Job::onUIUpdateRemainingTime, String(tmp));
			Dispatcher::GUI::Post(callback);

			return true;
		}

	private:
		Job& pJob;
		TimerElapsedTime* pTimerElapsedTime;

	}; // class TimerRemainingTime


	class MessageFlusherTimer final : public wxTimer
	{
	public:
		MessageFlusherTimer(const wxString& messageBuffer, wxStaticText* label, Yuni::Mutex& mutex) :
			wxTimer(),
			pMessageBuffer(messageBuffer),
			pLabel(label),
			pMutex(mutex)
		{
			assert(pLabel != NULL);
		}

		virtual ~MessageFlusherTimer()
		{
		}

		virtual void Notify()
		{
			{
				Yuni::MutexLocker locker(pMutex);
				// We use .c_str() here to force the copy of the string
				if (pStrCopy == pMessageBuffer)
					return;
				pStrCopy = pMessageBuffer.c_str();
			}
			pLabel->SetLabel(pStrCopy);
		}

	private:
		//! Text to display
		const wxString& pMessageBuffer;
		//! Copy
		wxString pStrCopy;
		//! GUI label
		wxStaticText*  pLabel;
		//! Mutex
		Yuni::Mutex& pMutex;

	}; // class MessageFlusherTimer



	class ReadWriteStatsFlusherTimer final : public wxTimer
	{
	public:
		ReadWriteStatsFlusherTimer(wxStaticText* label) :
			wxTimer(),
			pLabel(label)
		{
			assert(pLabel != NULL);
		}

		virtual ~ReadWriteStatsFlusherTimer()
		{
		}

		virtual void Notify()
		{
			uint64 r = Statistics::ReadFromDisk();
			uint64 w = Statistics::WrittenToDisk();
			bool hasR = (r >= 1024);
			bool hasW = (w >= 1024);

			if (hasR or hasW)
			{
				pText.clear();
				ShortString32 tmp;

				if (hasR)
				{
					pText << wxT("Read: ");
					if (r > 1024 * 1024)
					{
						pText << (r / (1024 * 1024));
					}
					else
					{
						double v = Math::Round(r / (1024. * 1024.), 1);
						tmp = v;
						tmp.trimRight('0');
						if (tmp.last() == '.')
							tmp.removeLast();
						pText << wxStringFromUTF8(tmp);
					}
					if (hasW)
						pText << wxT(" Mo,  ");
					else
						pText << wxT(" Mo");
				}
				if (hasW)
				{
					pText << wxT("Written: ");
					if (w > 1024 * 1024)
					{
						pText << (w / (1024 * 1024));
					}
					else
					{
						double v = Math::Round(w / (1024. * 1024.), 1);
						tmp = v;
						tmp.trimRight('0');
						if (tmp.last() == '.')
							tmp.removeLast();
						pText << wxStringFromUTF8(tmp);
					}
					pText << wxT(" Mo");
				}

				pLabel->SetLabel(pText);
			}
			else
				pLabel->SetLabel(wxEmptyString);
		}

	private:
		//! GUI label
		wxStaticText*  pLabel;
		wxString pText;

	}; // class MessageFlusherTimer





	Job::Job(const wxString& title, const wxString& subTitle, const char* icon) :
		wxDialog(Forms::ApplWnd::Instance(), wxID_ANY, title, wxDefaultPosition,
			wxSize(530, 100), wxCAPTION|wxCLIP_CHILDREN),
		pTitle(title),
		pSubTitle(subTitle),
		pIcon(icon),
		pDisplayProgression(false),
		pCanCancel(false), pResult(false),
		pGaugeSizer(nullptr),
		pGaugeParentSizer(nullptr),
		pRemainingSizer(nullptr),
		pGauge(nullptr), pCancelSizer(nullptr),
		pProgrText(nullptr),
		pProgrSubText(nullptr),
		pJobIsRunning(0),
		pGUICanUpdate(0),
		pTimerElapsedTime(nullptr),
		pTimerRemainingTime(nullptr),
		pThread(nullptr),
		pAnim(nullptr),
		pLblErrors(nullptr),
		pLogRegex(wxT("\\[([a-zA-Z0-9 :]+)\\]\\[([a-zA-Z0-9 -]+)\\](\\[[a-z0-9;]+){0,2}\\[([a-z]+)\\](\\[[a-z0-9;]+){0,2} (\\[[a-z0-9;]+){0,2}([^]*)(\\[[a-z0-9;]+){0,2}")),
		pCatchLogEvents(true),
		pWndCancelOperation(nullptr)
	{
		// Do no longer display the logs to the user
		Forms::ApplWnd::Instance()->beginUpdateLogs();

		// Retrieving the default color for a Window background
		pDefaultBGColor = GetBackgroundColour();

		pPartList = new PartList();

		// The main sizer
		wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
		// Main controls
		szMain->Add(createMainPnl(this), 0, wxALL|wxEXPAND);

		// Buttons
		szMain->Add(createPnlButtons(this), 0, wxALL|wxEXPAND);

		// Add the sizer
		SetSizer(szMain);

		// Logs: Taking the ownership on callbacks
		logs.callback.clear();
		logs.callback.connect(this, &Job::onLogMessage);


		pTimerMessageUpdater = new MessageFlusherTimer(pSubMessage, pProgrSubText, pMutex);
		if (not pTimerMessageUpdater->Start(180, wxTIMER_CONTINUOUS))
			logs.error() << "impossible to start internal timer";

		pTimerReadWriteStats = new ReadWriteStatsFlusherTimer(pReadWriteStats);
		if (not pTimerReadWriteStats->Start(1600, wxTIMER_CONTINUOUS))
			logs.error() << "impossible to start internal timer";
	}


	void Job::deleteAllThreads()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		// Note: pRemainingTime MUST be deleted defore pTimerElapsedTime
		//   since the last one is used by the first one and no check are
		//   performed
		if (pTimerRemainingTime)
		{
			TimerRemainingTime* timer = pTimerRemainingTime;
			pTimerRemainingTime = nullptr;
			timer->stop();
			delete timer;
		}
		if (pTimerElapsedTime)
		{
			TimerElapsedTime* timer = pTimerElapsedTime;
			pTimerElapsedTime = nullptr;
			timer->stop();
			delete timer;
		}

		auto thread = pThread;
		if (!(!thread))
			thread->gracefulStop();
	}


	Job::~Job()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");
		assert(!pJobIsRunning and "The job must be already stopped");

		if (pTimerReadWriteStats)
		{
			pTimerReadWriteStats->Stop();
			delete pTimerReadWriteStats;
			pTimerReadWriteStats = nullptr;
		}
		if (pTimerMessageUpdater)
		{
			pTimerMessageUpdater->Stop();
			delete pTimerMessageUpdater;
			pTimerMessageUpdater = nullptr;
		}

		// The job is no longer running
		pGUICanUpdate = 0;
		pJobIsRunning = 0;

		// Stopping Timer if not already done
		if (pAnim)
			pAnim->Stop();

		// Stopping all threads at once
		deleteAllThreads();

		// unref current thread
		pThread = nullptr;

		// Removing log callbacks
		logs.callback.clear();
		// Destroy all bounds (callback)
		destroyBoundEvents();

		// Restore the original callbacks
		Forms::ApplWnd::Instance()->connectLogCallback();

		delete pPartList;

		// Flushing logs
		pErrorMutex.lock();
		if (not pWarningList.empty())
		{
			auto end = pWarningList.cend();
			for (auto i = pWarningList.cbegin(); i != end; ++i)
				logs.warning() << *(*i);
		}

		if (not pErrorList.empty())
		{
			auto end = pErrorList.cend();
			for (auto i = pErrorList.cbegin(); i != end; ++i)
				logs.error() << *(*i);
		}
		pErrorMutex.unlock();

		// The logs can now be displayed to the user
		Forms::ApplWnd::Instance()->endUpdateLogs();
	}



	void Job::recomputeWindowSize()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		GetSizer()->Layout();
		Fit();

		wxSize p = GetSize();
		if (p.GetWidth() < 500)
			p.SetWidth(500);
		SetSize(p);
		GetSizer()->Layout();

		Dispatcher::GUI::Refresh(this);
	}


	void Job::displayGauge(const bool visible)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		if (pGaugeSizer and pGaugeParentSizer and pGUICanUpdate)
		{
			if (pTimerElapsedTime)
				pTimerElapsedTime->progression(visible);

			// Making the gauge visible (or not)
			pDisplayProgression = visible;
			pGaugeParentSizer->Show(pGaugeSizer, visible, false);
			pRemainingTimeText->Show(visible);

			// Enabling/Disabling the remaining timer
			if (pTimerRemainingTime)
			{
				if (visible)
					pTimerRemainingTime->start();
				else
					pTimerRemainingTime->stop();
			}
			// The label used to display the remaining time should be hidden
			// if the progression is not available
			if (not visible)
				pRemainingTimeText->SetLabel(wxEmptyString);

			// Rebuilding the layout
			recomputeWindowSize();
		}
	}


	void Job::onLogMessage(int level, const String& message)
	{
		if (message.empty() or message[0] == ' ')
			return;
		switch (level)
		{
			case Yuni::Logs::Verbosity::Info::level:
				manageLogLevelMessage(logInfo, wxStringFromUTF8(message));
				break;
			case Yuni::Logs::Verbosity::Notice::level:
				manageLogLevelMessage(logNotice, wxStringFromUTF8(message));
				break;
			case Yuni::Logs::Verbosity::Warning::level:
				manageLogLevelMessage(logWarning, wxStringFromUTF8(message));
				break;
			case Yuni::Logs::Verbosity::Error::level:
				manageLogLevelMessage(logError, wxStringFromUTF8(message));
				break;
			case Yuni::Logs::Verbosity::Fatal::level:
				manageLogLevelMessage(logFatal, wxStringFromUTF8(message));
				break;
			default:
				break;
		}
	}


	void Job::manageLogLevelMessage(enum LogLevel lvl, const wxString& msg)
	{
		// Avoid useless messages
		if (msg.empty() or msg[0] == ' ' or msg[0] == '[')
			return;

		switch (lvl)
		{
			case logProgress:
				{
					// Composition :
					// task <thread id> <section>, [post|year: <year>], <percent>
					// Example : 'task 0 mc, year: 0, 13.461538'
					CString<250, false> line;
					wxStringToString(msg, line);

					// A temporary string
					CString<20, false> tmp;
					CString<20, false> section;

					String::Size p = line.find(',');
					if (!p or p >= line.size() - 1)
						break;
					--p;
					String::Size p2 = line.rfind(' ', p);
					if (!p2 or p2 >= p)
						break;
					section.assign(line.c_str() + p2 + 1, p - p2);


					p += 3;
					if (!p or p >= line.size() - 1)
						break;
					p2 = line.find(',', p);
					if (p2 >= line.size() - 1)
						break;
					tmp.assign(line.c_str() + p, p2 - p);

					uint year;
					if (tmp == "post")
						year = (uint) -1;
					else
					{
						p = tmp.rfind(' ', p2);
						if (!p or p >= p2)
							break;
						++p;
						tmp.consume(p);
						if (!tmp.to(year))
							break;
					}

					p2 += 2;
					if (p2 >= line.size() - 1)
						break;
					tmp.assign(line.c_str() + p2, line.size() - p2);
					double percent;
					if (!tmp.to(percent))
						break;
					updateTheProgressValue(pPartList->set(year, section, percent));
					break;
				}
			case logInfo:
				{
					pMutex.lock();
					// A deep copy is required
					pSubMessage = msg.c_str();
					pMutex.unlock();
					break;
				}
			case logNotice:
				{
					// The event
					pMutex.lock();
					// A deep copy is required
					pMessage = msg.c_str();
					pMutex.unlock();
					Dispatcher::GUI::Post(this, &Job::onUIUpdateMessage);
					break;
				}
			case logWarning:
				{
					String* s = new String();
					wxStringToString(msg, *s);

					pErrorMutex.lock();
					pWarningList.push_back(s);
					pErrorMutex.unlock();

					Dispatcher::GUI::Post(this, &Job::onUIUpdateLabelErrors);
					break;
				}
			case logError:
				{
					String* s = new String();
					wxStringToString(msg, *s);

					pErrorMutex.lock();
					pErrorList.push_back(s);
					pErrorMutex.unlock();

					Dispatcher::GUI::Post(this, &Job::onUIUpdateLabelErrors);
					// It may not be an error (--force)
					// result(false);
					break;
				}
			case logFatal:
				{
					String* s = new String();
					wxStringToString(msg, *s);

					pErrorMutex.lock();
					pResult = false;
					pErrorList.push_back(s);
					pErrorMutex.unlock();

					Dispatcher::GUI::Post(this, &Job::onUIUpdateLabelErrors);
					break;
				}
			default:
				{
					// Do nothing
					break;
				}
		}
	}


	void Job::onUIUpdateLabelErrors()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		if (not pGUICanUpdate)
			return;
		pErrorMutex.lock();
		const uint warns = (uint) pWarningList.size();
		const uint errs  = (uint) pErrorList.size();
		pErrorMutex.unlock();

		if (!warns and !errs)
		{
			pLblErrors->SetLabel(wxEmptyString);
			return;
		}
		wxString m;
		if (errs)
		{
			switch (errs)
			{
				case 0:  break;
				case 1:  m << wxT("1 error");break;
				default: m << errs << wxT(" errors");
			}
		}
		if (warns)
		{
			if (!m.empty())
				m << wxT(", ");
			switch (warns)
			{
				case 0:  break;
				case 1:  m << wxT("1 warning");break;
				default: m << warns << wxT(" warnings");
			}
		}
		pLblErrors->SetLabel(m);
	}



	void Job::displayMessage(const wxString& line)
	{
		if (not pGUICanUpdate or line.IsEmpty()) // Aborting now
			return;

		if (pLogRegex.Matches(line))
		{
			pLogEntryLevelTmp = pLogRegex.GetMatch(line, 4);
			pLogEntryTmp = pLogRegex.GetMatch(line, 7);

			// Avoid empty or stranges logs
			if (pLogEntryTmp.IsEmpty() or pLogEntryTmp.GetChar(0) == wxT(' '))
				return;

			if (pLogEntryTmp.StartsWith(wxT(LOG_UI)))
			{
				// Special command to the interface
				if (pLogEntryTmp == wxT(LOG_UI_DISPLAY_MESSAGES_ON))
				{
					pCatchLogEvents = true;
					return;
				}
				if (pLogEntryTmp == wxT(LOG_UI_DISPLAY_MESSAGES_OFF))
				{
					pCatchLogEvents = false;
					return;
				}
				if (pLogEntryTmp == wxT(LOG_UI_HIDE_PROGRESSBAR))
				{
					displayGauge(false);
					return;
				}
				if (pLogEntryTmp.StartsWith(wxT(LOG_UI_PROGRESSION_MAP)))
				{
					String s;
					wxStringToString(pLogEntryTmp, s);
					s.consume((String::Size)(::strlen(LOG_UI_PROGRESSION_MAP)));
					// Load the map file
					pPartList->loadFromFile(s);
					updateTheProgressValue(0.);
					return;
				}

				return;
			}

			if (pLogEntryLevelTmp == wxT("progress"))
			{
				manageLogLevelMessage(logProgress, pLogEntryTmp);
				return;
			}
			if (pLogEntryLevelTmp == wxT("warns"))
			{
				manageLogLevelMessage(logWarning, pLogEntryTmp);
				return;
			}
			if (pLogEntryLevelTmp == wxT("error"))
			{
				manageLogLevelMessage(logError, pLogEntryTmp);
				return;
			}
			if (pLogEntryLevelTmp == wxT("fatal"))
			{
				manageLogLevelMessage(logFatal, pLogEntryTmp);
				return;
			}

			if (!pCatchLogEvents)
			{
				// The current message shall be ignored for now
				return;
			}

			if (pLogEntryLevelTmp == wxT("infos"))
			{
				manageLogLevelMessage(logInfo, pLogEntryTmp);
				return;
			}

			if (pLogEntryLevelTmp == wxT("notic") or pLogEntryLevelTmp == wxT("check"))
			{
				manageLogLevelMessage(logNotice, pLogEntryTmp);
				pLastNoticeMessage = pLogEntryTmp;
				return;
			}
			if (pLogEntryLevelTmp == wxT("memory"))
			{
				pLogEntryTmp.ToLong(&pMemoryFootprint);
				return;
			}
			if (pLogEntryLevelTmp == wxT("debug"))
			{
				return;
			}
			if (pLogEntryLevelTmp == wxT("compatibility notice"))
			{
				return;
			}
		}
	}


	wxSizer* Job::createMainPnl(wxWindow* parent)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		wxBoxSizer* ss = new wxBoxSizer(wxVERTICAL);;

		// The TOP panel
		pPanelHeader =
			Toolbox::Components::WizardHeader::Create(parent,
				pTitle, pIcon, pSubTitle, GetClientSize().GetWidth() - 50, false);
		ss->Add(pPanelHeader, 0, wxALL|wxEXPAND);
		ss->AddSpacer(15);

		// Sub components
		ss->Add(createPnlLoading(parent), 0, wxALL|wxEXPAND);
		return ss;
	}




	wxSizer* Job::createPnlLoading(wxWindow* parent)
	{
		wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);

		// Animation + Title
		{
			// Sizer
			wxBoxSizer* spp = new wxBoxSizer(wxHORIZONTAL);
			spp->AddSpacer(20);

			// Animation
			# ifdef wxUSE_ANIMATIONCTRL
			pAnim = new wxAnimationCtrl(parent, wxID_ANY, wxAnimation(), wxDefaultPosition, wxSize(20, 20));
			if (pAnim)
			{
				pAnim->LoadFile(Resources::WxFindFile("animations/loading.gif"));
				spp->Add(pAnim, 0, wxALL);
				spp->AddSpacer(5);
			}
			# else
			# 	warning "wxWidgets has not been compiled with the `AnimationCtrl` support"
			# endif

			// Text progression
			pProgrText = Component::CreateLabel(parent, wxT("Preparing..."), true);
			spp->Add(pProgrText, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

			s->Add(spp, 0, wxALL|wxEXPAND);

			spp = new wxBoxSizer(wxHORIZONTAL);
			spp->AddSpacer(20);

			pProgrSubText = Component::CreateLabel(parent, wxEmptyString);
			spp->Add(pProgrSubText, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
			if (System::windows)
				s->AddSpacer(4);
			s->Add(spp, 0, wxALL|wxEXPAND);
		}

		// The progress bar
		{
			// Sizer
			delete pGaugeSizer;
			pGaugeSizer = new wxBoxSizer(wxHORIZONTAL);

			// Gauge
			pGauge = new Component::ProgressBar(parent);

			//
			pGaugeSizer->Add(20, 1);
			pGaugeSizer->Add(pGauge, 1, wxLEFT|wxRIGHT|wxEXPAND);
			pGaugeSizer->Add(20, 1);

			s->Add(pGaugeSizer, 0, wxALL|wxEXPAND);
			pGaugeParentSizer = s;
		}

		// Informations
		{
			// Sizer
			wxBoxSizer* spg = new wxBoxSizer(wxVERTICAL);
			spg->AddSpacer(5);

			StdPair p;

			// Elapsed time
			p = CreateStdInfo(parent, wxT("Elapsed time: 0s"));
			spg->Add(p.first, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
			pElapsedTimeText = p.second;

			// Remaining time
			p = CreateStdInfo(parent, wxEmptyString); // "Remaining time: estimating..."
			pRemainingSizer = p.first;
			spg->Add(p.first, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
			pRemainingTimeText = p.second;

			// Elapsed time
			p = CreateStdInfo(parent, wxT("Read: 0 byte   Write: 0 byte"));
			spg->Add(p.first, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
			pReadWriteStats = p.second;

			// Extra space for beauty
			spg->AddSpacer(10);

			s->Add(spg, 0, wxALL|wxEXPAND);
		}

		s->Layout();
		return s;
	}


	wxSizer* Job::createPnlButtons(wxWindow* parent)
	{
		// Sizer of the panel
		pPanelButtons = new Component::Panel(parent);

		wxBoxSizer* t = new wxBoxSizer(wxVERTICAL);
		t->AddSpacer(8);
		wxBoxSizer* s = new wxBoxSizer(wxHORIZONTAL);
		t->Add(s, 0, wxALL|wxEXPAND);
		t->AddSpacer(8);
		pPanelButtons->SetSizer(t);

		s->AddSpacer(22);
		pLblErrors = Component::CreateLabel(pPanelButtons, wxEmptyString);
		pLblErrors->Enable(false);
		s->Add(pLblErrors, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
		s->AddStretchSpacer(0);

		pBtnCancel = Component::CreateButton(pPanelButtons, wxT("  Cancel  "), this, &Job::onCancel);
		s->Add(pBtnCancel, 0, wxALIGN_CENTRE_VERTICAL|wxALL);

		s->Add(25, 5);

		pCancelSizer = new wxBoxSizer(wxVERTICAL);
		pCancelSizer->AddSpacer(10);
		pCancelSizer->Add(new wxStaticLine(parent), 0, wxALL|wxEXPAND);
		pCancelSizer->Add(pPanelButtons, 0, wxALL|wxEXPAND);

		return pCancelSizer;
	}


	void Job::stop()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");
		pMutex.lock();
		pGUICanUpdate = 0;
		if (pJobIsRunning)
		{
			pJobIsRunning = 0;
			pMutex.unlock();
			// Stopping then delete all threads
			deleteAllThreads();
			stopAllGuiComponents();

			// Exiting the modal mode
			if (!pWndCancelOperation)
				Antares::Dispatcher::GUI::Close(this);
			else
				Antares::Dispatcher::GUI::Close(pWndCancelOperation);
		}
		else
			pMutex.unlock();
	}


	void Job::stopAllGuiComponents()
	{
		disableAllComponents();
		//if (pAnim)
		//	pAnim->Stop();
		if (pTimerElapsedTime)
			pTimerElapsedTime->stop();
	}


	void Job::disableAllComponents()
	{
		Enable(false);
		pBtnCancel->Enable(false);
		pLblErrors->Enable(false);

		pProgrText->SetLabel(wxT("Completed."));
		pProgrText->Enable(false);
		pMutex.lock();
		pSubMessage.clear();
		pMutex.unlock();
		pProgrSubText->SetLabel(wxEmptyString);
		pProgrSubText->Enable(false);
	}


	bool Job::run()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		if (!pJobIsRunning and this->onRunQuery())
		{
			// Removing all previous threads
			deleteAllThreads();

			// Flag this job as running
			pJobIsRunning = 1;
			pGUICanUpdate = 1;
			// Result
			pResult = true;

			// Default message
			if (pProgrText)
				pProgrText->SetLabel(wxT("Preparing..."));
			pMutex.lock();
			pSubMessage.clear();
			pMutex.unlock();
			if (pProgrSubText)
				pProgrSubText->SetLabel(wxEmptyString);

			this->GetSizer()->Show(pCancelSizer, pCanCancel, false);
			if (pCanCancel)
			{
				SetBackgroundColour(wxColour(255, 255, 255));
				pPanelHeader->SetBackgroundColour(wxColour(255, 255, 255));
				pPanelButtons->SetBackgroundColour(pDefaultBGColor);
				if (pAnim)
					pAnim->SetBackgroundColour(wxColour(255, 255, 255));
			}

			// Hide the gauge
			displayGauge(pDisplayProgression);
			// Reinitialize
			pGauge->value(0);

			pTimerElapsedTime   = new TimerElapsedTime(*this, *pPartList);
			pTimerRemainingTime = new TimerRemainingTime(*this, pTimerElapsedTime);
			pElapsedTimeText->SetLabel(wxEmptyString);
			pRemainingTimeText->SetLabel(wxEmptyString);
			pReadWriteStats->SetLabel(wxEmptyString);

			// Create the thread
			Thread::IThread::Ptr thread = new JobThread(*this);
			pThread = thread;

			// Event
			onBeforeRunning();

			// Starting the animation
			if (pAnim)
				pAnim->Play();

			// Positionate the window on the screen
			recomputeWindowSize();
			CentreOnParent();
			// Display the window
			ShowModal();

			// Destroying the thread as soon as possible
			deleteAllThreads();

			// Stopping the thread
			thread->wait();

			// Ok
			pMutex.lock();
			pErrorMutex.lock();
			bool r = pResult and pWarningList.empty() and pErrorList.empty();
			pErrorMutex.unlock();
			pMutex.unlock();
			return r;
		}
		return false;
	}



	void Job::updateTheProgressValue(double progress)
	{
		if (progress >= 0.)
		{
			if (progress > 100.)
				progress = 100.;
			// The event
			// The new percent
			const int v = Yuni::Math::MinMax<int>(
				(int)Yuni::Math::Trunc((progress / 100.) * PROGRESSBAR_MAX_RANGE_F),
				0, PROGRESSBAR_MAX_RANGE);

			// Broadcast the event
			Yuni::Bind<void()> callback;
			callback.bind(this, &Job::onUIUpdateProgression, v);
			Dispatcher::GUI::Post(callback);
		}
	}


	void Job::onUIUpdateProgression(uint value)
	{
		if (!pDisplayProgression)
			displayGauge(true);
		if (pGauge and pGUICanUpdate)
		{
			const double v = (value * 100.) / PROGRESSBAR_MAX_RANGE_F;
			if (pGauge->value() < v)
				pGauge->value(v);
		}
	}


	void Job::updateTheMessage(const wxString& msg, bool mustBeInterpreted)
	{
		if (!msg.IsEmpty())
		{
			if (mustBeInterpreted)
				displayMessage(msg);
			else
				Dispatcher::GUI::Post(this, &Job::onUIUpdateMessage);
		}
	}


	void Job::onUIUpdateMessage()
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");
		if (pGUICanUpdate and pProgrText)
		{
			//! Updating the message for the user
			pMutex.lock();
			// c_str: A deep copy is required to avoid thread-safety issues
			pProgrText->SetLabel(pMessage.c_str());
			pMutex.unlock();
		}
	}


	void Job::onCancel(void*)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		Window::Message message(Forms::ApplWnd::Instance(), wxT("Simulation"),
			wxT("Do you really want to cancel the operation ?"),
			wxT("All data produced by the process may be lost."));
		pWndCancelOperation = &message;
		message.add(Window::Message::btnYes);
		message.add(Window::Message::btnNo, true);
		if (message.showModal() == Window::Message::btnYes)
		{
			pGUICanUpdate = 0;
			if (pJobIsRunning)
			{
				// Logs
				String* s = new String("The operation has been canceled");
				pErrorMutex.lock();
				pMutex.lock();
				if (s)
					pWarningList.push_back(s);
				pResult = false;
				pErrorMutex.unlock();
				pMutex.unlock();

				disableAllComponents();
				// Rebuilding the layout
				Refresh();
				this->onCancel();
			}
		}
		pWndCancelOperation = nullptr;
		if (!pJobIsRunning)
		{
			stopAllGuiComponents();
			EndModal(0);
		}
	}


	void Job::onUIUpdateElapsedTime(const String& text)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");
		pElapsedTimeText->SetLabel(wxStringFromUTF8(text));
	}


	void Job::onUIUpdateRemainingTime(const String& text)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");
		pRemainingTimeText->SetLabel(wxStringFromUTF8(text));
	}



	void Job::evtOnClose(wxCloseEvent& evt)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		if (pJobIsRunning and evt.CanVeto())
			evt.Veto();
		else
			stopAllGuiComponents();
		evt.Skip();
	}


	void Job::evtOnInit(wxInitDialogEvent&)
	{
		// Asserts
		assert(GetSizer() and "No sizer available");
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		// We will flush all pending before displaying the window
		while (wxTheApp->Pending())
			wxTheApp->Dispatch();

		// Starting all threads
		auto thread = pThread;
		if (!(!thread))
			thread->start();
		pTimerElapsedTime->start();
	}






} // namespace FormatConvert
} // namespace Toolbox
} // namespace Antares

