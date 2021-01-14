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
#include "internal-data.h"
#include "internal-ids.h"

// Map
#include "../../toolbox/components/map/component.h"
// Main Panel
#include "../../toolbox/components/mainpanel.h"
#include "../../toolbox/dispatcher/study.h"

#include "../menus.h"
#include "../study.h"

// Windows
#include <ui/common/component/spotlight/spotlight.h>
#include "../../windows/studylogs.h"
#include "../../windows/memorystatistics.h"
#include "../../windows/inspector.h"
#include "../../windows/message.h"
#include "../../../common/lock.h"
#include "../../windows/startupwizard.h"

#include <wx/textdlg.h>

using namespace Yuni;

namespace Antares
{
namespace Forms
{
void ApplWnd::evtOnQuit(wxCommandEvent&)
{
    logs.notice() << LOG_UI << "Exiting";
    SetStatusText(wxT("Exiting"));
    Dispatcher::GUI::Close(this);
}

void ApplWnd::evtOnMemoryUsedByTheStudy(wxCommandEvent&)
{
    Dispatcher::GUI::CreateAndShowModal<Window::MemoryStatistics>(this);
}

void ApplWnd::evtOnWizard(wxCommandEvent&)
{
    Window::StartupWizard::Show();
}

void MainFormData::onToolbarWizard(void*)
{
    Window::StartupWizard::Show();
}

void ApplWnd::evtOnFullscreen(wxCommandEvent&)
{
    Yuni::Bind<void()> callback;
    callback.bind(pData, &MainFormData::onToolbarFullscreen, nullptr);
    Dispatcher::GUI::Post(callback, 20 /*ms*/);
}

void MainFormData::onToolbarFullscreen(void*)
{
    pMainForm.ShowFullScreen(!pMainForm.IsFullScreen(),
                             wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION);
    pMainForm.pAUIManager.Update();
    Dispatcher::GUI::Refresh(&pMainForm);
}

void MainFormData::onToolbarInspector(void*)
{
    if (Data::Study::Current::Valid())
        Antares::Window::Inspector::Show();
}

void MainFormData::onToolbarOptimizationPreferences(void*)
{
    wxCommandEvent dummy;
    pMainForm.evtOnOptionsOptimizationPrefs(dummy);
}

void ApplWnd::evtOnInspector(wxCommandEvent&)
{
    if (Data::Study::Current::Valid())
        Antares::Window::Inspector::Show();
}

void ApplWnd::evtOnSetStudyInfos(wxCommandEvent& evt)
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return;

    Forms::Disabler<ApplWnd> disabler(*this);
    switch (evt.GetId())
    {
    case mnIDStudyEditTitle:
    {
        wxTextEntryDialog dialog(this,
                                 wxT("Please enter the new name of the study :"),
                                 wxT("Name of the study"),
                                 wxStringFromUTF8(study->header.caption),
                                 wxOK | wxCANCEL);

        if (dialog.ShowModal() == wxID_OK)
        {
            MarkTheStudyAsModified();
            pMainPanel->studyCaption(dialog.GetValue());
            title(dialog.GetValue());
            String stdText;
            wxStringToString(dialog.GetValue(), stdText);
            logs.info() << "Renamed the study's title to '" << stdText << "'";
            study->header.caption = stdText;
        }
        break;
    }
    case mnIDStudyEditAuthors:
    {
        wxTextEntryDialog dialog(this,
                                 wxT("Please enter the new author(s) of the study :"),
                                 wxT("Author(s) of the study"),
                                 wxStringFromUTF8(study->header.author),
                                 wxOK | wxCANCEL);

        if (dialog.ShowModal() == wxID_OK)
        {
            MarkTheStudyAsModified();
            pMainPanel->author(dialog.GetValue());
            String stdText;
            wxStringToString(dialog.GetValue(), stdText);
            logs.info() << "Renamed the study's authors to '" << stdText << "'";
            study->header.author = stdText;
        }
        break;
    }
    }
}

void ApplWnd::evtOnFrameClose(wxCloseEvent& evt)
{
    logs.debug() << "  Event: main frame: OnClose";

    if (IsGUIAboutToQuit())
    {
        evt.Skip();
    }
    else
    {
        // Change the focus - to avoid race condition with wxGrid or any other
        // component
        SetFocus();
        // Notifying that the interface is about to exit
        GUIIsAboutToQuit();

        // Close any opened windows
        Component::Spotlight::FrameClose();

        // !! It is extremly important to wait for all jobs to finish
        // In the contrary, it may appen a race condition with another thread
        // and the swap mode. It would be possible to flush all variables
        // while accessing them
        Dispatcher::Wait();

        // Closing the study if any then quitting the application
        Dispatcher::StudyClose(false, true);

        // Canceling the event
        evt.Veto();
    }
}

void ApplWnd::internalFrameClose()
{
    logs.debug() << "quitting the application. Triggerring event onApplicationQuit";
    onApplicationQuit();
    Destroy();
}

void ApplWnd::evtOnStudySessions(wxCommandEvent&)
{
}

void ApplWnd::evtOnStudyLogs(wxCommandEvent&)
{
    showStudyLogs();
}

void MainFormData::onToolbarLogs(void*)
{
    pMainForm.showStudyLogs();
}

} // namespace Forms
} // namespace Antares
