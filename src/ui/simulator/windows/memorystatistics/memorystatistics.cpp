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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <yuni/core/system/memory.h>
#include <yuni/thread/thread.h>
#include "memorystatistics.h"

#include <antares/study/study.h>
#include "../../application/study.h"
#include "../../toolbox/create.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/system/diskfreespace.hxx"
#include <ui/common/component/panel.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/wupdlock.h>

#include <yuni/core/system/cpu.h>

using namespace Yuni;

namespace Antares
{
namespace Private
{
namespace Window
{
class MemoryStatisticsData final
{
public:
    MemoryStatisticsData() :
     stTxtTotal(nullptr),
     stTxtDiskFree(nullptr),
     stTxtDataDisplay(nullptr),
     stTxtMemoryCache(nullptr),
     timer(nullptr)
    {
    }

    ~MemoryStatisticsData();

public:
    wxStaticText* stTxtTotal;
    wxStaticText* stTxtDiskFree;
    wxStaticText* stTxtDataDisplay;
    wxStaticText* stTxtMemoryCache;
    wxTimer* timer;

}; // class MemoryStatisticsData

MemoryStatisticsData::~MemoryStatisticsData()
{
    if (timer)
    {
        timer->Stop();
        delete timer;
    }
}

} // namespace Window
} // namespace Private
} // namespace Antares


namespace Antares::Window
{
namespace // anonymous
{
class ResourcesInfoTimer final : public wxTimer
{
public:
    ResourcesInfoTimer(MemoryStatistics& form) : wxTimer(), pForm(form)
    {
    }
    virtual ~ResourcesInfoTimer()
    {
    }

    void Notify() final
    {
        pForm.refreshInformation();
    }

private:
    MemoryStatistics& pForm;
};

static inline uint64_t NormalizeAmountOfMemory(uint64_t v)
{
    v *= 10u;
    v /= 1024u * 1024u;
    v /= 10u;
    return v;
}

} // anonymous namespace

MemoryStatistics::MemoryStatistics(wxWindow* parent) :
 wxDialog(parent, wxID_ANY, wxT("Resources monitor"), wxDefaultPosition, wxDefaultSize),
 pDisplayLogsOnce(false)
{
    // Allocating internal data
    pData = new Antares::Private::Window::MemoryStatisticsData();

    // Informations about the study
    auto study = GetCurrentStudy();

    wxColour defaultBgColor = GetBackgroundColour();
    SetBackgroundColour(wxColour(255, 255, 255));

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddSpacer(10);

    auto* gridSizer = new wxFlexGridSizer(3, 4, 10);
    gridSizer->AddGrowableCol(1, 1);

    // Space
    gridSizer->AddSpacer(2);
    {
        auto* st = Component::CreateLabel(this, wxT("Memory"), true);
        gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    }
    {
        auto* st = Component::CreateLabel(this, wxT("    Disk"), true);
        gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    }

    // Space
    {
        auto* st = new wxStaticText(
          this, wxID_ANY, wxT("Interface  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
        wxFont f = st->GetFont();
        f.SetWeight(wxFONTWEIGHT_BOLD);
        st->SetFont(f);
        gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        gridSizer->AddSpacer(5);
        gridSizer->AddSpacer(5);
    }

    // Data display
    auto* stTxtDataDisplayTtl = new wxStaticText(this,
                                                 wxID_ANY,
                                                 wxT("Currently used for data display : "),
                                                 wxDefaultPosition,
                                                 wxDefaultSize,
                                                 wxALIGN_RIGHT);
    gridSizer->Add(stTxtDataDisplayTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    pData->stTxtDataDisplay
      = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    gridSizer->Add(pData->stTxtDataDisplay, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    pData->stTxtMemoryCache
      = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    gridSizer->Add(pData->stTxtMemoryCache, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    // Space
    {
        auto* st = new wxStaticText(
          this, wxID_ANY, wxT("System  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
        wxFont f = st->GetFont();
        f.SetWeight(wxFONTWEIGHT_BOLD);
        st->SetFont(f);
        gridSizer->Add(st, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        gridSizer->AddSpacer(5);
        gridSizer->AddSpacer(5);
    }

    // Total available memory
    auto* stTxtTotalTtl = new wxStaticText(this,
                                           wxID_ANY,
                                           wxT("Currently available on the system : "),
                                           wxDefaultPosition,
                                           wxDefaultSize,
                                           wxALIGN_RIGHT);
    gridSizer->Add(stTxtTotalTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    pData->stTxtTotal = new wxStaticText(
      this, wxID_ANY, wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    gridSizer->Add(pData->stTxtTotal, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    pData->stTxtDiskFree = new wxStaticText(
      this, wxID_ANY, wxT("   Updating..."), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    gridSizer->Add(pData->stTxtDiskFree, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    // Machine number of cores
    {
        gridSizer->AddSpacer(10);
        gridSizer->AddSpacer(10);
        gridSizer->AddSpacer(10);

        auto* stTxtMachineNbCoresTtl = new wxStaticText(
          this, wxID_ANY, wxT("CPU cores :"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
        wxFont f = stTxtMachineNbCoresTtl->GetFont();
        f.SetWeight(wxFONTWEIGHT_BOLD);
        stTxtMachineNbCoresTtl->SetFont(f);
        gridSizer->Add(
          stTxtMachineNbCoresTtl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

        wxString s;
        s << Yuni::System::CPU::Count();
        wxStaticText* stTxtMachineNbCores = new wxStaticText(
          this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
        stTxtMachineNbCores->SetLabel(s);
        gridSizer->Add(stTxtMachineNbCores, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    auto* hz = new wxBoxSizer(wxHORIZONTAL);
    hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/64x64/cpu.png"),
            0,
            wxALL | wxALIGN_TOP | wxALIGN_CENTER);
    hz->Add(gridSizer, 1, wxALL | wxEXPAND);
    sizer->Add(hz, 1, wxALL | wxEXPAND, 20);

    auto* panel = new Component::Panel(this);
    auto* sizerBar = new wxBoxSizer(wxHORIZONTAL);
    sizerBar->AddStretchSpacer();
    panel->SetSizer(sizerBar);
    panel->SetBackgroundColour(defaultBgColor);

    // Button Close
    {
        auto* btnClose
          = Component::CreateButton(panel, wxT("   Close   "), this, &MemoryStatistics::onClose);
        sizerBar->Add(btnClose, 0, wxFIXED_MINSIZE | wxALIGN_CENTRE_VERTICAL | wxALL, 8);
        sizerBar->Add(15, 5);
        btnClose->SetDefault();
        btnClose->SetFocus();
    }

    sizer->AddSpacer(15);
    sizer->Add(new wxStaticLine(this), 0, wxALL | wxEXPAND);
    sizer->Add(panel, 0, wxALL | wxEXPAND);

    sizer->Layout();
    SetSizer(sizer);
    sizer->Fit(this);
    Centre(wxBOTH);

    refreshInformation();

    pData->timer = new ResourcesInfoTimer(*this);
    pData->timer->Start(100);
}

MemoryStatistics::~MemoryStatistics()
{
    delete pData;

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void MemoryStatistics::refreshInformation()
{
    assert(pData);
    wxString s;

    // Prevent refreshing
    wxWindowUpdateLocker updater(this);

    if (pData->timer)
        pData->timer->Stop();

    // Memory available on the system
    {
        const uint64_t memFree = System::Memory::Available() / (1024 * 1024);
        s.clear();
        s << memFree << wxT(" Mo");
        pData->stTxtTotal->SetLabel(s);
    }

    // The study
    auto study = GetCurrentStudy();

    // Disk free
    {
#ifdef YUNI_OS_WINDOWS
        if (!(!study) and not study->folder.empty())
        {
            s.clear();
            CString<5, false> driveletter;
            driveletter += study->folder.string()[0];
            s << wxStringFromUTF8(driveletter) << wxT(": ");

            uint64_t diskFree = DiskFreeSpace(study->folder.string());
            if (diskFree == (uint64_t)-1)
                s << wxT("N/A");
            else
                s << (diskFree / (1024 * 1024)) << wxT(" Mo");
            pData->stTxtDiskFree->SetLabel(s);
        }
        else
        {
            uint64_t diskFree = DiskFreeSpace("C:\\");
            s.clear();
            s << wxT("C: ") << (diskFree / (1024 * 1024)) << wxT(" Mo");
            pData->stTxtDiskFree->SetLabel(s);
        }
#else
        // TODO linux implementation is missing
        (void)DiskFreeSpace(nullptr); // remove compilation warning
        pData->stTxtDiskFree->SetLabel(wxT("N/A"));
#endif
    }

    GetSizer()->Layout();
    if (pData->timer)
        pData->timer->Start(3000);
}

void MemoryStatistics::onClose(void*)
{
    // ASync close
    Dispatcher::GUI::Close(this);
}

} // namespace Antares::Window

