/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "main.h"
#include <antares/logs/logs.h>
#include "../study.h"
#include "../../toolbox/execute/execute.h"
#include "../../windows/message.h"
#include "../../toolbox/jobs.h"
#include <wx/wupdlock.h>
#include "internal-ids.h"
#include "antares/study/ui-runtimeinfos.h"
#include "application/study.h"

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

    const wxString wfilename = evt.GetString();
    if (wfilename.empty())
        return;
    String filename;
    wxStringToString(wfilename, filename);

    auto study = GetCurrentStudy();
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
        logs.info() << "  Found Analyzer: `" << analyzerLocation << '`';
        logs.info() << "  Study folder      : `" << study->folder << '`';
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
    // OnStudyBeginUpdate();

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

    // Remove the temporary file
    IO::File::Delete(filename);

    if (result)
    {
        Window::Message message(
          &mainFrm,
          wxT("Analyzer"),
          wxT("The analyzer has finished"),
          wxString() << wxT("Time to complete the preprocessing : ") << timeSpan.Format());
        message.add(Window::Message::btnContinue);
        message.showModal();
    }
}

} // namespace Forms
} // namespace Antares
