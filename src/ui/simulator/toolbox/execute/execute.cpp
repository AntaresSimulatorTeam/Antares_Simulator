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

#include <yuni/core/system/suspend.h>
#include "execute.h"
#include "../jobs.h"
#include <iostream>
#include <atomic>
#include <wx/process.h>
#include <wx/txtstrm.h>
#include <wx/utils.h>
#include <wx/filefn.h>
#include <wx/app.h>

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace Process
{
// Forward declaration
class JobProcess;

/*!
** \brief Dedicated class to customize the job
*/
class JobExecute : public Jobs::Job
{
    friend class JobProcess;

public:
    JobExecute(const wxString& command,
               const wxString& workingdirectory,
               const wxString& title,
               const wxString& subTitle,
               const char* icon);
    virtual ~JobExecute();

protected:
    virtual bool executeTask();
    virtual void onBeforeRunning();
    virtual bool onRunQuery();
    virtual void onCancel();
    void disable();

private:
    void evtOnIdle(wxIdleEvent& evt);

private:
    const wxString pCommand;
    const wxString pWDirectory;
    //! Get if the job is
    std::atomic<int> pRunning;
    //! runtime Informations about the process which will be executed
    JobProcess* pProcess;
    //! The original directory
    wxString pPreviousDirectory;
    //! The pid
    long pPid;

    // wxWidgets event table
    DECLARE_EVENT_TABLE()
};

/*!
 * \brief Dedicated class to handle the executed process
 */
class JobProcess : public wxProcess
{
public:
    JobProcess(JobExecute& j) : wxProcess(&j), pJob(j)
    {
    }
    virtual ~JobProcess()
    {
    }

    /*!
     * brief Grab the output
     */
    virtual bool HasInput();

    /*!
     * \brief The execution is over
     */
    virtual void OnTerminate(int pid, int status);

private:
    JobExecute& pJob;

}; // class JobProcess

BEGIN_EVENT_TABLE(JobExecute, Jobs::Job)
EVT_IDLE(JobExecute::evtOnIdle)
END_EVENT_TABLE()

void JobProcess::OnTerminate(int /*pid*/, int /*status*/)
{
    while (IsInputOpened() && HasInput())
        ;

    pJob.disable();
    pJob.pRunning = 0;
}

bool JobProcess::HasInput()
{
    bool hasInput = false;
    if (IsInputAvailable())
    {
        auto* stream = GetInputStream();
        if (stream)
        {
            wxTextInputStream tis(*stream);
            const wxString& msg = tis.ReadLine();
            if (not msg.empty())
                pJob.updateTheMessage(msg);
            hasInput = true;
        }
    }
    if (IsErrorAvailable())
    {
        auto* stream = GetErrorStream();
        if (stream)
        {
            wxTextInputStream tis(*stream);
            const wxString& msg = tis.ReadLine();
            if (not msg.empty())
                pJob.updateTheMessage(msg);
            return true;
        }
    }
    return hasInput;
}

JobExecute::JobExecute(const wxString& command,
                       const wxString& workingdirectory,
                       const wxString& title,
                       const wxString& subTitle,
                       const char* icon) :
 Jobs::Job(title, subTitle, icon),
 pCommand(command),
 pWDirectory(workingdirectory),
 pRunning(0),
 pProcess(nullptr)
{
    pPreviousDirectory = wxGetCwd();
}

JobExecute::~JobExecute()
{
    pRunning = 0;
    ::wxSetWorkingDirectory(pPreviousDirectory);
    delete pProcess;
}

void JobExecute::evtOnIdle(wxIdleEvent& evt)
{
    if (pRunning && pProcess && pProcess->HasInput())
        evt.RequestMore();
}

bool JobExecute::onRunQuery()
{
    return (not pCommand.IsEmpty());
}

void JobExecute::onCancel()
{
    if (pRunning)
    {
        result(false);
        wxProcess::Kill(pPid, wxSIGKILL);

        // We should wait a little for IO operations to finish
        // The system is likely to be unresponsive
        SuspendMilliSeconds(100 /*ms*/); // arbitrary value
    }
}

void JobExecute::onBeforeRunning()
{
    // Important: wxExecute must be called from the main thread (enjoy)...
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    pRunning = 1;

    // Setting the new working directory, if any
    if (not pWDirectory.IsEmpty())
        ::wxSetWorkingDirectory(pWDirectory);

    // Create the process handler
    auto* redirect = new JobProcess(*this);
    redirect->Redirect();

    // Execute the process
    long pidLast;
    pidLast = wxExecute(pCommand, wxEXEC_ASYNC, redirect);
    if (pidLast <= 0)
    {
        // Error
        logs.error() << "The process has been lost";
        delete redirect;
        pRunning = 0;
        pProcess = nullptr;
        pPid = 0;
    }
    else
    {
        pPid = pidLast;
        pProcess = redirect;
    }
}

bool JobExecute::executeTask()
{
    // Return true when the task is complete
    return not pRunning;
}

void JobExecute::disable()
{
    disableAllComponents();
}

Execute::Execute() :
 pTitle(wxT("Execute")),
 pSubTitle(),
 pCommand(),
 pDisplayProgressBar(true),
 pIcon("images/32x32/converter.png")
{
}

Execute::~Execute()
{
}

bool Execute::run()
{
    auto* job = new JobExecute(pCommand, pWDirectory, pTitle, pSubTitle, pIcon);

    // The job can be cancelable
    job->canCancel(true);
    // Start the job in background
    bool ret = job->run();
    // The window will be deleted whenever the framework wants
    job->Destroy();
    return ret;
}

} // namespace Process
} // namespace Toolbox
} // namespace Antares
