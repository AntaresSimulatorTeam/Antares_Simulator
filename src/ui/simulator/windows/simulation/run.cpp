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

#include "run.h"
#include <yuni/core/math.h>
#include <yuni/core/system/memory.h>
#include <yuni/io/directory/system.h>
#include <yuni/core/system/process.h>
#include <yuni/io/file.h>
#include <antares/study/parameters.h>
#include <antares/study/memory-usage.h>

#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/statline.h>
#include <wx/utils.h>
#include <wx/checkbox.h>

#include "../../toolbox/components/wizardheader.h"
#include "../../toolbox/components/button.h"
#include <ui/common/component/panel.h>

#include "../../toolbox/validator.h"
#include "../../toolbox/create.h"
#include <antares/solver.h>
#include "../../application/study.h"
#include "../../application/main/main.h"
#include "../../application/menus.h"
#include "../../windows/message.h"
#include "../../toolbox/system/diskfreespace.hxx"
#include <antares/config.h>
#include <antares/Enum.hpp>

#include <solver/utils/ortools_utils.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Simulation
{
enum
{
    featuresCount = 2,
    timerInterval = 3500 // ms
};

static const wxString featuresNames[featuresCount]
  = {wxT(" Default  "), wxT(" Parallel ")};

static const Solver::Feature featuresAlias[featuresCount]
  = {Solver::standard, Solver::parallel};

static wxString TimeSeriesToWxString(uint m)
{
    if (!m)
        return wxT("none");

    wxString r;
    if (m & Data::timeSeriesLoad)
        r /*<< (r.empty() ? wxEmptyString : wxT(", "))*/ << wxT("load");
    if (m & Data::timeSeriesHydro)
        r << (r.empty() ? wxEmptyString : wxT(", ")) << wxT("hydro");
    if (m & Data::timeSeriesWind)
        r << (r.empty() ? wxEmptyString : wxT(", ")) << wxT("wind");
    if (m & Data::timeSeriesThermal)
        r << (r.empty() ? wxEmptyString : wxT(", ")) << wxT("thermal");
    return r.empty() ? wxT("none") : r;
}

static inline void UpdateLabel(bool& guiUpdated, wxStaticText* label, const wxString& text)
{
    if (text != label->GetLabel())
    {
        label->SetLabel(text);
        guiUpdated = true;
    }
}

class ResourcesInfoTimer final : public wxTimer
{
public:
    ResourcesInfoTimer(Run& form) : wxTimer(), pForm(form)
    {
    }
    virtual ~ResourcesInfoTimer()
    {
    }

    void Notify() override
    {
        pForm.estimateMemoryUsage();
    }

private:
    Run& pForm;
};

void Run::gridAppend(wxFlexGridSizer& sizer,
                     const wxString& title,
                     const wxString& key,
                     wxWindow* value,
                     bool bold)
{
    if (title.empty())
    {
        sizer.AddStretchSpacer();
    }
    else
    {
        auto* t = Antares::Component::CreateLabel(
          pBigDaddy, wxString(wxT("   ")) << title << wxT("       "), true);
        t->Enable(false);
        sizer.Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    }

    // Key
    wxStaticText* keyLbl = Antares::Component::CreateLabel(pBigDaddy, key, bold);
    sizer.Add(keyLbl, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    // Value
    sizer.Add(value, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
}

void Run::gridAppend(wxFlexGridSizer& sizer,
                     const wxString& title,
                     const wxString& key,
                     const wxString& value)
{
    wxStaticText* lbl = Antares::Component::CreateLabel(pBigDaddy, value);
    lbl->Wrap(300);
    gridAppend(sizer, title, key, lbl);
}

void Run::gridAppend(wxFlexGridSizer& sizer, const wxString& key, wxWindow* value)
{
    gridAppend(sizer, wxEmptyString, key, value);
}

void Run::gridAppend(wxFlexGridSizer& sizer, wxWindow* key, wxWindow* value)
{
    sizer.AddStretchSpacer();

    // Key
    sizer.Add(key, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    // Value
    sizer.Add(value, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
}

void Run::gridAppend(wxFlexGridSizer& sizer, wxWindow* key, wxSizer* value)
{
    sizer.AddStretchSpacer();

    // Key
    sizer.Add(key, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    // Value
    sizer.Add(value, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
}

void Run::gridAppend(wxFlexGridSizer& sizer, const wxString& key, const wxString& value)
{
    gridAppend(sizer, wxEmptyString, key, value);
}

Run::Run(wxWindow* parent, bool preproOnly) :
 wxDialog(parent, wxID_ANY, wxString(wxT("Simulation"))),
 pSimulationName(nullptr),
 pSimulationComments(nullptr),
 pIgnoreWarnings(nullptr),
 pTimer(nullptr),
 pWarnAboutMemoryLimit(false),
 pWarnAboutDiskLimit(false),
 pAlreadyWarnedNoMCYear(false),
 pFeatureIndex(0)
{
    assert(parent);

    // Informations about the study
    auto& study = *Data::Study::Current::Get();

    pThread = study.createThreadToEstimateInputMemoryUsage();
    pThread->start();

    // The main sizer
    auto* mnSizer = new wxBoxSizer(wxVERTICAL);

    // Header
    mnSizer->Add(Toolbox::Components::WizardHeader::Create(
                   this,
                   wxT("Run a simulation..."),
                   "images/32x32/run.png",
                   wxString(wxT("For the study `"))
                     << wxStringFromUTF8(study.header.caption) << wxT("`, ")
                     << wxStringFromUTF8(Data::StudyModeToCString(study.parameters.mode))),
                 0,
                 wxALL | wxEXPAND | wxFIXED_MINSIZE);

    pBigDaddy = new Component::Panel(this);

    // Grid
    wxFlexGridSizer* s = new wxFlexGridSizer(3, 1, 0);

    // Space
    s->AddSpacer(8);
    s->AddSpacer(8);
    s->AddSpacer(8);

    // Informations about the INPUT
    {
        auto* lblMode
          = Antares::Component::CreateLabel(pBigDaddy, wxStringFromUTF8(study.header.caption));
        wxFont f = lblMode->GetFont();
        f.SetWeight(wxFONTWEIGHT_BOLD);
        lblMode->SetFont(f);
        gridAppend(*s, wxT("INPUT"), wxT("Study : "), lblMode, true);
    }
    {
        auto* lblMode = Antares::Component::CreateLabel(
          pBigDaddy, wxStringFromUTF8(Data::StudyModeToCString(study.parameters.mode)));
        wxFont f = lblMode->GetFont();
        f.SetWeight(wxFONTWEIGHT_BOLD);
        lblMode->SetFont(f);
        gridAppend(*s, wxT("Mode : "), lblMode);
    }

    pMonteCarloYears = Antares::Component::CreateLabel(pBigDaddy, wxEmptyString, true);
    gridAppend(*s, wxT("Year(s) of Monte Carlo : "), pMonteCarloYears);
    gridAppend(*s,
               wxT("Time-series generator(s) : "),
               TimeSeriesToWxString(study.parameters.timeSeriesToGenerate));

    // Space
    s->AddSpacer(15);
    s->AddSpacer(15);
    s->AddSpacer(15);

    // Informations about the OUTPUT
    pSimulationName = new wxTextCtrl(pBigDaddy,
                                     wxID_ANY,
                                     wxEmptyString,
                                     wxDefaultPosition,
                                     wxSize(360, -1),
                                     0,
                                     Toolbox::Validator::Default());
    gridAppend(*s, wxT("OUTPUT"), wxT("Simulation name : "), pSimulationName, true);

    if (false)
    {
        pSimulationComments = new wxTextCtrl(pBigDaddy,
                                             wxID_ANY,
                                             wxEmptyString,
                                             wxDefaultPosition,
                                             wxSize(360, 70),
                                             wxTE_MULTILINE | wxTE_PROCESS_TAB);
        gridAppend(*s, wxT("Comments : "), pSimulationComments);
    }

// Run the preprocessors only
#ifdef YUNI_OS_WINDOWS
    s->AddSpacer(2); // for beauty
    s->AddSpacer(2);
    s->AddSpacer(2);
#endif

    pPreproOnly = new wxCheckBox(
      pBigDaddy, wxID_ANY, wxString(wxT(" Run the time-series generators only  ")));
    pPreproOnly->SetValue(preproOnly);
    pPreproOnly->Connect(pPreproOnly->GetId(),
                         wxEVT_COMMAND_CHECKBOX_CLICKED,
                         wxCommandEventHandler(Run::evtOnPreprocessorsOnlyClick),
                         nullptr,
                         this);
    gridAppend(*s, wxEmptyString, pPreproOnly);

    {
        s->AddSpacer(15); // for beauty
        s->AddSpacer(15);
        s->AddSpacer(15);
    }

    // Space
    s->AddSpacer(15);
    s->AddSpacer(15);
    s->AddSpacer(15);

    // features
    auto* btn = new Component::Button(pBigDaddy, featuresNames[0], "images/16x16/run.png");
    btn->menu(true);
    btn->onPopupMenu(this, &Run::prepareMenuSolverMode);
    gridAppend(*s, wxT("RESOURCES"), wxT("Solver : "), btn);
    pBtnMode = btn;
    pFeatureIndex = 0;

    // Ortools use
    {
        // Ortools use
        auto* ortoolsCheckBox = new wxCheckBox(pBigDaddy, wxID_ANY, wxT(""));
        ortoolsCheckBox->SetValue(false);

        Connect(ortoolsCheckBox->GetId(),
                wxEVT_COMMAND_CHECKBOX_CLICKED,
                wxCommandEventHandler(Run::onOrtoolsCheckboxChanged));
        pOrtoolsCheckBox = ortoolsCheckBox;

        // Ortools solver selection
        pTitleOrtoolsSolverCombox
          = Antares::Component::CreateLabel(pBigDaddy, wxT("Ortools solver : "));

        pOrtoolsSolverCombox = new wxComboBox(pBigDaddy, wxID_ANY);
        std::list<std::string> ortoolsSolverList = OrtoolsUtils().getAvailableOrtoolsSolverName();
        for (const std::string& ortoolsSolver : ortoolsSolverList)
        {
            pOrtoolsSolverCombox->Append(ortoolsSolver);
        }

        // Ortools solver selection visibility
        pTitleOrtoolsSolverCombox->Show(pOrtoolsCheckBox->GetValue());
        pOrtoolsSolverCombox->Show(pOrtoolsCheckBox->GetValue());

        // Display 2 rows for ortools option
        gridAppend(*s, wxT("Ortools use : "), ortoolsCheckBox);
        gridAppend(*s, pTitleOrtoolsSolverCombox, pOrtoolsSolverCombox);
    }

    // When opening the Run window, the solver mode is default.
    // Therefore, the number of cores must be set (back) to the value associated with default mode
    // (== 1).
    uint& minNbCores = Data::Study::Current::Get()
                         ->minNbYearsInParallel; // For run window's simulation cores field.
    uint& maxNbCores = Data::Study::Current::Get()->maxNbYearsInParallel; // For RAM estimation
    minNbCores = 1;
    maxNbCores = 1;

    {
        wxStaticText* title;
        wxSizer* sizer;

        sizer = new wxBoxSizer(wxHORIZONTAL);
        pTitleSimCores = Antares::Component::CreateLabel(pBigDaddy, wxT("Simulation cores : "));
        pNbCores = Antares::Component::CreateLabel(pBigDaddy, wxEmptyString);
        sizer->AddSpacer(10);
        sizer->Add(pNbCores, 0, wxALL | wxEXPAND);
        sizer->AddSpacer(10);
        gridAppend(*s, pTitleSimCores, sizer);

        // "Simulation cores" field default behavior (by default, pFeatureIndex == 0)
        pTitleSimCores->Hide();
        pNbCores->Hide();

        sizer = new wxBoxSizer(wxHORIZONTAL);
        title = Antares::Component::CreateLabel(pBigDaddy, wxT("Memory (estimation) : "));
        pLblEstimation = Antares::Component::CreateLabel(pBigDaddy, wxEmptyString);
        pLblEstimationAvailable
          = Antares::Component::CreateLabel(pBigDaddy, wxEmptyString, false, true);
        sizer->Add(pLblEstimation, 0, wxALL | wxEXPAND);
        sizer->AddSpacer(10);
        sizer->Add(pLblEstimationAvailable, 0, wxALL | wxEXPAND);
        gridAppend(*s, title, sizer);

        sizer = new wxBoxSizer(wxHORIZONTAL);
        title = Antares::Component::CreateLabel(pBigDaddy, wxT("Disk (estimation) : "));
        pLblDiskEstimation = Antares::Component::CreateLabel(pBigDaddy, wxEmptyString);
        pLblDiskEstimationAvailable
          = Antares::Component::CreateLabel(pBigDaddy, wxEmptyString, false, true);
        sizer->Add(pLblDiskEstimation, 0, wxALL | wxEXPAND);
        sizer->AddSpacer(10);
        sizer->Add(pLblDiskEstimationAvailable, 0, wxALL | wxEXPAND);
        gridAppend(*s, title, sizer);

        pOptionSpacer = s->AddSpacer(13);
    }

    // Buttons
    auto* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
    pIgnoreWarnings = new wxCheckBox(this, wxID_ANY, wxT(" Ignore warnings  "));
    pnlBtns->Add(25, 5);
    pnlBtns->Add(pIgnoreWarnings, 0, wxALL | wxEXPAND);
    pnlBtns->AddStretchSpacer();
    pnlBtns->Add(Component::CreateButton(this, wxT("Cancel"), this, &Run::onCancel));
    pnlBtns->AddSpacer(3);
    pBtnRun = Component::CreateButton(this, wxT("Run the simulation"), this, &Run::onRun);
    pnlBtns->Add(pBtnRun, 0, wxALL | wxEXPAND);
    pnlBtns->AddSpacer(22);

    // Set the sizer
    s->Layout();
    pBigDaddy->SetSizer(s);
    mnSizer->Add(pBigDaddy, 0, wxALL | wxEXPAND, 20);
    mnSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND, 8);
    mnSizer->Add(pnlBtns, 0, wxEXPAND | wxALL);
    mnSizer->AddSpacer(8);
    SetSizer(mnSizer);
    mnSizer->Fit(this);
    CentreOnParent();

    // Update the monte-carlo years
    updateMonteCarloYears();

    pBtnRun->SetDefault();
    pSimulationName->SetFocus();

    updateNbCores();

    estimateMemoryUsage();

    pTimer = new ResourcesInfoTimer(*this);
    pTimer->Start(150);

    // Event
    Connect(GetId(), wxEVT_MOTION, wxMouseEventHandler(Run::onInternalMotion), NULL, this);
}

Run::~Run()
{
    if (pNbCores)
        delete pNbCores;
    if (pTitleSimCores)
        delete pTitleSimCores;

    if (pTimer)
    {
        pTimer->Stop();
        delete pTimer;
        pTimer = nullptr;
    }

    if (!(!pThread))
    {
        pThread->gracefulStop();
        pThread->stop();
        pThread = nullptr;
    }
}

void Run::estimateMemoryUsage()
{
    if (pTimer)
        pTimer->Stop();

    pWarnAboutMemoryLimit = false;
    pWarnAboutDiskLimit = false;

    auto studyptr = Data::Study::Current::Get();
    // The study
    if (!studyptr)
        return;
    auto& study = *studyptr;

    // flag to know if the gui has been updated, to avoid a call to Layout,
    // and to avoid flickering
    bool guiUpdated = false;

    const bool updating = (!pTimer or (pThread->started()));
    if (not updating)
    {
        // Total of memory available on the system
        uint64 memFree = System::Memory::Available();
        uint64 diskFree = DiskFreeSpace(study.folder);

        Data::StudyMemoryUsage m(study);
        if (pPreproOnly->GetValue())
            m.years = 0;

        m.estimate();

        uint64 amountNeeded = m.requiredMemory;
        pWarnAboutMemoryLimit = memFree < amountNeeded;
        pWarnAboutDiskLimit = (diskFree != (uint64)-1)
                              and (not study.folder.empty() and (diskFree < m.requiredDiskSpace));

        wxString s;
        s = wxT("   ~");
        BytesToStringW(s, amountNeeded);
        UpdateLabel(guiUpdated, pLblEstimation, s);

        s = wxT("/  ");
        BytesToStringW(s, memFree) << wxT(" available");
        UpdateLabel(guiUpdated, pLblEstimationAvailable, s);

        s = wxT("  ");
        switch (featuresAlias[pFeatureIndex])
        {
        case Solver::parallel:
        case Solver::standard:
        {
            s << wxT(" < ");
            BytesToStringW(s, m.requiredDiskSpace);
            break;
        }
        default:
            break;
        }
        UpdateLabel(guiUpdated, pLblDiskEstimation, s);

        // Free space
        s.clear();
        if (diskFree != (uint64)-1)
        {
            s = wxT("/  ");
            BytesToStringW(s, diskFree) << wxT(" available");
        }
        UpdateLabel(guiUpdated, pLblDiskEstimationAvailable, s);
    }
    else
    {
        UpdateLabel(guiUpdated, pLblEstimation, wxEmptyString);
        UpdateLabel(guiUpdated, pLblDiskEstimation, wxEmptyString);
        UpdateLabel(guiUpdated, pLblEstimationAvailable, wxT("updating..."));
        UpdateLabel(guiUpdated, pLblDiskEstimationAvailable, wxT("updating..."));
        pWarnAboutMemoryLimit = false;
        pWarnAboutDiskLimit = false;
    }

    // rebuild the layout
    if (guiUpdated)
    {
        auto* sizer = pBigDaddy->GetSizer();
        if (sizer)
            sizer->Layout();
        sizer = GetSizer();
        if (sizer)
            sizer->Layout();
    }

    // Restoring the timer
    if (pTimer)
        pTimer->Start(updating ? 500 : timerInterval);
}

void Run::onCancel(void*)
{
    this->Enable(false);
    if (pTimer)
    {
        pTimer->Stop();
        delete pTimer;
        pTimer = nullptr;
    }
    if (!(!pThread))
    {
        pThread->gracefulStop();
        pThread->wait();
        pThread = nullptr;
    }

    Dispatcher::GUI::Close(this);
}

bool Run::createCommentsFile(String& filename) const
{
    filename.clear();

    if (not pSimulationComments)
        return false;

    wxString cmt = pSimulationComments->GetValue();
    if (cmt.empty())
        return false;

    String content;
    wxStringToString(cmt, content);
    content.trim();

    if (not content.empty())
    {
        content += '\n'; // adding a final cariage return

        String temporary;
        if (not IO::Directory::System::Temporary(temporary))
            return false;

        auto processID = ProcessID();

        String fn;
        uint index = 0;
        do
        {
            fn.clear() << temporary << IO::Separator << "antares-" << processID << "-simu-comment-"
                       << (void*)this << '-' << (++index) << ".txt";

            if (not IO::File::Exists(fn))
            {
                filename = fn;
                return IO::File::SetContent(filename, content);
            }
        } while (index < 10000);
    }
    return false;
}

int Run::checkForLowResources()
{
#ifndef YUNI_OS_WINDOWS
    pWarnAboutDiskLimit = false; // not implemented
#endif

    if (not pWarnAboutMemoryLimit and not pWarnAboutDiskLimit)
        return 0;

    auto& mainFrm = *Forms::ApplWnd::Instance();

    wxString msg;
    if (pWarnAboutMemoryLimit)
    {
        if (pWarnAboutDiskLimit)
            msg = wxT("Memory and Disk");
        else
            msg = wxT("Memory");
    }
    else
        msg = wxT("Disk");
    msg << wxT(" almost full");

    Window::Message message(
      &mainFrm, wxT("Simulation"), msg, wxT("Try anyway ?"), "images/misc/warning.png");
    message.add(Window::Message::btnContinue);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() != Window::Message::btnContinue)
    {
        this->Enable(true);
        return 1;
    }
    return -1;
}

void Run::onRun(void*)
{
    if (not Data::Study::Current::Valid())
        return;

    bool canNotifyUserForLowResources = true;
    switch (checkForLowResources())
    {
    case 1:
        return; // abort
    case -1:
        canNotifyUserForLowResources = false;
        break;
    }

    if (StudyHasBeenModified())
    {
        Window::Message message(this,
                                wxT("Simulation"),
                                wxT("Simulation"),
                                wxT("The study has been modified. All changes must be written to "
                                    "the disk\nbefore launching the simulation."),
                                "images/misc/save.png");
        message.add(Window::Message::btnSaveChanges);
        message.add(Window::Message::btnCancel, true);
        if (message.showModal() == Window::Message::btnCancel)
        {
            this->Enable(true);
            return;
        }

        const SaveResult r = ::Antares::SaveStudy();
        if (!(r == svsDiscard or r == svsSaved))
        {
            this->Enable(true);
            return;
        }
    }

    // Memory limit
    estimateMemoryUsage();

    if (canNotifyUserForLowResources and 1 == checkForLowResources())
        return;

    // Updating the display
    if (pTimer)
        pTimer->Stop();
    if (!(!pThread))
        pThread->stop();
    this->Enable(false);

    Refresh();
    wxTheApp->Yield();

    this->Enable(false);
    if (pTimer)
    {
        pTimer->Stop();
        delete pTimer;
        pTimer = nullptr;
    }
    if (!(!pThread))
    {
        pThread->gracefulStop();
        pThread->stop();
        pThread = nullptr;
    }

    // The Simulation Name
    String simulationName;
    {
        wxString smName = pSimulationName->GetValue();
        wxStringToString(smName, simulationName);
        simulationName.trim();
        pSimulationName->SetValue(wxStringFromUTF8(simulationName));
    }

    String commentFile;
    if (not createCommentsFile(commentFile))
        commentFile.clear();
    else
        logs.debug() << "using comment file: " << commentFile;

    Hide();

    // Run the simulation
    RunSimulationOnTheStudy(Data::Study::Current::Get(),
                            simulationName,
                            commentFile,
                            pIgnoreWarnings->GetValue(),                     // Ignore warnings
                            featuresAlias[pFeatureIndex],                    // Features
                            pPreproOnly->GetValue(),                         // Prepro Only ?
                            pOrtoolsCheckBox->IsChecked(),                   // Ortools use
                            pOrtoolsSolverCombox->GetValue().ToStdString()); // Ortools solver

    // Remove the temporary file
    if (not commentFile.empty())
    {
        IO::File::Delete(commentFile);
    }

    Dispatcher::GUI::Close(this);
}

void Run::evtOnPreprocessorsOnlyClick(wxCommandEvent&)
{
    updateMonteCarloYears();
    estimateMemoryUsage();
}

void Run::updateMonteCarloYears()
{
    assert(pMonteCarloYears);
    assert(pBtnRun);

    if (pPreproOnly->IsChecked())
    {
        pMonteCarloYears->SetLabel(wxT("none (generators only)"));
        pBtnRun->Enable(true);
    }
    else
    {
        if (Data::Study::Current::Valid())
        {
            uint y = Data::Study::Current::Get()->parameters.nbYears;
            if (y)
            {
                pMonteCarloYears->SetLabel(wxString() << y);
                pBtnRun->Enable(true);
            }
            else
            {
                pMonteCarloYears->SetLabel(wxT("0"));
                pBtnRun->Enable(false);
                if (!pAlreadyWarnedNoMCYear)
                {
                    pAlreadyWarnedNoMCYear = true;

                    Window::Message message(
                      this,
                      wxT("Simulation"),
                      wxT("Impossible to launch a simulation"),
                      wxT("The number of MC years is invalid (can not be null)"),
                      "images/misc/error.png");
                    message.add(Window::Message::btnContinue, true);
                    message.showModal();
                }
            }
        }
        else
        {
            pMonteCarloYears->SetLabel(wxT("0"));
            pBtnRun->Enable(false);
        }
    }
}

void Run::updateNbCores()
{
    assert(pNbCores);
    assert(pBtnRun);

    if (Data::Study::Current::Valid())
    {
        // Minimum number of years in a set of parallel years (reduction from raw number of cores
        // chosen by user).
        uint minNbCores = Data::Study::Current::Get()->minNbYearsInParallel;

        // Number of cores before any reduction, that is based on nb of cores level (advanced
        // parameters)
        uint nbCoresRaw = Data::Study::Current::Get()->nbYearsParallelRaw;

        if (minNbCores)
        {
            wxString s = wxT("");
            s << nbCoresRaw;
            if (minNbCores < nbCoresRaw)
                s << L"  (smallest batch size : " << minNbCores << L")";
            pNbCores->SetLabel(s);

            pBtnRun->Enable(true);
        }
        else
        {
            pNbCores->SetLabel(wxT("0"));
            pBtnRun->Enable(false);
            Window::Message message(this,
                                    wxT("Simulation"),
                                    wxT("Impossible to launch a simulation"),
                                    wxT("The number of cores is 0"),
                                    "images/misc/error.png");
            message.add(Window::Message::btnContinue, true);
            message.showModal();
        }
    }
    else
    {
        pNbCores->SetLabel(wxT("0"));
        pBtnRun->Enable(false);
    }
}

void Run::prepareMenuSolverMode(Antares::Component::Button&, wxMenu& menu, void*)
{
    // cleanup
    pMappingSolverMode.clear();

    // Simulation mode is adequacy-draft mode ?
    auto& study = *Data::Study::Current::Get();
    bool draftMode = study.parameters.adequacyDraft();

    for (uint i = 0; i != featuresCount; ++i)
    {
        wxMenuItem* it = Menu::CreateItem(&menu,
                                          wxID_ANY,
                                          featuresNames[i],
                                          "images/16x16/empty.png",
                                          wxEmptyString,
                                          wxITEM_NORMAL,
                                          (i == 0));

        pMappingSolverMode[it->GetId()] = i;
        menu.Connect(it->GetId(),
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(Run::onSelectMode),
                     nullptr,
                     this);

        // In case of adequacy-draft mode, parallel mode is disabled
        if (i == Solver::parallel && draftMode)
            it->Enable(false);
    }
}

void Run::onSelectMode(wxCommandEvent& evt)
{
    pFeatureIndex = pMappingSolverMode[evt.GetId()];
    if (pFeatureIndex >= featuresCount)
        pFeatureIndex = 0;

    // In case of either default mode, MC years parallel computation is disabled (nb
    // of cores is set to 1)

    // Needed For RAM estimation
    uint& maxNbCores = Data::Study::Current::Get()->maxNbYearsInParallel;
    uint maxNbCoresParallelMode = Data::Study::Current::Get()->maxNbYearsInParallel_save;

    // Needed for run window's simulation cores field
    uint& minNbCores = Data::Study::Current::Get()->minNbYearsInParallel;
    uint minNbCoresParallelMode = Data::Study::Current::Get()->minNbYearsInParallel_save;

    if (featuresAlias[pFeatureIndex] == Solver::parallel)
    {
        maxNbCores = maxNbCoresParallelMode; // For RAM estimation
        minNbCores = minNbCoresParallelMode; // For run window's simulation cores field
        pTitleSimCores->Show();
        pNbCores->Show();
        pOptionSpacer->Show(false);
    }
    else
    {
        maxNbCores = 1; // For RAM estimation
        minNbCores = 1; // Not needed (because simulation cores field is hidden here)
        pNbCores->Hide();
        pTitleSimCores->Hide();
        pOptionSpacer->Show(true);
    }

    // Update the estimation of the memory consumption
    estimateMemoryUsage();

    // Update the nb of cores in the Run window
    updateNbCores();

    pBtnMode->caption(featuresNames[pFeatureIndex]);
}

void Run::onInternalMotion(wxMouseEvent&)
{
    Antares::Component::Panel::OnMouseMoveFromExternalComponent();
}

void Run::onOrtoolsCheckboxChanged(wxCommandEvent& WXUNUSED(event))
{
    pTitleOrtoolsSolverCombox->Show(pOrtoolsCheckBox->GetValue());
    pOrtoolsSolverCombox->Show(pOrtoolsCheckBox->GetValue());

    // Layout update
    auto* sizer = pBigDaddy->GetSizer();
    if (sizer)
        sizer->Fit(pBigDaddy);
    sizer = GetSizer();
    if (sizer)
        sizer->Fit(this);
}

} // namespace Simulation
} // namespace Window
} // namespace Antares
