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
#include <ctime>
#include <fstream>
// Status Bar
#include <wx/statusbr.h>

// Rich Text
#include <wx/richtext/richtextxml.h>
#include <wx/richtext/richtexthtml.h>

#include <antares/logs.h>
#include "../../toolbox/resources.h"
#include "../../toolbox/locales.h"
#include "internal-data.h"
#include "../../windows/version.h"
#include "../../config.h"
#include "../../windows/message.h"
#include "../../config.h"
#include "drag-drop.hxx"

// Antares study
#include <antares/study.h>
#include <antares/sys/appdata.h>

// Create toolbox
#include "../../toolbox/create.h"
// Panel
#include <ui/common/component/panel.h>
// Button
#include "../../toolbox/components/button.h"
// Map
#include "../../toolbox/components/map/component.h"
// WIP Panel
#include "../../toolbox/components/wip-panel.h"
// Datagrid
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/gridhelper.h"
#include "../../toolbox/components/datagrid/renderer/connection.h"
#include "../../toolbox/components/datagrid/renderer/area/dsm.h"
#include "../../toolbox/components/datagrid/renderer/area/misc.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include "../../toolbox/components/datagrid/renderer/links/summary.h"
#include "../../toolbox/components/datagrid/renderer/layers.h"
#include "../../toolbox/input/area.h"
#include "../../toolbox/input/connection.h"
#include "../../toolbox/input/bindingconstraint.h"
// MainPanel
#include "../../toolbox/components/mainpanel.h"

// Windows
#include "../../windows/connection.h"
#include "../../windows/simulation/panel.h"
#include "../../windows/thermal/panel.h"
#include "../../windows/correlation/correlation.h"
#include "../../windows/bindingconstraint/bindingconstraint.h"
#include "../../windows/analyzer/analyzer.h"
#include "../../windows/inspector/inspector.h"

// Standard page
#include "build/standard-page.hxx"
// Wait
#include "../wait.h"
// startup wizard
#include "../../windows/startupwizard.h"
#include "../../toolbox/dispatcher/study.h"
// license
#include "check-license-at-startup.hxx"

using namespace Yuni;

namespace Antares
{
namespace Forms
{
class CustomStatusBar : public wxStatusBar
{
public:
    /*!
    ** \brief Get the default status bar text for Antares
    */
    static wxString DefaultText();

public:
    CustomStatusBar(wxWindow* parent) : wxStatusBar(parent)
    {
    }

    virtual ~CustomStatusBar()
    {
    }

}; // class StatusBar

class MemoryFlushTimer : public wxTimer
{
public:
    MemoryFlushTimer() : pDelayedCount()
    {
    }

    void Notify() override
    {
        // Releasing all the memory currently used
        if (Antares::Memory::swapSupport)
        {
            if (CanPerformMemoryFlush())
            {
                Antares::memory.flushAll();
                pDelayedCount = 0;
            }
            else
            {
                if (pDelayedCount++ == 0)
                    logs.info()
                      << "memory flush delayed because a data update is currently performed";
                else
                {
                    if (pDelayedCount > 5)
                        pDelayedCount = 0;
                }
            }
        }
    }

private:
    //! Variable to reduce verbosity on logs
    uint pDelayedCount;

}; // class MemoryFlushTimer

static void CreateWindowToolbar(ApplWnd& mainfrm, wxAuiManager& auimanager)
{
    // Our toolbar
    auto* toolbar = new Component::Panel(&mainfrm);

    // Toolbar
    {
        auto* s = new wxBoxSizer(wxHORIZONTAL);
        auto* t = new wxBoxSizer(wxVERTICAL);
        toolbar->SetSizer(t);
        Antares::Component::Button* btn;

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s, 2);

        btn = new Antares::Component::Button(toolbar,
                                             wxEmptyString,
                                             "images/16x16/wizard.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarWizard);
        s->Add(btn, 0, wxALL | wxEXPAND);

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s);

        btn = new Antares::Component::Button(toolbar,
                                             wxEmptyString,
                                             "images/16x16/new.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarNewStudy);
        s->Add(btn, 0, wxALL | wxEXPAND);
        btn = new Antares::Component::Button(toolbar,
                                             wxT("Open"),
                                             "images/16x16/open.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarOpenLocalStudy);
        btn->dropDown(true);
        btn->onPopupMenu(mainfrm.data(), &MainFormData::onToolbarOpenRecentMenu);
        s->Add(btn, 0, wxALL | wxEXPAND);
        btn = new Antares::Component::Button(toolbar,
                                             wxT("Save"),
                                             "images/16x16/savestudy.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarSave);
        s->Add(btn, 0, wxALL | wxEXPAND);

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s);

        btn = new Antares::Component::Button(toolbar,
                                             wxT("Run a simulation"),
                                             "images/16x16/run-simulation.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarRunSimulation);
        s->Add(btn, 0, wxALL | wxEXPAND);

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s);

        btn = new Antares::Component::Button(toolbar,
                                             wxEmptyString,
                                             "images/16x16/optimization-prefs.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarOptimizationPreferences);
        s->Add(btn, 0, wxALL | wxEXPAND);

        btn = new Antares::Component::Button(toolbar,
                                             wxEmptyString,
                                             "images/16x16/logs.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarLogs);
        s->Add(btn, 0, wxALL | wxEXPAND);

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s);

        btn = new Antares::Component::Button(toolbar,
                                             wxT("Inspector"),
                                             "images/16x16/inspector.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarInspector);
        s->Add(btn, 0, wxALL | wxEXPAND);

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s);

        btn = new Antares::Component::Button(toolbar,
                                             wxEmptyString,
                                             "images/16x16/fullscreen.png",
                                             mainfrm.data(),
                                             &MainFormData::onToolbarFullscreen);
        s->Add(btn, 0, wxALL | wxEXPAND);

        // separator
        Antares::Component::AddVerticalSeparator(toolbar, s);

        s->AddStretchSpacer();

        auto* wip = new Antares::Component::WIPPanel(toolbar);
        mainfrm.data()->wipPanel = wip;
        s->Add(wip, 0, wxALL | wxEXPAND);
        s->SetItemMinSize(wip, 30, 20);

        t->Add(s, 1, wxALL | wxEXPAND, 3);
        t->Layout();
        toolbar->Fit();
    }

    auimanager.AddPane(toolbar,
                       wxAuiPaneInfo()
                         .Name(wxT("form_toolbar"))
                         .Top()
                         .Row(1)
                         .DockFixed(true)
                         .LeftDockable(false)
                         .RightDockable(false)
                         .TopDockable(false)
                         .BottomDockable(false)
                         .PaneBorder(false)
                         .MaximizeButton(false)
                         .GripperTop(false)
                         .CloseButton(false)
                         .CaptionVisible(false)
                         .Floatable(false)
                         .Gripper(false));
    auimanager.Update();
}

void ApplWnd::resetDragAndDrop()
{
    SetDropTarget(new StudyDrop());
}

void ApplWnd::internalInitialize()
{
    // drag & drop
    resetDragAndDrop();

    // Internal data
    pData = new MainFormData(*this);

    // Start the queue manager
    Dispatcher::Start();

    // Logs
    createLogs();
    // Get informations about the current locale
    Locale::Init();

    // Add extra handlers (plain text is automatically added)
    wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    wxRichTextBuffer::AddHandler(new wxRichTextHTMLHandler);
    // Add extra handlers for images
    wxImage::AddHandler(new wxPNGHandler);

    // Icon file for studies
    if (Data::StudyIconFile.empty())
        prepareStudyIconFile();

    // Icon for antares
    // MinSize
    SetMinSize(wxSize(minimalWidth, minimalHeight));

    // Set the default title
    title();

// StatusBar
#if defined(wxUSE_STATUSBAR)
    {
        // auto* statusbar = new Antares::Private::Forms::StatusBar(this);
        // auto* statusbar = new CustomStatusBar(this);
        auto* statusbar = new wxStatusBar(this);
        SetStatusBar(statusbar);

        const int widths[2] = {-1, 180};
        const int styles[2] = {wxSB_FLAT, wxSB_FLAT};

        statusbar->SetFieldsCount(2, widths);
        statusbar->SetStatusStyles(2, styles);

        statusbar->SetMinHeight(14);
        statusbar->Connect(statusbar->GetId(),
                           wxEVT_CONTEXT_MENU,
                           wxContextMenuEventHandler(ApplWnd::evtOnContextMenuStatusBar),
                           nullptr,
                           this);

        statusbar->SetStatusText(wxT("|  "), 1);

        wxFont f = statusbar->GetFont();
        f.SetPointSize(f.GetPointSize() - 1);
        statusbar->SetFont(f);
    }
#endif

    WIP::Locker wip;
    Freeze();
    // A gray background color
    // SetBackgroundColour(wxColour(128, 128, 128));

    pFlushMemoryTimer = new MemoryFlushTimer();

    // The menu for the window
    pMenu = this->createMenu();
    this->SetMenuBar(pMenu);

    // Minimal Component support
    // The main parent
    pBigDaddy = new Component::Panel(this);

    // The notebook used for different sections
    pSectionNotebook = new Component::Notebook(pBigDaddy);
    pSectionNotebook->tabsVisible(false);

    // The main notebook, for INPUT data
    pNotebook = new Component::Notebook(pSectionNotebook);
    pSectionNotebook->add(pNotebook, wxT("input"), wxT("input"));

    // onPageChanged
    pSectionNotebook->onPageChanged.connect(this, &ApplWnd::onSectionNotebookPageChanging);
    pNotebook->onPageChanged.connect(this, &ApplWnd::onMainNotebookPageChanging);

    // The Layers Notebook
    Component::Notebook* layersNotebook
      = new Component::Notebook(pNotebook, Component::Notebook::orTop);
    layersNotebook->displayTitle(false);
    pNotebook->add(layersNotebook, wxT("sys"), wxT("System Maps"));

    pMainMap = new Map::Component(layersNotebook);
    pMainMap->onPopupEvent.connect(this, &ApplWnd::evtOnContextMenuMap);
    pMainMap->onDblClick.connect(this, &ApplWnd::onMapDblClick);

    // The first page
    Component::Notebook::Page* MapPage
      = layersNotebook->add(pMainMap, wxT("map"), wxT("Set Content"));

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> layersUIPage
      = createStdNotebookPage<Toolbox::InputSelector::Area>(
        layersNotebook, wxT("properties"), wxT("Set Visibility"));

    Component::Datagrid::Renderer::LayersUI* areaUIPropertiesRenderer
      = new Component::Datagrid::Renderer::LayersUI(layersUIPage.second);
    Component::Datagrid::Component* areaUIPropertiesGrid
      = new Component::Datagrid::Component(layersUIPage.first, areaUIPropertiesRenderer);
    layersUIPage.first->add(areaUIPropertiesGrid, wxT("layersUI"), wxT("Map Wise"));
    areaUIPropertiesRenderer->control(areaUIPropertiesGrid);

    Component::Datagrid::Renderer::LayersVisibility* visibilityRenderer
      = new Component::Datagrid::Renderer::LayersVisibility();
    Component::Datagrid::Component* visibilityGrid
      = new Component::Datagrid::Component(layersUIPage.first, visibilityRenderer);
    auto pageVisilityGrid
      = layersUIPage.first->add(visibilityGrid, wxT("areaVisibility"), wxT("Area Wise"));
    pageVisilityGrid->displayExtraControls(false);
    visibilityRenderer->control(visibilityGrid);

    layersUIPage.first->select(wxT("layersUI"));

    layersNotebook->select(wxT("map"));

    // createAllComponentsNeededByTheMainNotebook();

    // Sizers
    pMainSizer = new wxBoxSizer(wxVERTICAL);

    pMainPanel = new Component::MainPanel(pBigDaddy);
    pMainSizer->Add(pMainPanel, 0, wxALL | wxEXPAND);

    wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);
    sizerH->Add(pSectionNotebook, 1, wxALL | wxEXPAND);
    pMainSizer->Add(sizerH, 1, wxALL | wxEXPAND);

    pBigDaddy->SetSizer(pMainSizer);

    // AUI Manager
    pAUIManager.SetManagedWindow(this);

    pAUIManager.AddPane(pBigDaddy,
                        wxAuiPaneInfo()
                          .Name(wxT("form_content"))
                          .CenterPane()
                          .PaneBorder(false)
                          .MaximizeButton(true));

    // Do not display controls by default - No study is loaded
    pAUIManager.GetPane(pBigDaddy).Hide();

    // Updating the manager
    pAUIManager.Update();
    // Initialize the state of controls for the first time
    UpdateGUIFromStudyState();

    // Toolbar
    CreateWindowToolbar(*this, pAUIManager);

    // The components are now ready. (prevent against assertions)
    pGuiReady = true;

    // Binding events
    OnStudyLoaded.connect(this, &ApplWnd::onStudyLoaded);
    OnStudyLoaded.connect(&Window::AnalyzerWizard::ResetLastFolderToCurrentStudyUser);
    OnStudySavedAs.connect(&Window::AnalyzerWizard::ResetLastFolderToCurrentStudyUser);
    OnStudyBeginUpdate.connect(&MemoryFlushBeginUpdate);
    OnStudyEndUpdate.connect(&MemoryFlushEndUpdate);
    // System parameter
    OnStudySettingsChanged.connect(this, &ApplWnd::onSystemParametersChanged);

    // Update the status bar
    resetDefaultStatusBarText();

    Thaw();

    // temporary disabled while checking the license
    Enable(false);
    installUserLicense();

    if (checkGDPRStatus())
    {
        installUserLicense(true);
        Antares::License::statusOnline = Antares::License::Status::stInitialize;
        // check the license informations
        // (in another thread - it can take some time)
        DispatchCheckAntaresLicense();
    }
    else
    {
        // Enable(true);// not enough, show the startup wizard!!!!
        // logs.info() << "Launching startup wizard";
        //			Window::StartupWizard::Show();
        Antares::License::statusOnline = Antares::License::Status::stNotRequested;
        Yuni::Bind<void()> callback;
        callback.bind(&CheckAntaresLicense, true);
        Antares::Dispatcher::Post(callback);
    }
}

void ApplWnd::installUserLicense(bool online)
{
    String activationKey = online ? ANTARES_ONLINE_ACTIVATION_KEY : ANTARES_OFFLINE_ACTIVATION_KEY;

    // creating a copy of the activation key, since CheckActivationKeyValidity
    // may modify our variable
    String activationKeyToInstall = activationKey;
    activationKeyToInstall.trim();
    activationKeyToInstall.replace("\r", "");

    if (not Antares::License::CheckActivationKeyValidity(Data::versionLatest, activationKey))
    {
        logs.error() << "The activation key is invalid";
        return;
    }

    // Installation for all users ?
    bool allusers = false;

    // Installation filename
    String filename;

    if (not OperatingSystem::FindAntaresLocalAppData(filename, allusers))
    {
        logs.error() << "impossible to find the local app data";
        return;
    }

    if (not IO::Directory::Create(filename))
    {
        logs.error()
          << "impossible to install the license. Please check your user account privileges";
        return;
    }

    // the license filename
    filename << IO::Separator << "antares-" << ANTARES_VERSION << ".hwb";

    if (not IO::File::SetContent(filename, activationKeyToInstall))
    {
        logs.error()
          << "impossible to install the license. Please check your user account privileges";
        return;
    }
}

void ApplWnd::startAntares()
{
    // Reset the status bar to display the new informations
    // extracted from the license
    resetDefaultStatusBarText();

    // re-enable the main form
    Enable(true);

    if (not logs.logfile())
    {
        wxString msg = wxT("Impossible to create the log file.\n");
#ifdef YUNI_OS_WINDOWS
        msg << wxT("Please check your permissions for writing into the local app data folder.");
#else
        msg << wxT("Please check your permissions for writing into the temporary folder");
#endif

        Window::Message message(
          this, wxT("Logs"), wxT("No Log file"), msg, "images/misc/warning.png");
        message.add(Window::Message::btnContinue, true);
        message.showModal();
    }

    // start the flushing timer
    if (pFlushMemoryTimer)
        pFlushMemoryTimer->Start(23000, wxTIMER_CONTINUOUS); // 23s

    // we may have to load a study given from the command line
    // otherwise, the startup wizard will be launched
    if (not StudyToLoadAtStartup.empty())
    {
        // Load the study
        Dispatcher::StudyOpen(StudyToLoadAtStartup);
        // Making sure that this variable is empty before loading the study
        StudyToLoadAtStartup.clear();
        StudyToLoadAtStartup.shrink();
    }
    else
    {
        logs.info() << "Launching startup wizard";
        Window::StartupWizard::Show();
    }
}

void ApplWnd::createAllComponentsNeededByTheMainNotebook()
{
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (pMainNotebookAlreadyHasItsComponents)
        return;

    logs.info() << LOG_UI << "Preparing the interface";
    ::wxBeginBusyCursor();

    // Work in progress
    SetStatusText(wxT("  Preparing the interface"));
    WIP::Locker wip;

    // Making sure that the main panel is hidden
    pAUIManager.GetPane(pBigDaddy).Hide();
    pAUIManager.Update();
    // Force the redraw of the main frame, especially needed when a study is
    // loaded from the startup wizard
    forceRefresh();

    // Do not recreate those components again
    pMainNotebookAlreadyHasItsComponents = true;

    // We can not seriously do something without the main notebook
    if (!pNotebook)
    {
        ::wxEndBusyCursor();
        return;
    }

    // Simulation
    createNBSimulation();
    createNBNotes();

    // yield !
    wxTheApp->Yield();

    // Separator
    pNotebook->addSeparator();

    // Load
    createNBLoad();
    // Thermal
    createNBThermal();
    // Hydro
    createNBHydro();

    // yield !
    wxTheApp->Yield();

    // Wind
    createNBWind();
    // Solar
    createNBSolar();

    // Separator
    pNotebook->addSeparator();

    // yield !
    wxTheApp->Yield();

    // Misc
    createNBMisc();
    // Reserves / DSM
    createNBDSM();

    // Interconnections
    createNBInterconnections();
    // Binding constraints
    createNBBindingConstraints();
    // Nodal optimization
    createNBNodalOptimization();

    // yield !
    wxTheApp->Yield();

    // Scenario Builder
    createNBScenarioBuilder();
    // Output viewer
    createNBOutputViewer();

    // yield !
    wxTheApp->Yield();

    // Separator
    pNotebook->addSeparator();

    createNBSets();

    // Select the first page
    pSectionNotebook->select(wxT("input"));

    // Restoring the cursor
    ::wxEndBusyCursor();
}

void ApplWnd::prepareStudyIconFile()
{
    assert(wxIsMainThread() == true && "Must be ran from the main thread");
    if (Resources::FindFile(Data::StudyIconFile, "icons/study.ico"))
    {
        wxIcon icon(wxStringFromUTF8(Data::StudyIconFile), wxBITMAP_TYPE_ICO);
        SetIcon(icon);
    }
}

void ApplWnd::createNBSimulation()
{
    assert(pNotebook);
    pNotebook->add(new Window::Simulation::Panel(pNotebook), wxT("simulation"), wxT("Simulation"));
}

void ApplWnd::createNBThermal()
{
    assert(pNotebook);

    auto* panel = new Window::Thermal::Panel(pNotebook);
    pNotebook->add(panel, wxT("thermal"), wxT("Thermal"));

    pageThermalTimeSeries = panel->pageThermalTimeSeries;
    pageThermalPrepro = panel->pageThermalPrepro;
    pageThermalCommon = panel->pageThermalCommon;
    pageThermalClusterList = panel->pageThermalClusterList;
}

void ApplWnd::createNBBindingConstraints()
{
    assert(pNotebook);

    pWndBindingConstraints = new Window::BindingConstraint(pNotebook);
    if (pWndBindingConstraints)
        pNotebook->add(
          pWndBindingConstraints, wxT("bindingconstraints"), wxT("Binding constraint"));
}

void ApplWnd::createNBDSM()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page
      = createStdNotebookPage<Toolbox::InputSelector::Area>(
        pNotebook, wxT("dsm"), wxT("Reserves / DSM"));

    // Time-series
    auto* p = page.first->add(
      new Component::Datagrid::Component(
        page.first, new Component::Datagrid::Renderer::DSM(page.first, page.second)),
      wxT("Reserves / DSM"));
    p->select();
}

void ApplWnd::createNBMisc()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page
      = createStdNotebookPage<Toolbox::InputSelector::Area>(
        pNotebook, wxT("misc"), wxT("Misc. Gen."));

    // Time-series
    Component::Notebook::Page* p = page.first->add(
      new Component::Datagrid::Component(
        page.first, new Component::Datagrid::Renderer::Misc(page.first, page.second)),
      wxT("Misc. Gen."));
    p->select();
}

void ApplWnd::createNBInterconnections()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Connections*> page
      = createStdNotebookPage<Toolbox::InputSelector::Connections>(
        pNotebook, wxT("interconnections"), wxT("Links"));

    // Interco
    pageLinksDetails
      = page.first->add(new Window::Interconnection(page.first, page.second), wxT("  Details  "));
    pageLinksSummary = page.first->add(
      new Component::Datagrid::Component(
        page.first, new Component::Datagrid::Renderer::Links::Summary(page.first)),
      wxT(" Summary "));
}

void ApplWnd::onMapDblClick(Map::Component& /*sender*/)
{
    Window::Inspector::Show();
}

bool ApplWnd::checkGDPRStatus()
{
    // search for the GDPR configuration file
    GDPR_filename.clear();

    String localAppData;
    if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
    {
        localAppData.clear();
    }

    if (not localAppData.empty())
    {
        GDPR_filename = localAppData;
    }

    GDPR_filename << Yuni::IO::Separator << "antares.hwb";
    if (IO::File::Exists(GDPR_filename))
    {
        // load the GDPR status from file
        std::ifstream ifs;

        ifs.open(GDPR_filename.to<std::string>(), std::ifstream::in);
        ifs >> timeStamp;
        ifs >> consent;
        ifs >> banned;

        ifs.close();

        if (banned == 1)
        {
            timeStamp = std::time(nullptr);
            banned = 0;
            consent = -1;

            // save the GDPR status to file
            std::ofstream ofs;

            ofs.open(GDPR_filename.to<std::string>(), std::ifstream::out | std::ofstream::trunc);
            ofs << timeStamp;
            ofs << "\n";
            ofs << consent;
            ofs << "\n";
            ofs << banned;

            ofs.close();
        }
    }
    else
    {
        // show GDPR compliance notice if not found

        wxString msg
          = wxT("By letting us know that our software is now running for a new user, you\n\
can give the Antares_Simulator Team a powerful incentive to keep on further\n\
developing and improving the tool.\n\
\n\
This is why we ask for your consent to the automated communication, through\n\
the internet, of the activation of this copy of Antares_Simulator.\n\
\n\
At any time, You can :\n\
\n\
- Withdraw your consent  with the \"continue off - line\" command\n\
- Renew your consent  with the \"continue on - line\" command\n\
- Display the signature of this copy with the \"show signature\" command\n\
\n\
Click on Continue to express consent");

        Window::Message message(
          this,
          wxT(""),
          wxT("Congratulations for the successful installation of Antares_Simulator"),
          msg,
          "images/128x128/antares.png");
        message.add(Window::Message::btnContinue, true);
        message.add(Window::Message::btnCancel);
        // get the timestamp
        timeStamp = std::time(nullptr);
        if (message.showModal() == Window::Message::btnCancel)
        {
            consent = -1;
        }
        else
        {
            consent = 1;
        }

        // save the GDPR status to file
        std::ofstream ofs;

        ofs.open(GDPR_filename.to<std::string>(), std::ifstream::out | std::ofstream::trunc);
        ofs << timeStamp;
        ofs << "\n";
        ofs << consent;
        ofs << "\n";
        ofs << banned;

        ofs.close();
    }

    return consent == 1;
}

void ApplWnd::setGDPRStatus(bool checkOnline)
{
    if (IO::File::Exists(GDPR_filename))
    {
        std::ifstream ifs;

        ifs.open(GDPR_filename.to<std::string>(), std::ifstream::in);
        ifs >> timeStamp;
        ifs >> consent;
        ifs >> banned;
        ifs.close();

        if (checkOnline)
        {
            if (banned == 1)
            {
                Window::Message message(this,
                                        wxT(""),
                                        "Usage metrics",
                                        "Antares_Simulator signature is no longer valid. Please "
                                        "restart to be able to join again",
                                        "images/128x128/antares.png");
                message.add(Window::Message::btnOk, true);
                message.showModal();
            }
            else
            {
                if (consent == 1) // display  "  Antares_Simulator is already online (anonymous
                                  // usage metrics)" // nothing to do
                {
                    Window::Message message(
                      this,
                      wxT(""),
                      "Usage metrics",
                      "Antares_Simulator is already online (anonymous usage metrics)",
                      "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                }
                else
                {
                    Window::Message message(
                      this,
                      wxT(""),
                      "Usage metrics",
                      "Antares_Simulator will start on-line next time (anonymous usage metrics)",
                      "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                    consent = 1;
                }
            }
        }
        else
        {
            if (banned == 0)
            {
                if (consent == -1)
                {
                    Window::Message message(this,
                                            wxT(""),
                                            "Usage metrics",
                                            "Antares_Simulator is already off-line",
                                            "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                } // no need to BAN
                else
                {
                    Window::Message message(this,
                                            wxT(""),
                                            "Usage metrics",
                                            "Antares_Simulator will continue off-line",
                                            "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                    banned = 1;
                } // wait restart to set CONSENT to -1 and compute new timestamp and hostid
            }
            else // new call of the BAN command by this ui instance, or by another one ; nothing to
                 // do until next restart
            {
                Window::Message message(this,
                                        wxT(""),
                                        "Usage metrics",
                                        "Antares_Simulator will stay off-line",
                                        "images/128x128/antares.png");
                message.add(Window::Message::btnOk, true);
                message.showModal();
                banned = 1;
            }
        }

        // save the GDPR status to file
        std::ofstream ofs;

        ofs.open(GDPR_filename.to<std::string>(), std::ifstream::out | std::ofstream::trunc);
        ofs << timeStamp;
        ofs << "\n";
        ofs << consent;
        ofs << "\n";
        ofs << banned;

        ofs.close();
    }
}

} // namespace Forms
} // namespace Antares
