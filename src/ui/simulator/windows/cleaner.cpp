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

#include "cleaner.h"
#include <yuni/io/directory.h>

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/timer.h>
#include <wx/app.h>
#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/statline.h>

#include "../toolbox/components/htmllistbox/item/info.h"
#include "../toolbox/components/htmllistbox/item/pathlistitem.h"
#include "../toolbox/components/wizardheader.h"
#include "../toolbox/create.h"
#include "../application/main.h"
#include "../application/wait.h"
#include <antares/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
class CleaningThread final : public Yuni::Thread::IThread
{
public:
    CleaningThread(StudyCleaner* parent) : pParent(parent)
    {
    }

    virtual ~CleaningThread()
    {
        // Removing references to this instance
        {
            Yuni::MutexLocker locker(pParent->pMutex);
            if (pParent->pInfos)
                pParent->pInfos->onProgress.unbind();
        }
        stop();
    }

protected:
    virtual bool onStarting() override
    {
        // Cleaning
        Yuni::MutexLocker locker(pParent->pMutex);
        if (pParent->pInfos)
        {
            pParent->pInfos->onProgress.bind(this, &CleaningThread::onProgress);
            delete pParent->pInfos;
            pParent->pInfos = nullptr;
        }
        return true;
    }

    virtual bool onExecute() override
    {
        // Folder
        String stdFolder;
        pParent->pMutex.lock();
        wxStringToString(pParent->pFolder, stdFolder);

        if (not stdFolder.empty() and IO::Directory::Exists(stdFolder))
        {
            logs.callback.clear();
            pParent->pInfos = new Data::StudyCleaningInfos(stdFolder.c_str());
            pParent->pInfos->onProgress.bind(this, &CleaningThread::onProgress);
            pParent->pMutex.unlock();

            pParent->pInfos->analyze();
            Forms::ApplWnd::Instance()->connectLogCallback();
        }
        else
            pParent->pMutex.unlock();

        return false;
    }

    virtual void onStop()
    {
        Bind<void()> callback;
        callback.bind(pParent, &StudyCleaner::updateGUI, true);
        Dispatcher::GUI::Post(callback);
    }

    bool onProgress(uint count)
    {
        pParent->progress(count);
        return !(shouldAbort());
    }

private:
    StudyCleaner* pParent;
};

class CleaningInfoRefreshTimer final : public wxTimer
{
public:
    CleaningInfoRefreshTimer(StudyCleaner* parent) : pParent(parent)
    {
        // Do Nothing
    }

    virtual ~CleaningInfoRefreshTimer()
    {
        // Do nothing
    }

    virtual void Notify() override
    {
        pParent->updateProgressionLabel();
    }

private:
    StudyCleaner* pParent;
};

StudyCleaner::StudyCleaner(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Cleaner"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN),
 pInfos(nullptr),
 pRefreshTimer(nullptr),
 pThread(nullptr)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Header
    mainSizer->Add(
      Toolbox::Components::WizardHeader::Create(
        this,
        wxT("Study folder cleaner"),
        "images/32x32/cleaner.png",
        wxT("Remove all files and folders that are no longer needed in a study folder")),
      0,
      wxALL | wxEXPAND);

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(sizer, 1, wxALL | wxEXPAND);

    pListbox = new Component::HTMLListbox::Component(this);
    sizer->AddSpacer(15);
    sizer->Add(pListbox, 1, wxALL | wxEXPAND);
    sizer->AddSpacer(15);

    // Buttons
    wxBoxSizer* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
    pnlBtns->AddSpacer(15);

    // Refresh
    pBtnRefresh = Component::CreateButton(this, wxT("Refresh"), this, &StudyCleaner::onRefresh);
    pnlBtns->Add(pBtnRefresh);
    pnlBtns->AddSpacer(15);

    pLblInfos = new wxStaticText(this, wxID_ANY, wxString());
    pnlBtns->Add(pLblInfos, 0, wxALL | wxALIGN_CENTER_VERTICAL);
    pnlBtns->AddStretchSpacer();

    // Cancel
    pBtnCancel = Component::CreateButton(this, wxT("Cancel"), this, &StudyCleaner::onCancel);
    pnlBtns->Add(pBtnCancel);
    pnlBtns->AddSpacer(5);
    pBtnGo = Component::CreateButton(
      this, wxT("Perform a disk cleanup"), this, &StudyCleaner::onProceed);
    pnlBtns->Add(pBtnGo, 0, wxALL | wxEXPAND);
    pnlBtns->AddSpacer(15);

    // Button Panel
    if (System::unix)
        mainSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND, 8);
    else
        mainSizer->AddSpacer(8);

    mainSizer->Add(pnlBtns, 0, wxALL | wxEXPAND);
    mainSizer->AddSpacer(8);

    // Set the sizer
    sizer->Layout();
    SetSizer(mainSizer);

    GetSizer()->Fit(this);
    wxSize p = GetSize();
    p.SetWidth(p.GetWidth() + 20);
    if (p.GetHeight() < 430)
        p.SetHeight(430);
    if (p.GetWidth() < 750)
        p.SetWidth(750);
    SetSize(p);
    Centre(wxBOTH);

    pRefreshTimer = new CleaningInfoRefreshTimer(this);
}

StudyCleaner::~StudyCleaner()
{
    delete pRefreshTimer;
    if (pThread)
    {
        pThread->stop();
        delete pThread;
    }

    delete pInfos;
}

void StudyCleaner::studyFolder(const wxString& folder)
{
    if (pRefreshTimer)
        pRefreshTimer->Stop();

    if (not pThread)
        pThread = new CleaningThread(this);
    else
        pThread->stop();

    // Reset internal variables
    {
        Yuni::MutexLocker locker(pMutex);
        if (&pFolder != &folder) // avoid possible undefined behavior
            pFolder = folder;
        pProgressionCount = 0;
    }

    // We are working !
    pListbox->clear();
    *(pListbox) += new Component::HTMLListbox::Item::Info(wxT("Gathering informations..."));
    pLblInfos->SetLabel(wxT("Gathering informations..."));
    GetSizer()->Show(pBtnGo, false, true);
    GetSizer()->Show(pBtnRefresh, false, true);
    pBtnGo->Enable(true);
    GetSizer()->Layout();
    Refresh();

    if (pRefreshTimer)
        pRefreshTimer->Start(300 /*ms*/, wxTIMER_CONTINUOUS);
    pThread->start();
}

void StudyCleaner::onRefresh(void*)
{
    // Refresh
    studyFolder(pFolder);
}

void StudyCleaner::onCancel(void*)
{
    Enable(false);
    if (pBtnRefresh)
        pBtnRefresh->Enable(false);
    if (pBtnCancel)
        pBtnCancel->Enable(false);
    if (pBtnGo)
        pBtnGo->Enable(false);
    Refresh();
    if (pThread)
        pThread->gracefulStop();

    Dispatcher::GUI::Post(this, &StudyCleaner::closeMe);
}

void StudyCleaner::closeMe()
{
    if (pRefreshTimer)
        pRefreshTimer->Stop();
    if (pThread)
        pThread->stop();
    Dispatcher::GUI::Close(this);
}

void StudyCleaner::onProceed(void*)
{
    WIP::Locker wip;
    pLblInfos->SetLabel(wxT("Performing a cleanup..."));
    GetSizer()->Show(pBtnGo, false, true);
    GetSizer()->Show(pBtnRefresh, false, true);
    GetSizer()->Layout();
    Refresh();

    // Performing the cleanup
    Dispatcher::GUI::Post(this, &StudyCleaner::launchCleanup);
}

void StudyCleaner::launchCleanup()
{
    // Performing the cleanup
    if (pInfos)
        pInfos->performCleanup();
    studyFolder(pFolder);
}

void StudyCleaner::updateProgressionLabel()
{
    if (not pLblInfos)
        return;

    pMutex.lock();
    uint p = pProgressionCount;
    pMutex.unlock();

    switch (p)
    {
    case 0:
        break;
    case 1:
        pLblInfos->SetLabel(wxT("1 folder has been analyzed..."));
        break;
    default:
        pLblInfos->SetLabel(wxString() << p << wxT(" folders have been analyzed..."));
    }
}

void StudyCleaner::updateGUI(bool hasItems)
{
    if (pRefreshTimer)
        pRefreshTimer->Stop();

    pMutex.lock();

    bool hasAtLeastOneItem = false;

    pListbox->clear();
    if (hasItems and pInfos and !pInfos->intruders.empty())
    {
        const PathList::const_iterator end = pInfos->intruders.end();
        for (PathList::const_iterator i = pInfos->intruders.begin(); i != end; ++i)
        {
            *(pListbox) += new Component::HTMLListbox::Item::PathListItem(i->first, i->second);
            hasAtLeastOneItem = true;
        }
        pBtnGo->Enable(true);
    }
    else
    {
        *(pListbox) += new Component::HTMLListbox::Item::Info(wxT("The folder is clean."));
        pBtnGo->Enable(false);
    }

    if (hasAtLeastOneItem and pInfos)
    {
        const double totalSize = (double)(pInfos->totalSize()) / 1024. / 1024.;
        pLblInfos->SetLabel(wxString::Format(
          wxT("Total size : %.1f Mb  (%u files)"), totalSize, pInfos->intruders.size()));

        GetSizer()->Layout();
        pLblInfos->Refresh();
    }
    else
        pLblInfos->SetLabel(wxEmptyString);

    if (pThread)
    {
        pMutex.unlock();
        pThread->stop(1000);
        pMutex.lock();

        delete pThread;
        pThread = nullptr;
    }

    GetSizer()->Show(pBtnCancel, true, true);
    GetSizer()->Show(pBtnGo, true, true);
    GetSizer()->Show(pBtnRefresh, true, true);
    GetSizer()->Layout();
    pListbox->Refresh();

    pMutex.unlock();
}

void StudyCleaner::progress(uint count)
{
    Yuni::MutexLocker locker(pMutex);
    pProgressionCount = count;
}

} // namespace Window
} // namespace Antares
