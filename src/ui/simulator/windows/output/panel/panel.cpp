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

#include "panel.h"
#include <wx/sizer.h>
#include "../../../toolbox/create.h"
#include "../../../toolbox/resources.h"
#include "../../../toolbox/execute/execute.h"
#include "../output.h"
#include "../../../application/study.h"
#include "../../../application/main.h"
#include <ui/common/component/panel/group.h>
#include <ui/common/dispatcher/gui.h>
#include <ui/common/lock.h>
#include "../../../application/wait.h"

#include <memory>

using namespace Yuni;

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
Yuni::Atomic::Int<32> Panel::pPanelsInCallingLoadDataFromFile = 0;

namespace // anonymous
{
static std::map<YString, std::shared_ptr<Yuni::Mutex>> mutexForFiles;
static Yuni::Mutex mutexToAccessToLockFiles;

static inline void DefaultBckColour(Antares::Component::Panel* panel)
{
    Antares::Component::PanelGroup::SetDarkBackgroundColor(panel);
}

static inline void CreateVerticalSeparator(Panel* parent, wxBoxSizer* sizer)
{
    auto* separator = new Antares::Component::Panel(parent);
    separator->SetSize(2, 40);
    DefaultBckColour(separator);
    sizer->Add(separator, 0, wxALL | wxEXPAND);
    sizer->SetItemMinSize(separator, 2, 40);
}

static inline void CreateHorizontalSeparator(Panel* parent, wxBoxSizer* sizer)
{
    auto* separator = new Antares::Component::Panel(parent);
    separator->SetSize(40, 2);
    DefaultBckColour(separator);
    sizer->Add(separator, 0, wxALL | wxEXPAND);
    sizer->SetItemMinSize(separator, 40, 2);
}

} // anonymous namespace

Panel::Panel(OutputViewerComponent* component, wxWindow* parent) :
 Antares::Component::Panel(parent),
 pComponent(component),
 pLayer(nullptr),
 pIndex((uint)-1),
 pSizer(nullptr),
 pLabelMessage(nullptr),
 pIconMessage(nullptr),
 pButton(nullptr),
 pShouldRebuildMessage(true),
 pData(nullptr)
{
    assert(parent && "invalid parent");

    wxBoxSizer* vz = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);

    CreateHorizontalSeparator(this, vz);
    vz->Add(hz, 1, wxALL | wxEXPAND);
    CreateHorizontalSeparator(this, vz);

    // Left - Separator
    CreateVerticalSeparator(this, hz);

    pSizer = new wxBoxSizer(wxVERTICAL);
    hz->Add(pSizer, 1, wxALL | wxEXPAND);

    // Right - Separator
    // CreateVerticalSeparator(this, hz);

    SetSizer(vz);
}

Panel::~Panel()
{
    assert(GetSizer() && "Invalid sizer");

    if (pData)
    {
        delete pData;
        pData = nullptr;
    }

    pLayer = nullptr;
    pComponent = nullptr;

    // Remove the sizerz
    pSizer = nullptr;
    pFilename.clear();

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Panel::index(uint i)
{
    pIndex = i;
}

void Panel::layer(Layer* newLayer, bool forceUpdate)
{
    if (!forceUpdate && pLayer == newLayer)
        return;

    pLayer = newLayer;
    forceRefresh();
}

void Panel::forceRefresh()
{
    assert(this && "Invalid this");
    assert(wxIsMainThread() == true && "Must be ran from the main thread");
    assert(pSizer && "Invalid sizer");

    if (not pComponent)
        return;

    // Memory fence
    WIP::Locker wipLocker;
    GUILocker locker;

    // Data cleanup
    pFilename.clear();
    delete pData;
    pData = nullptr;

    if (!pLayer)
    {
        clearAllComponents();
        auto* sizer = GetSizer();
        if (sizer)
            sizer->Layout();
        return;
    }
    if (pLayer->isVirtual())
    {
        // Notifying the user
        message(wxT("waiting for data..."), "images/48x48/refresh.png");

        // This panel has to display a virtual view of the output
        //
        // This call must be delayed. To perform this operation it is mandatory
        // to know all filenames to analyze, and it will be only known
        // after all panels have prepared their data.
        // arbitrary timeout, the execution may still be delayed again
        // if some panel are not ready
        Antares::Dispatcher::GUI::Post(this, &Panel::loadVirtualLayer, 20);
    }
    else
    {
        // This panel has to display a real file from the output
        assert(pLayer);
        assert(!(!pLayer->selection));
        auto selectionType = pComponent->pCurrentSelectionType;
        if (pLayer->detached)
            selectionType = pLayer->customSelectionType;

        switch (selectionType)
        {
        case Antares::Window::OutputViewer::stArea:
        case Antares::Window::OutputViewer::stLink:
        {
            // Notifying the user
            // It is important that the method loadDataFromFile is called
            // on the next iteration of the main loop. Otherwise it may happen
            // that the data will be already loaded when the 'loading' message
            // will be displayed (especially when the host has several CPUs)
            ++pPanelsInCallingLoadDataFromFile;
            message(wxT("loading output data..."), "images/48x48/refresh.png");
            Yuni::Bind<void()> callback;
            callback.bind(this, &Panel::loadDataFromFile);

            // Dispatch the loading event
            // Using an arbitrary (but short) timeout for delaying the execution
            // of the event. This delay should be enough to guarantee the execution
            // after all GUI events
            Antares::Dispatcher::GUI::Post(callback, 10);
            break;
        }
        case Antares::Window::OutputViewer::stNone:
        {
            message(wxT("Please select an item"));
            break;
        }
        case Antares::Window::OutputViewer::stLogs:
        {
            message(wxT("not implemented"));
            break;
        }
        case Antares::Window::OutputViewer::stSummary:
        {
            message(wxT("not implemented"));
            break;
        }
        case Antares::Window::OutputViewer::stComments:
        {
            message(wxT("not implemented"));
            break;
        }
        }
    }

    auto* sizer = GetSizer();
    if (sizer)
        sizer->Layout();
}

void Panel::messageMergeYbY()
{
    assert(this && "Invalid this");
    assert(pSizer && "Invalid sizer");
    assert(GetSizer() && "Invalid sizer");
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (pLayer && pLayer->isVirtual())
    {
        noData();
        return;
    }

    clearAllComponents();

    const char* const image = "images/64x64/concat.png";
    wxString msg = wxT("Output files scan required");

    wxWindow* lbl = Antares::Component::CreateLabel(this, msg);
    wxStaticBitmap* sbmp = Antares::Resources::StaticBitmapLoadFromFile(this, wxID_ANY, image);
    pSizer->AddStretchSpacer();
    if (sbmp)
    {
        pSizer->Add(sbmp, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
        pSizer->AddSpacer(12);
    }
    if (lbl)
    {
        pSizer->Add(lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
        pSizer->AddSpacer(10);
    }

    wxButton* button
      = Antares::Component::CreateButton(this, wxT("Proceed"), this, &Panel::onProceed);
    pSizer->Add(button, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN);
    pButton = button;

    pSizer->AddStretchSpacer();
    pLabelMessage = lbl;
    pIconMessage = sbmp;
    pShouldRebuildMessage = true;

    Antares::Dispatcher::GUI::Refresh(this);
}

void Panel::message(const wxString& msg, const char* image)
{
    assert(this && "Invalid this");
    assert(pSizer && "Invalid sizer");
    assert(GetSizer() && "Invalid sizer");
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (!pLabelMessage || pShouldRebuildMessage)
    {
        clearAllComponents();
        auto* lbl = Antares::Component::CreateLabel(this, msg);
        auto* sbmp = Antares::Resources::StaticBitmapLoadFromFile(this, wxID_ANY, image);
        pSizer->AddStretchSpacer();
        if (sbmp)
        {
            pSizer->Add(sbmp, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);
            pSizer->AddSpacer(8);
        }
        if (lbl)
            pSizer->Add(lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL);

        pSizer->AddStretchSpacer();
        pLabelMessage = lbl;
        pIconMessage = sbmp;
        pButton = nullptr;
        pShouldRebuildMessage = false;
    }
    else
    {
        if (pIconMessage)
        {
            auto* bmp = Antares::Resources::BitmapLoadFromFile(image);
            if (bmp)
            {
                pIconMessage->SetBitmap(*bmp);
                delete bmp;
            }
            else
                pIconMessage->SetBitmap(wxBitmap());
        }
        pLabelMessage->SetLabel(msg);
    }

    // Refresh
    Refresh();
}

void Panel::clearAllComponents()
{
    assert(this && "Invalid this");

    pLabelMessage = nullptr;
    pIconMessage = nullptr;
    pButton = nullptr;
    if (pSizer)
        pSizer->Clear(true);
    pShouldRebuildMessage = true;
}

void Panel::onProceed(void*)
{
    assert(this && "Invalid this");
    if (!pLabelMessage || !pButton)
        return;
    if (not Data::Study::Current::Valid())
        return;

    pButton->Enable(false);
    pButton->SetLabel(wxT("running"));
    pButton->Hide();
    pLabelMessage->Enable(false);
    pLabelMessage->SetLabel(wxT("The data are being prepared"));
    if (pSizer)
        pSizer->Layout();
    Refresh();

    Yuni::Bind<void()> callback;
    callback.bind(this, &Panel::runMerge);

    // dispatch the merge
    // arbitrary timeout, to let the GUI update itself
    Antares::Dispatcher::GUI::Post(callback, 20 /*ms*/);
}

void Panel::runMerge()
{
    // Resetting statistics
    Statistics::Reset();

    executeAggregator();

    // Reload data
    Yuni::Bind<void()> callback;
    callback.bind(this, &Panel::forceRefresh);
    Antares::Dispatcher::GUI::Post(callback, 10 /**/);

    // Checking for orphan swap files
    Forms::ApplWnd* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->timerCleanSwapFiles(100 /*ms*/);
}

void Panel::executeAggregator()
{
    if (not pComponent || not Data::Study::Current::Valid())
        return;

    // Where is our program ?
    String exeLocation;
    if (not Solver::FindYearByYearAggregator(exeLocation))
    {
        logs.error() << "Impossible to find the program `antares-yby-aggregator`.";
        return;
    }
    logs.info() << "  Found Binary: `" << exeLocation << '`';
    logs.info();

    String cmd;
    AppendWithQuotes(cmd, exeLocation);

    // Area / Link
    auto selectionType = pComponent->pCurrentSelectionType;
    if (pLayer->detached)
        selectionType = pLayer->customSelectionType;

    String currentAreaOrLink = pComponent->pCurrentAreaOrLink;
    if (pLayer->detached)
        currentAreaOrLink = pLayer->customAreaOrLink;

    switch (selectionType)
    {
    case Antares::Window::OutputViewer::stArea:
    {
        cmd << ' ';
        String name;
        name << "--area=" << currentAreaOrLink;
        AppendWithQuotes(cmd, name);
        break;
    }
    case Antares::Window::OutputViewer::stLink:
    {
        currentAreaOrLink.replace(" - ", ",");
        String name;
        name << "--link=" << currentAreaOrLink;
        cmd << ' ';
        AppendWithQuotes(cmd, name);
        break;
    }
    default:
    {
        logs.error() << "invalid selection";
        return;
    }
    }

    // Variable
    cmd << " -c ";
    AppendWithQuotes(cmd, pComponent->pCurrentVariable);

    // The current output
    assert(!(!pLayer->selection));
    Antares::Data::Output& output = *(pLayer->selection);
    cmd << " -i ";
    AppendWithQuotes(cmd, output.path);

    // Data type
    cmd << " -d values -d details -d id";
    // Time interval
    cmd << " -t hourly -t daily -t weekly -t monthly -t annual";

    // Temp folder
    String cacheFolder = Antares::memory.cacheFolder();
    if (not cacheFolder.empty())
    {
        if (cacheFolder.last() == '\\' || cacheFolder.last() == '/')
            cacheFolder.removeLast();
        if (not cacheFolder.empty())
        {
            cmd << " --swap-folder=";
            AppendWithQuotes(cmd, cacheFolder);
        }
    }

    auto* exec = new Toolbox::Process::Execute();
    exec->title(wxT("Extracting data from the output"));
    exec->subTitle(wxT("Running..."));
    exec->icon("images/32x32/run.png");

    logs.debug() << "run " << cmd;
    wxString shellCmd = wxStringFromUTF8(cmd);
    exec->command(shellCmd);

    // Running the simulation - it may take some time
    exec->run();

    // Releasing
    delete exec;
}

std::shared_ptr<Yuni::Mutex> ProvideLockingForFileLocking(const YString& filename)
{
    Yuni::MutexLocker locker(mutexToAccessToLockFiles);
    auto& ptr = mutexForFiles[filename];
    if (!ptr)
        ptr = std::make_shared<Yuni::Mutex>();
    return ptr;
}

void ClearAllMutexForFileLocking()
{
    Yuni::MutexLocker locker(mutexToAccessToLockFiles);
    mutexForFiles.clear();
}

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares
