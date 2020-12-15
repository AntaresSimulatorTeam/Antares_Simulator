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
#include "../study.h"
#include "../../toolbox/execute/execute.h"
#include "../../windows/message.h"
#include "../../toolbox/jobs.h"
#include <wx/wupdlock.h>
#include "internal-ids.h"
#include "../../../../solver/constraints-builder/cbuilder.h"
#include <ui/common/lock.h>
#include "../wait.h"

#define SEP IO::Separator

using namespace Yuni;

namespace Antares
{
namespace Forms
{
namespace // anonymous
{
class JobBuildConstraints final : public Toolbox::Jobs::Job
{
public:
    JobBuildConstraints(const String& filename, Data::Study::Ptr study) :
     Toolbox::Jobs::Job(wxT("Constraints Builder"),
                        wxT("Build the network constraints"),
                        "images/32x32/run.png"),
     study(study)
    {
        // reset IO statistics
        Statistics::Reset();

        // The main form
        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();

        // Normalize the folder
        IO::Normalize(pFilename, filename);
    }

    //! Destructor
    virtual ~JobBuildConstraints()
    {
    }

    template<class StringT>
    void filename(const StringT& f)
    {
        pFilename = f;
    }

protected:
    /*!
     * \brief Run the constraints builder
     */
    virtual bool executeTask()
    {
        // Logs
        logs.notice() << "Launching the constraints builder...";
        logs.info() << "Location of ini file: " << pFilename;

        // making sure that all internal data are allocated
        study->ensureDataAreAllInitialized();

        // The swap memory MUST not be flushed. This can happen since we are not
        // in the main thread
        MemoryFlushLocker memoryLocker;

        // Constraint Generator
        CBuilder exec(study);

        // assert(exec.loadFromINIFile(pFilename));
        exec.completeFromStudy();
        // exec.deletePreviousConstraints();
        exec.completeCBuilderFromFile(pFilename);

        const bool result = exec.runConstraintsBuilder(pFilename);

        if (result)
        {
            // pGrid->markTheStudyAsModified();
            /*const SaveResult r = ::Antares::SaveStudy();
            if (!(r == svsDiscard or r == svsSaved))
            {
            Enable(false);
            return;
            }*/
            study->uiinfo->reloadBindingConstraints();

            OnStudyConstraintAdded(nullptr);

            // MarkTheStudyAsModified();
            /**/
        }

        // The task is complete
        return true;
    }

private:
    //! The .ini filename
    String pFilename;
    //! Our study
    String pTitle;
    // reference to the study
    Data::Study::Ptr study;

}; // class JobSaveStudy

} // anonymous namespace

void ApplWnd::evtLaunchConstraintsBuilder(wxCommandEvent& evt)
{
    // We assume here that the study is already saved

    auto& mainFrm = *Forms::ApplWnd::Instance();
    Forms::Disabler<Forms::ApplWnd> disabler(mainFrm);

    // Checking for orphan swap files
    // mainFrm.timerCleanSwapFiles(100 /*ms*/);

    // Getting when the process was launched
    const wxDateTime startTime = wxDateTime::Now();

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

    // Ok ! We're good to go !
    GUILocker locker;
    logs.notice() << "Launching the constraint generator...";
    WIP::Locker wip;
    StudyUpdateLocker studylocker;

    mainFrm.SetStatusText(wxString() << wxT("  Building Network Constraints "));

    // Building Network Constraints (in background)
    auto* job = new JobBuildConstraints(filename, study);
    const bool result = job->run();
    job->Destroy();

    // Forms::Disabler<Forms::ApplWnd> disabler(mainFrm);
    {
        // Lock the window to prevent flickering
        wxWindowUpdateLocker updater(&mainFrm);

        // Refreshing the output
        RefreshListOfOutputsForTheCurrentStudy();

        // The refresh is important. This is the way to force the reloading
        // some data (mainly wxGrid)
        mainFrm.refreshStudyLogs();

        // The study is not modified anymore
        // ResetTheModifierState(false);

        if (result)
        {
            logs.info() << "The constraint generator has finished.";
        }
        // Reload all data
        logs.info() << "Updating the study data...";

        // The binding constraints data must be reloaded since the current
        // code is not able to dynamically reload it by itself
        study->ensureDataAreLoadedForAllBindingConstraints();

        // Reload runtime info about the study (Paranoid, should not be required)
        if (study->uiinfo)
            study->uiinfo->reloadAll();

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

    // How long did the generation take ?
    const wxTimeSpan timeSpan = wxDateTime::Now() - startTime;

    Window::Message message(&mainFrm,
                            wxT("Constraint Generator"),
                            wxT("The constraint generator has finished"),
                            wxString() << wxT("Elapsed time : ") << timeSpan.Format());
    message.add(Window::Message::btnContinue);
    message.showModal();
}

void ApplWnd::launchConstraintsBuilder(const String& filename)
{
    if (not filename.empty())
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnIDLaunchConstraintsBuilder);
        evt.SetString(wxStringFromUTF8(filename));
        AddPendingEvent(evt);
    }
}

} // namespace Forms
} // namespace Antares
