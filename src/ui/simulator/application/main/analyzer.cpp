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

#include "main.h"
#include <antares/logs.h>
#include "../study.h"
#include "../../toolbox/execute/execute.h"
#include "../../windows/message.h"
#include "../../toolbox/jobs.h"
#include <wx/wupdlock.h>
#include "internal-ids.h"


using namespace Yuni;

namespace Antares
{
namespace Forms
{

	void ApplWnd::launchAnalyzer(const String& filename)
	{
		if (not filename.empty())
		{
			wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDLaunchAnalyzer);
			evt.SetString(wxStringFromUTF8(filename));
			AddPendingEvent(evt);
		}
	}


	void ApplWnd::evtLaunchAnalyzer(wxCommandEvent& evt)
	{
		// We assume here that the study is already saved

		auto& mainFrm = *Forms::ApplWnd::Instance();
		Forms::Disabler<Forms::ApplWnd> disabler(mainFrm);

		// Checking for orphan swap files
		mainFrm.timerCleanSwapFiles(100 /*ms*/);

		const wxString wfilename = evt.GetString();
		if (wfilename.empty())
			return;
		String filename;
		wxStringToString(wfilename, filename);

		auto study = Data::Study::Current::Get();
		if (!study) // A valid study would be better
		{
			logs.fatal() << "Internal error: Please provide a valid study";
			OnStudyEndUpdate();
			IO::File::Delete(filename);
			return;
		}

		// Logs
		{
			logs.info();
			logs.checkpoint() << "Launching the analyzer...";
			logs.info() << "Gathering informations...";
		}


		// Where is our solver ?
		String analyzerLocation;
		{
			if (!Solver::FindAnalyzerLocation(analyzerLocation))
			{
				logs.error() << "Impossible to find the program `antares-analyzer`.";
				IO::File::Delete(filename);
				OnStudyEndUpdate();
				return;
			}
			logs.info() <<  "  Found Analyzer: `" << analyzerLocation << '`';
			logs.info() <<  "  Study folder      : `" << study->folder << '`';
			logs.debug() << "  Running from " << filename;
			logs.info();
		}

		// The process utility
		auto* exec = new Toolbox::Process::Execute();
		exec->title(wxT("Analyzer"));
		exec->subTitle(wxT(""));
		exec->icon("images/32x32/run.png");

		// The command line
		{
			String cmd;
			if (System::unix)
				cmd << "nice ";
			cmd << "\"" << analyzerLocation << "\" -i \"" << filename << "\"";
			exec->command(wxStringFromUTF8(cmd));

			logs.debug() << "running " << cmd;
		}

		// Already done from windows/analyzer/analyzer.cpp, evtProceed()
		//OnStudyBeginUpdate();

		// Getting when the process was launched
		const wxDateTime startTime = wxDateTime::Now();

		// Running the simulation - it may take some time
		const bool result = exec->run();

		// Releasing
		delete exec;

		// How long took the simulation ?
		const wxTimeSpan timeSpan = wxDateTime::Now() - startTime;

		{
			// Lock the window to prevent flickering
			wxWindowUpdateLocker updater(&mainFrm);

			// Refreshing the output
			RefreshListOfOutputsForTheCurrentStudy();

			// The refresh is important. This is the way to force the reloading
			// some data (mainly wxGrid)
			mainFrm.refreshStudyLogs();

			if (result)
				logs.info() << "The analyzer has finished.";
			logs.info();

			// Reload all data
			logs.info() << "Updating the study data...";
			study->reloadCorrelation();
			study->reloadXCastData();
			// The binding constraints data must be reloaded since the current
			// code is not able to dynamically reload it by itself
			study->ensureDataAreLoadedForAllBindingConstraints();
			// Reload runtime info about the study (Paranoid, should not be required)
			study->uiinfo->reload();

			GUIFlagInvalidateAreas = true;

			OnStudyEndUpdate();

			OnStudyChanged(*study);
			OnStudySettingsChanged();
			OnStudyAreasChanged();

			// Reset the status bar
			mainFrm.resetDefaultStatusBarText();

			mainFrm.forceRefresh();
		}

		// Checking for orphan swap files
		// We may have to clean the cache folder
		mainFrm.timerCleanSwapFiles(5000 /*ms*/);

		// Remove the temporary file
		IO::File::Delete(filename); 

		if (result)
		{
			Window::Message message(&mainFrm, wxT("Analyzer"),
				wxT("The analyzer has finished"),
				wxString() << wxT("Time to complete the preprocessing : ") << timeSpan.Format());
			message.add(Window::Message::btnContinue);
			message.showModal();
		}
	}





} // namespace Forms
} // namespace Antares

