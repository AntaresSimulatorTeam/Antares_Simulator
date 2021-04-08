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

#include <cassert>
#include "application.h"
#include "main.h"
#include "../config.h"
#include <wx/font.h>
#include <wx/image.h>
#include <wx/app.h>
#include "../toolbox/resources.h"
#include <antares/hostinfo.h>
#include <yuni/io/file.h>
#include <antares/jit.h>
#include <antares/logs.h>
#include <antares/memory/memory.h>
#include <wx/config.h>
#include "../windows/message.h"
#include <antares/sys/appdata.h>
#include <antares/sys/policy.h>
#include "study.h"
#include <yuni/datetime/timestamp.h>
#include <antares/logs/cleaner.h>
#include "../../../internet/license.h"
#include <antares/locale.h>
#ifndef YUNI_OS_WINDOWS
#include <signal.h>
#endif

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
Forms::ApplWnd* mainFrame = nullptr;

static void detectStudyToLoadAtStartup()
{
    String name;
    IO::ExtractFileName(name, Forms::StudyToLoadAtStartup);
    name.toLower();

    // Fix when a study header file has been given
    if (name == "study.antares")
    {
        String t;
        IO::ExtractFilePath(t, Forms::StudyToLoadAtStartup);
        Forms::StudyToLoadAtStartup = t;
    }
    else
    {
        // Another fix when an output header file has been given
        if (name == "info.antares-output")
        {
            String t;
            IO::ExtractFilePath(t, Forms::StudyToLoadAtStartup);
            if (System::windows)
                Forms::StudyToLoadAtStartup.clear() << t << "\\..\\..";
            else
                Forms::StudyToLoadAtStartup.clear() << t << "/../..";
        }
    }

    if (not Antares::Forms::StudyToLoadAtStartup.empty())
    {
        IO::MakeAbsolute(name, Forms::StudyToLoadAtStartup);
        IO::Normalize(Antares::Forms::StudyToLoadAtStartup, name);

        if (not IO::Exists(Antares::Forms::StudyToLoadAtStartup))
            Antares::Forms::StudyToLoadAtStartup.clear();
    }
}

static void AppendDate(String& out)
{
    DateTime::TimestampToString(out, "%Y%m%d-%H%M%S", 0, false);
}

static void OpenLogFilename()
{
    String ff;
    ff.reserve(1024);
    if (!LocalPolicy::Read(ff, "ui_logs_folder") || ff.empty())
    {
        OperatingSystem::FindLocalAppData(ff);
        if (ff.empty())
            return;
        ff << SEP << "rte" << SEP << "antares" << SEP << "logs";
    }
    if (not ff.empty() and IO::Directory::Create(ff))
    {
        ff << SEP << "uisimulator-";
        AppendDate(ff);
        ff << ".log";
        logs.logfile(ff);
    }
}

static void OnNotifyStudyBeginUpdate()
{
    // logs.info() << "[notify study begin update]";
}

static void OnNotifyStudyEndUpdate()
{
    // logs.info() << "[notify study end update]";
}

static void OnNotifyStudyLoaded()
{
    // logs.info() << "notify study loaded and ready";
}

static void OnNotifyStudyClosed()
{
    // logs.info() << "notify study closing immediatly";
}

static void NotEnoughMemory()
{
    // Displaying a message in its own scope to make it is released
    // before exiting.
    {
        Window::Message message(nullptr,
                                wxT("Not enough memory"),
                                wxT("Antares UI error"),
                                wxT("There is not enough memory to perform the action. Aborting."));
        message.add(Window::Message::btnQuit);
        message.showModal();
    }
    logs.error() << "Not enough memory. aborting.";
    exit(42);
}

#ifndef YUNI_OS_WINDOWS
void signalCtrl_term(int)
{
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
    {
        std::cout << std::endl; // for beauty from the console
        logs.info();
        logs.notice() << "[signal] received signal SIGTERM. Exiting...";
        mainFrm->Close();
    }
}

void signalCtrl_int(int)
{
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
    {
        std::cout << std::endl; // for beauty from the console
        logs.info();
        logs.notice() << "[signal] received signal SIGINT. Exiting...";
        mainFrm->Close();
    }
}
#endif

static void InstallSignalHandlers()
{
#ifndef YUNI_OS_WINDOWS
    logs.info() << "registering signal handlers";
    // signal(SIGUSR1, signalCtrl_usr1);
    // signal(SIGUSR2, signalCtrl_usr2);
    // signal(SIGQUIT, signalCtrl_quit);
    signal(SIGTERM, signalCtrl_term);
    signal(SIGINT, signalCtrl_int);
#endif
}

Application::Application() : wxApp()
{
    // Dealing with the lack of memory
    std::set_new_handler(&NotEnoughMemory);
    // Antares SWAP
    memory.initialize();
}

bool Application::OnInit()
{
#ifdef YUNI_OS_MSVC
    // Application name
    logs.applicationName("antares");

    // Local policy
    // Initializing the resources...
    //
    // When ran from Visual Studio, we don't have our own implementation
    // of the main routine
    // We only have argc/argv from wx
    {
        String s;
        wxStringToString(wxString(argv[0]), s);
        char* c_argv[] = {s.data(), nullptr};

        // Load the local policy settings
        LocalPolicy::Open();
        LocalPolicy::CheckRootPrefix(s.c_str());
        // Load the resources
        Resources::Initialize(1, c_argv, true);
    }
#endif

    // Log filename
    OpenLogFilename();

    // Set the UTF-8 encoding
    // wxFont::SetDefaultEncoding(wxFONTENCODING_UTF8);
    wxFont::SetDefaultEncoding(wxFONTENCODING_SYSTEM);
    // Title
    SetAppName(wxString(wxT("Antares v"))
               << int(ANTARES_VERSION_HI) << wxT(".") << int(ANTARES_VERSION_LO)
#ifndef NDEBUG
               << wxT(" - DEBUG")
#endif
    );

    // Actiavating the JIT Mode, which is disabled by default
    JIT::enabled = true;
    JIT::usedFromGUI = true;

    logs.checkpoint() << "Antares UI Simulator  v" << ANTARES_VERSION_PUB_STR;
    WriteHostInfoIntoLogs();
    logs.info();

    // Re-force the locale after wx is initialized

    // Print the local policy settings
    LocalPolicy::DumpToLogs();

    // Notify
    OnStudyBeginUpdate.connect(&OnNotifyStudyBeginUpdate);
    OnStudyEndUpdate.connect(&OnNotifyStudyEndUpdate);
    OnStudyLoaded.connect(&OnNotifyStudyLoaded);
    OnStudyClosed.connect(&OnNotifyStudyClosed);

#ifdef YUNI_OS_MSVC
    if (this->argc > 1)
    {
        for (uint i = 1; i < (uint)this->argc; ++i)
        {
            wxStringToString((wxString() << argv[(int)i]), Antares::Forms::StudyToLoadAtStartup);
            detectStudyToLoadAtStartup();
            if (not Antares::Forms::StudyToLoadAtStartup.empty())
                break;
        }
    }
#endif

    // Detecting study to load at startup (once the gui is created)
    if (not Antares::Forms::StudyToLoadAtStartup.empty())
        detectStudyToLoadAtStartup();

    // Image handlers
    wxInitAllImageHandlers();

    // Looking the temp folder
    {
        auto now = DateTime::Now();
        auto* config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));
        bool useDefaults = false;
        if (config->Read(wxT("tmp/usedefaults"), &useDefaults, false) and !useDefaults)
        {
            const wxString& pth = config->Read(wxT("tmp/path"), wxEmptyString);
            if (not pth.empty())
            {
                String p;
                wxStringToString(pth, p);
                Antares::memory.cacheFolder(p);
            }
        }

        // The last time that the application has been launched
        long lasttime = 0;

        if (not config->Read(wxT("last"), &lasttime))
            lasttime = 0;
        config->Write(wxT("last"), (long)now);
        delete config;

        if (now - lasttime > 3600 * 24 * 20)
        {
            String path;
            IO::ExtractFilePath(path, logs.logfile());
            logs.info() << "deleting old log files in " << path << "...";
            PurgeLogFiles(path);
        }
    }

    Antares::memory.displayInfo();

    InstallSignalHandlers();

    // Creating the main frame
    mainFrame = new Forms::ApplWnd();
    // Checking for orphan swap files
    mainFrame->timerCleanSwapFiles(700 /*ms*/);

    // Show time !
    mainFrame->Show(true);
    // Set the frame to the top window
    SetTopWindow(mainFrame);

    return true;
}

Application::~Application()
{
    // Removing all unused swap files
    memory.removeAllUnusedSwapFiles();
    // Checking for orphan swap files
    memory.cleanupCacheFolder();

    logs.info() << "Exiting now.";
}

} // namespace Antares
