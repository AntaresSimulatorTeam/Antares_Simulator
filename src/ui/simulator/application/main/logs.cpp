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
#include <ui/common/wx-wrapper.h>
#include "../main.h"
#include "../../windows/studylogs.h"
#include "../../../config.h"
#include <stdio.h>
#include <time.h>
#include <wx/stdpaths.h>
#include "../../windows/message.h"
#include "../../toolbox/components/htmllistbox/item/error.h"
#include "internal-ids.h"

using namespace Yuni;

namespace Antares
{
namespace Forms
{
namespace
{
class LogInfo final
{
public:
    bool warning;
    wxString entry;
};

typedef std::vector<LogInfo*> WaitingLogEntries;

WaitingLogEntries waitingLogEntry;
Yuni::Mutex logMutex;

Yuni::Atomic::Int<> logUpdateCount;

class LogFlusherTimer final : public wxTimer
{
public:
    LogFlusherTimer() : wxTimer()
    {
    }

    virtual ~LogFlusherTimer()
    {
    }

    virtual void Notify() override
    {
        if (!(!logUpdateCount))
        {
            Start(300, wxTIMER_ONE_SHOT);
            return;
        }

        logMutex.lock();
        if (waitingLogEntry.empty())
        {
            logMutex.unlock();
            return;
        }

        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
        Forms::Disabler<Forms::ApplWnd> disabler(mainFrm);

        wxString msg;
        if (waitingLogEntry.size() == 1)
        {
            msg = waitingLogEntry[0]->entry;
        }
        else
        {
            uint w = 0;
            uint e = 0;
            for (uint i = 0; i != waitingLogEntry.size(); ++i)
            {
                if (waitingLogEntry[i]->warning)
                    ++w;
                else
                    ++e;
            }
            if (w)
            {
                if (w == 1)
                    msg << wxT("1 warning");
                else
                    msg << w << wxT(" warnings");
                if (e)
                    msg << wxT(", ");
            }
            if (e)
            {
                if (e == 1)
                    msg << wxT("1 error");
                else
                    msg << e << wxT(" errors");
            }
        }

        auto* message = new Window::Message(
          &mainFrm, wxT("Log"), wxT("Log Report"), msg, "images/misc/warning.png");
        message->add(Window::Message::btnContinue, true);

        // We should display a list of all errors, only if more than one
        if (waitingLogEntry.size() > 1)
        {
            uint w = 0;
            uint l;
            uint count = (uint)waitingLogEntry.size();
            String text;
            for (uint i = 0; i != count; ++i)
            {
                LogInfo* info = waitingLogEntry[i];

                wxStringToString(info->entry, text);
                l = (uint)(30 + text.size() * 6.2) /*px*/;
                if (l > w)
                    w = l;

                if (info->warning)
                    message->appendWarning(text);
                else
                    message->appendError(text);

                delete info;
            }
            message->recommendedWidth(w);
        }
        else
        {
            // We already know that the list is not empty
            assert((!waitingLogEntry.empty()) && "The list must not be empty");
            delete waitingLogEntry[0];
        }

        waitingLogEntry.clear();
        logMutex.unlock();

        message->showModalAsync();
    }

}; // class LogFlusherTimer

} // anonymous namespace

void ApplWnd::createLogs()
{
    pLogFlusherTimer = new LogFlusherTimer();
    connectLogCallback();
}

void ApplWnd::destroyLogs()
{
    if (pLogFlusherTimer)
    {
        wxTimer* timer = pLogFlusherTimer;
        pLogFlusherTimer = NULL;
        delete timer;
    }
}

void ApplWnd::showStudyLogs()
{
    if (!pWndLogs)
        pWndLogs = new Window::StudyLogs(this);
    pWndLogs->Show();
    pWndLogs->Raise();
}

void ApplWnd::refreshStudyLogs()
{
    if (pWndLogs)
        pWndLogs->refreshListOfAllAvailableLogs();
}

void ApplWnd::destroyLogsViewer()
{
    if (pWndLogs)
    {
        pWndLogs->Destroy();
        pWndLogs = NULL;
    }
}

void ApplWnd::connectLogCallback()
{
    logs.callback.clear();
    logs.callback.connect(this, &ApplWnd::onLogMessageDeferred);
}

void ApplWnd::onLogMessageDeferred(int level, const String& message)
{
    if (not message.empty() and message.first() != '[')
    {
        // wxLogError(), like wxLogWarning(), are routine with
        // variadic parameters, like the standard printf.
        // Consequently, any char `%` are interpreted as additional
        // parameters to read, which will lead to a SegV.
        // We must replace all occurences of `%` by `%%`.
        switch (level)
        {
        case Logs::Verbosity::Warning::level:
        {
            auto* info = new LogInfo();
            info->warning = true;
            info->entry = wxStringFromUTF8(message);
            info->entry.Replace(wxT("%"), wxT("%%"));

            logMutex.lock();
            waitingLogEntry.push_back(info);
            logMutex.unlock();

            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnInternalLogMessage);
            AddPendingEvent(evt);
            break;
        }
        case Logs::Verbosity::Error::level:
        case Logs::Verbosity::Fatal::level:
        {
            auto* info = new LogInfo();
            info->warning = false;
            info->entry = wxStringFromUTF8(message);
            info->entry.Replace(wxT("%"), wxT("%%"));

            logMutex.lock();
            waitingLogEntry.push_back(info);
            logMutex.unlock();

            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, mnInternalLogMessage);
            AddPendingEvent(evt);
            break;
        }
        default:
            break;
        }
    }
}

void ApplWnd::onLogMessage(wxCommandEvent&)
{
    if (pLogFlusherTimer)
        pLogFlusherTimer->Start(300, wxTIMER_ONE_SHOT);
}

void ApplWnd::endUpdateLogs() const
{
    --logUpdateCount;
}

void ApplWnd::beginUpdateLogs() const
{
    ++logUpdateCount;
}

} // namespace Forms
} // namespace Antares
