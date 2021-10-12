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

#include "../main.h"
#include "../menus.h"
#include "../recentfiles.h"
#include "../study.h"
#include "../../windows/message.h"
#include "../../toolbox/resources.h"
#include "internal-data.h"
#include "internal-ids.h"
#include "../../../../config.h"
#include <antares/antares.h>
#include <ui/common/component/frame/registry.h>

using namespace Yuni;

namespace Antares
{
namespace Forms
{
wxMenuBar* ApplWnd::createMenu()
{
    wxMenuBar* ret = new wxMenuBar();
    // File
    ret->Append(this->createMenuFiles(), wxT("&File"));
    // Edit
    ret->Append((pMenuEdit = createMenuEdit()), wxT("&Edit"));
    // View
    // ret->Append(this->createMenuView(), wxT("&View"));
    // Input
    ret->Append(this->createMenuInput(), wxT("&Input"));
    // Output
    ret->Append(this->createMenuOutput(), wxT("&Output"));
    // Simulation
    ret->Append(this->createMenuSimulation(), wxT("&Run"));
    // Options
    ret->Append(this->createMenuOptions(), wxT("&Configure"));
    // Tools
    ret->Append(this->createMenuTools(), wxT("&Tools"));
    // Window
    ret->Append(this->createMenuWindow(), wxT("&Window"));
    // Help / ?
    ret->Append(this->createMenuHelp(), (System::windows) ? wxT("&?") : wxT("&Help"));

    return ret;
}

wxMenu* ApplWnd::createPopupMenuOperatorsOnGrid()
{
    auto* menu = new wxMenu();

    // Wizard
    Menu::CreateItem(menu, mnIDPopupOpNone, wxT("None"), "images/16x16/empty.png");
    menu->AppendSeparator();
    Menu::CreateItem(menu, mnIDPopupOpAverage, wxT("Average "));
    Menu::CreateItem(menu, mnIDPopupOpCellCount, wxT("Cell count "));
    Menu::CreateItem(menu, mnIDPopupOpMinimum, wxT("Minimum "));
    Menu::CreateItem(menu, mnIDPopupOpMaximum, wxT("Maximum "));
    Menu::CreateItem(menu, mnIDPopupOpSum, wxT("Sum "));
    return menu;
}

wxMenu* ApplWnd::createMenuFiles()
{
    delete pMenuFile;
    pMenuFile = new wxMenu();
    // Wizard
    Menu::CreateItem(pMenuFile,
                     mnIDWizard,
                     wxT("Startup wizard"),
                     "images/16x16/wizard.png",
                     wxT("Display the startup wizard"));
    pMenuFile->AppendSeparator();

    // New
    Menu::CreateItem(pMenuFile,
                     mnIDNew,
                     wxT("New\tCtrl+N"),
                     "images/16x16/new.png",
                     wxT("Create a new study with default settings"));

    // Open a study
    Menu::CreateItem(
      pMenuFile, mnIDOpen, wxT("&Open...\tCtrl+O"), "images/16x16/open.png", wxT("Load a study"));
    pMenuFileRecents = pMenuFile->AppendSubMenu(new wxMenu(), wxT("Quick open"));
    Menu::RebuildRecentFiles(pMenuFileRecents);

    // Save
    Menu::CreateItem(pMenuFile,
                     mnIDSave,
                     wxT("Save\tCtrl+S"),
                     "images/16x16/savestudy.png",
                     wxT("Save the last changes"));
    Menu::CreateItem(
      pMenuFile, mnIDSaveAs, wxT("Save As..."), nullptr, wxT("Save the study as..."));

    pMenuFile->AppendSeparator();

    Menu::CreateItem(
      pMenuFile, mnIDExportMap, wxT("Export Map..."), nullptr, wxT("Export the map as..."));

    pMenuFile->AppendSeparator();

    if (System::windows)
    {
        Menu::CreateItem(pMenuFile,
                         mnIDOpenExplorer,
                         wxT("Open in Windows Explorer..."),
                         "images/16x16/empty.png",
                         wxT("Open the folder in Windows Explorer"));
    }
    else
    {
        Menu::CreateItem(pMenuFile,
                         mnIDOpenExplorer,
                         wxT("Open in file explorer..."),
                         "images/16x16/empty.png",
                         wxT("Open the folder in file explorer"));
    }

    Menu::CreateItem(pMenuFile,
                     mnIDCleanOpenedStudyFolder,
                     wxT("Clean the study folder..."),
                     nullptr,
                     wxT("Clean the study folder..."));

    pMenuFile->AppendSeparator();

    // Close
    Menu::CreateItem(
      pMenuFile, mnIDClose, wxT("Close"), "images/16x16/empty.png", wxT("Close the current study"));

    pMenuFile->AppendSeparator();

    pMenuFile->Append(wxID_EXIT, wxT("Quit"), wxT("Quit"));
    return pMenuFile;
}

wxMenu* ApplWnd::createMenuEdit()
{
    auto& data = *(this->data()); // internal data
    wxMenu* menu = new wxMenu();

    data.pEditCurrentLocation = Menu::CreateItem(
      menu, wxID_ANY, wxT("System Maps"), "images/16x16/empty.png", wxEmptyString);
    data.pEditCurrentLocation->Enable(false);

    Menu::CreateItem(menu,
                     mnIDStudyEditCopy,
                     wxT("&Copy\tCtrl+C"),
                     "images/16x16/copy.png",
                     wxT("Copy to the clipboard"));
    Menu::CreateItem(menu,
                     mnIDStudyEditPaste,
                     wxT("&Paste\tCtrl+V"),
                     "images/16x16/paste.png",
                     wxT("Paste from the clipboard"));
    Menu::CreateItem(menu,
                     mnIDStudyEditPasteSpecial,
                     wxT("Paste special..."),
                     nullptr,
                     wxT("Paste special from the clipboard"));

    menu->AppendSeparator();

    Menu::CreateItem(menu,
                     mnIDStudyEditMapReverseSelection,
                     wxT("Reverse  "),
                     nullptr,
                     wxT("Reverse the selection"));

    Menu::CreateItem(menu,
                     mnIDStudyEditMapUnselectAll,
                     wxT("Unselect a&ll"),
                     nullptr,
                     wxT("Unselect all items on the map"));

    Menu::CreateItem(menu,
                     mnIDStudyEditMapSelectAll,
                     wxT("Select a&ll"),
                     nullptr,
                     wxT("Select all items on the map"));

    /*
    menu->AppendSeparator();

    Menu::CreateItem(menu, mnIDStudyEditTitle,
                    wxT("Name of the study"), "images/16x16/empty.png",
                    wxT("Set the name of the study"));
    Menu::CreateItem(menu, mnIDStudyEditAuthors,
                    wxT("Author(s) of the study"), "images/16x16/empty.png",
                    wxT("Set the author(s) of the study"));
    */

    return menu;
}

wxMenu* ApplWnd::createMenuView()
{
    delete pMenuView;
    return new wxMenu();
}

wxMenu* ApplWnd::createMenuInput()
{
    delete pMenuInput;
    pMenuInput = new wxMenu();

    pMenuInputCreation = Menu::CreateItem(
      pMenuInput, wxID_ANY, wxT("Created: "), "images/16x16/empty.png", wxEmptyString);

    pMenuInputLastSaved
      = Menu::CreateItem(pMenuInput, wxID_ANY, wxT("Last saved: none"), nullptr, wxEmptyString);

    pMenuInputCreation->Enable(false);
    pMenuInputLastSaved->Enable(false);

    pMenuInput->AppendSeparator();

    Menu::CreateItem(pMenuInput,
                     mnIDStudyEditTitle,
                     wxT("Name of the study"),
                     "images/16x16/empty.png",
                     wxT("Set the name of the study"));
    Menu::CreateItem(pMenuInput,
                     mnIDStudyEditAuthors,
                     wxT("Author(s) of the study"),
                     "images/16x16/empty.png",
                     wxT("Set the author(s) of the study"));

    pMenuInput->AppendSeparator();

    Menu::CreateItem(pMenuInput,
                     mnIDViewAllSystem,
                     wxT("View Map All"),
                     "images/16x16/empty.png",
                     wxT("View 'All'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewSystem,
                     wxT("View  System\tAlt+1"),
                     "images/16x16/empty.png",
                     wxT("View 'System Maps'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewSimulation,
                     wxT("View  Simulation\tAlt+2"),
                     "images/16x16/empty.png",
                     wxT("View 'Simulation'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewNotes,
                     wxT("View  User's Notes\tAlt+3"),
                     "images/16x16/notes.png",
                     wxT("View 'User's Notes'"));

    pMenuInput->AppendSeparator();

    Menu::CreateItem(pMenuInput, mnIDViewLoad, wxT("View  Load"), nullptr, wxT("View 'Load'"));
    Menu::CreateItem(pMenuInput, mnIDViewSolar, wxT("View  Solar"), nullptr, wxT("View 'Solar'"));
    Menu::CreateItem(pMenuInput, mnIDViewWind, wxT("View  Wind"), nullptr, wxT("View 'Wind'"));
    Menu::CreateItem(pMenuInput, mnIDViewRenewable, wxT("View  Renewable"), nullptr, wxT("View 'Wind'"));
    Menu::CreateItem(pMenuInput, mnIDViewHydro, wxT("View  Hydro"), nullptr, wxT("View 'Hydro'"));
    Menu::CreateItem(
      pMenuInput, mnIDViewThermal, wxT("View  Thermal"), nullptr, wxT("View 'Thermal'"));

    Menu::CreateItem(
      pMenuInput, mnIDViewMiscGen, wxT("View  Misc Gen."), nullptr, wxT("View 'Misc Gen.'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewReservesDSM,
                     wxT("View  Reserves / DSM"),
                     nullptr,
                     wxT("View 'Reserves / DSM'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewInterconnections,
                     wxT("View  Links"),
                     nullptr,
                     wxT("View 'Interconnections'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewBindingConstraints,
                     wxT("View  Binding constraints"),
                     nullptr,
                     wxT("View 'Binding constraints'"));
    Menu::CreateItem(pMenuInput,
                     mnIDViewNodalOptimization,
                     wxT("View  Economic Optimization"),
                     nullptr,
                     wxT("View 'Economic Optimization'"));

    /*
    Menu::CreateItem(pMenuInput, mnIDStudyCheck,
                    wxT("Check study..."), "images/16x16/check2.png",
                    wxT("Check data of the study"));

    pMenuInput->AppendSeparator();

    auto* it = Menu::CreateItem(pMenuInput, mnIDStudyCreateCommitPoint,
                    wxT("Create a new commit point\tF5"), "images/16x16/commit.png",
                    wxT("Create a new commit point"));
    it->Enable(false);

    it = Menu::CreateItem(pMenuInput, mnIDStudyRevisionTree,
                    wxT("Revision tree..."), "images/16x16/revision.gif",
                    wxT("Open the revision tree"));
    it->Enable(false);

    pMenuInput->AppendSeparator();

    it = Menu::CreateItem(pMenuInput, mnIDStudyRemoveReadyMadeTS,
                    wxT("Remove ready-made TS..."), "images/16x16/empty.png",
                    wxT("Remive ready-made timeseries..."));
    it->Enable(false);
    */

    return pMenuInput;
}

wxMenu* ApplWnd::createMenuOutput()
{
    delete pMenuOutput;
    pMenuOutput = new wxMenu();

    // see application/main/refresh.cpp:
    //  method ApplWnd::refreshMenuOutput

    /*!
    pMenuOutputCountAvailable = Menu::CreateItem(pMenuOutput, wxID_ANY,
            wxT("0 simulation results available"),
            "images/16x16/minibullet.png", wxEmptyString);

    pMenuOutputLastAvailable = Menu::CreateItem(pMenuOutput, wxID_ANY,
            wxT("last simulation: none"), nullptr,
            wxEmptyString);

    pMenuOutput->AppendSeparator();

    # ifdef YUNI_OS_WINDOWS
    pMenuOpenOutputInExplorer = pMenuOutput->AppendSubMenu(new wxMenu(),
            wxT("Open in Windows Explorer..."));
    # else
    pMenuOpenOutputInExplorer = pMenuOutput->AppendSubMenu(new wxMenu(),
            wxT("Open in Gnome Nautilus..."));
    # endif

    pMenuOutputCountAvailable->Enable(false);
    pMenuOutputLastAvailable->Enable(false);
    pMenuOpenOutputInExplorer->Enable(false);
    */
    return pMenuOutput;
}

wxMenu* ApplWnd::createMenuSimulation()
{
    delete pMenuSimulation;
    pMenuSimulation = new wxMenu();

    // Run !
    Menu::CreateItem(pMenuSimulation,
                     mnIDRunTheSimulation,
                     wxT("Run a Monte Carlo simulation\tF9"),
                     "images/16x16/run-simulation.png",
                     wxT("Run a Monte Carlo simulation"));
    Menu::CreateItem(pMenuSimulation,
                     mnIDRunTheTSGenerators,
                     wxT("Run the time-series generators  "),
                     "images/16x16/empty.png",
                     wxT("Run the time-series generators"));

    pMenuSimulation->AppendSeparator();

    Menu::CreateItem(pMenuSimulation,
                     mnIDRunTheTSAnalyzer,
                     wxT("Run the time-series analyzer"),
                     nullptr,
                     wxT("Run the time-series analyzer"));

    Menu::CreateItem(pMenuSimulation,
                     mnIDRunTheConstraintsBuilder,
                     wxT("Run the Kirchhoff's constraints generator"),
                     nullptr,
                     wxT("Run the Kirchhoff's constraints generator"));

    return pMenuSimulation;
}

wxMenu* ApplWnd::createMenuOptions()
{
    delete pMenuOptions;
    pMenuOptions = new wxMenu();
    wxMenuItem* it;

    Menu::CreateItem(pMenuOptions,
                     mnIDOptionConfigureThematicTrimming,
                     wxT("Thematic trimming\tctrl+alt+t"),
                     "images/16x16/filter.png",
                     wxT("Configure thematic trimming"));

    delete pMenuGeographicTrimming;
    pMenuGeographicTrimming
      = pMenuOptions->AppendSubMenu(new wxMenu(), wxT("Geographic trimming..."));

    wxMenu* geoTrimSubMenu = pMenuGeographicTrimming->GetSubMenu();
    Menu::CreateItem(geoTrimSubMenu,
                     mnIDOptionConfigureAreasTrimming,
                     wxT("Areas"),
                     "images/16x16/filter.png",
                     wxT("Configure areas trimming"));
    Menu::CreateItem(geoTrimSubMenu,
                     mnIDOptionConfigureLinksTrimming,
                     wxT("Links"),
                     "images/16x16/filter.png",
                     wxT("Configure links trimming"));

    it = Menu::CreateItem(pMenuOptions,
                          mnIDOptionConfigureDistricts,
                          wxT("Regional districts\tctrl+alt+d"),
                          "images/16x16/filter.png",
                          wxT("Configure regional districts"));
    Menu::CreateItem(pMenuOptions,
                     mnIDOptionConfigureMCScenarioBuilder,
                     wxT("MC Scenario Builder"),
                     "images/16x16/scenariobuilder.png",
                     wxT("Configure MC Scenario Builder"));
    Menu::CreateItem(pMenuOptions,
                     mnIDOptionConfigureMCScenarioPlaylist,
                     wxT("MC Scenario playlist"),
                     "images/16x16/orderedlist.png",
                     wxT("Configure MC Scenario playlist"));

    pMenuOptions->AppendSeparator();

    Menu::CreateItem(pMenuOptions,
                     mnIDOptionOptimizationPrefs,
                     wxT("Optimization preferences"),
                     "images/16x16/optimization-prefs.png",
                     wxT("Set optimization preferences"));
    it = Menu::CreateItem(pMenuOptions,
                          mnIDOptionAdvanced,
                          wxT("Advanced parameters"),
                          "images/16x16/empty.png",
                          wxT("Set advanced parameters"));
    it->Enable(false);

    return pMenuOptions;
}

wxMenu* ApplWnd::createMenuTools()
{
    delete pMenuTools;
    pMenuTools = new wxMenu();
    wxMenuItem* it;

    it = Menu::CreateItem(pMenuTools,
                          mnIDToolsStudyManager,
                          wxT("Study Manager  \tCtrl+M"),
                          "images/16x16/studymanager.png",
                          wxT("Open the scripts editor"));
    it->Enable(false);

    pMenuTools->AppendSeparator();

    // Memory Statistics
    Menu::CreateItem(pMenuTools,
                     mnIDMemoryUsedByTheStudy,
                     wxT("Resources monitor"),
                     "images/16x16/resources.png",
                     wxT("View 'Resources monitor'"));

    pMenuTools->AppendSeparator();

    // SWAP Folder
    Menu::CreateItem(pMenuTools,
                     mnIDOptionTempFolder,
                     wxT("Configure the swap folder"),
                     nullptr,
                     wxT("Configure the swap folder"));

    return pMenuTools;
}

wxMenu* ApplWnd::createMenuWindow()
{
    if (not pMenuWindow)
        pMenuWindow = new wxMenu();
    else
        Menu::Clear(*pMenuWindow);

    // Fullscreen
    Menu::CreateItem(pMenuWindow,
                     mnIDFullscreen,
                     wxT("Toggle Fullscreen"),
                     "images/16x16/fullscreen.png",
                     wxT("Toggle the Fullscreen mode"));

    pMenuWindow->AppendSeparator();

    Menu::CreateItem(pMenuWindow,
                     mnIDInspector,
                     wxT("Inspector"),
                     "images/16x16/inspector.png",
                     wxT("Show the inspector"));

    // Logs
    Menu::CreateItem(pMenuWindow,
                     mnIDStudyLogs,
                     wxT("Log viewer"),
                     "images/16x16/logs.png",
                     wxT("View 'Log viewer'"));

    auto& framelist = Component::Frame::Registry::List();
    if (framelist.size() > 1)
    {
        pMenuWindow->AppendSeparator();
        int evId = mnIDWindowRaise_0 - 1;

        foreach (auto* frame, framelist)
        {
            ++evId;
            if (frame->excludeFromMenu())
                continue;

            wxString mnCaption = wxStringFromUTF8(frame->frameTitle());
            if (evId != mnIDWindowRaise_Other)
            {
                Menu::CreateItem(pMenuWindow,
                                 evId,
                                 mnCaption,
                                 "images/16x16/minibullet.png",
                                 wxString() << wxT("Raise window `") << mnCaption << wxT('`'));
            }
            else
            {
                Menu::CreateItem(pMenuWindow,
                                 evId,
                                 wxT("Others..."),
                                 "images/16x16/minibullet.png",
                                 wxT("Expand to other open windows"));
                break;
            }
        }
    }

    return pMenuWindow;
}

wxMenu* ApplWnd::createMenuHelp()
{
    delete pMenuHelp;
    pMenuHelp = new wxMenu();

    Menu::CreateItem(pMenuHelp,
                     mnIDHelpPDFGeneralReferenceGuide,
                     wxT("General reference guide    \tF1"),
                     "images/16x16/help.png",
                     wxT("Open PDF : General reference guide"));

    Menu::CreateItem(pMenuHelp,
                     mnIDHelpPDFOptimizationProblemsFormulation,
                     wxT("Optimization problems formulation"),
                     "images/16x16/help.png",
                     wxT("Open PDF : Optimization problems formulation"));

    Menu::CreateItem(pMenuHelp,
                     mnIDHelpPDFSystemMapEditorReferenceGuide,
                     wxT("System Map Editor reference guide"),
                     "images/16x16/help.png",
                     wxT("Open PDF : System Map Editor reference guide"));

    Menu::CreateItem(pMenuHelp,
                     mnIDHelpPDFExamplesLibrary,
                     wxT("Examples library"),
                     "images/16x16/help.png",
                     wxT("Open PDF : Examples library"));

    pMenuHelp->AppendSeparator();

    Menu::CreateItem(pMenuHelp, mnIDHelpAbout, wxT("About Antares"), nullptr, wxT("About Antares"));

    return pMenuHelp;
}

void ApplWnd::evtOnOpenRecentsToggleShowPaths(wxCommandEvent& evt)
{
    RecentFiles::ShowPathInMenu(evt.IsChecked());
    Menu::RebuildRecentFiles(Forms::ApplWnd::Instance()->menuRecentFiles());
}

void ApplWnd::evtOnOpenRecentsClearHistory(wxCommandEvent&)
{
    Window::Message message(this,
                            wxT("Recent files"),
                            wxT("Clear the recent files list"),
                            wxT("Do you really want to clear the recent files list ?"));
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);

    if (message.showModal() == Window::Message::btnYes)
    {
        RecentFiles::ListPtr emptyList;
        RecentFiles::Write(emptyList);
        Menu::RebuildRecentFiles(Forms::ApplWnd::Instance()->menuRecentFiles());

        Window::Message ok(this, wxT("Recent files"), wxT("\nThe recent files list is cleared"));
        ok.add(Window::Message::btnContinue);
        ok.showModal();
    }
}

void ApplWnd::evtOnOpenStudyFolderInExplorer(wxCommandEvent&)
{
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        if (System::windows)
            wxExecute(wxString(wxT("explorer.exe \""))
                      << wxStringFromUTF8(study->folder) << wxT("\""));
        else
            wxExecute(wxString(wxT("xdg-open \"")) << wxStringFromUTF8(study->folder) << wxT("\""));
    }
}

void ApplWnd::evtOnOpenOutputInExplorer(wxCommandEvent& evt)
{
    auto end = ListOfOutputsForTheCurrentStudy.end();
    for (auto i = ListOfOutputsForTheCurrentStudy.begin(); i != end; ++i)
    {
        if ((*i)->menuID == evt.GetId())
        {
            if (System::windows)
                wxExecute(wxString(wxT("explorer.exe \""))
                          << wxStringFromUTF8((*i)->path) << wxT("\""));
            else
                wxExecute(wxString(wxT("xdg-open \""))
                          << wxStringFromUTF8((*i)->path) << wxT("\""));
            return;
        }
    }
}

void ApplWnd::evtOnViewOutput(wxCommandEvent& evt)
{
    auto end = ListOfOutputsForTheCurrentStudy.end();
    for (auto i = ListOfOutputsForTheCurrentStudy.begin(); i != end; ++i)
    {
        if ((*i)->viewMenuID == evt.GetId())
        {
            OnStudyUpdateOutputInfo(ListOfOutputsForTheCurrentStudy, *i);
            pSectionNotebook->select(wxT("output"), true);
            return;
        }
    }
}

void ApplWnd::evtOnViewSystem(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("sys"));
}

void ApplWnd::evtOnViewSimulation(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("simulation"));
}

void ApplWnd::evtOnViewNotes(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("notes"));
}

void ApplWnd::evtOnViewLoad(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("load"));
}

void ApplWnd::evtOnViewSolar(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("solar"));
}

void ApplWnd::evtOnViewWind(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("wind"));
}

void ApplWnd::evtOnViewRenewable(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("renewable"));
}

void ApplWnd::evtOnViewHydro(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("hydro"));
}

void ApplWnd::evtOnViewThermal(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("thermal"));
}

void ApplWnd::evtOnViewReservesDSM(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("dsm"));
}

void ApplWnd::evtOnViewMiscGen(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("misc"));
}

void ApplWnd::evtOnViewInterconnections(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("interconnections"));
}

void ApplWnd::evtOnViewBindingConstraints(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("bindingconstraints"));
}

void ApplWnd::evtOnViewNodalOptimization(wxCommandEvent&)
{
    pSectionNotebook->select(wxT("input"));
    pNotebook->select(wxT("nodal"));
}

} // namespace Forms
} // namespace Antares
