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
#ifndef __ANTARES_APPLICATION_MAIN_MAIN_H__
# define __ANTARES_APPLICATION_MAIN_MAIN_H__

# include <yuni/yuni.h>
# include <ui/common/wx-wrapper.h>
# include <wx/aui/aui.h>

# include "../../toolbox/components/notebook/notebook.h"
# include "../../toolbox/components/datagrid/selectionoperation.h"
# include "../../toolbox/components/map/settings.h"
# include <list>
# include "fwd.h"
# include "config.h"
# include <ui/common/component/frame/local-frame.h>

const Yuni::String ANTARES_ONLINE_ACTIVATION_KEY = "-----BEGIN LICENSE ACTIVATION KEY-----\n\
KhdzHc1K+qf48oQE6WVxMAhdazfLoHAEDBFdmLAUFhDYmHc6Sh7f6mpDlyeiwX63R5bk4vfhaFkDOot+h\n\
GrDshZD/PJk/1z/6f8SCnbbX0G8qZu88VqscnI2AXBvBSY39G8ZYo/Qy41gJZdEQgG/mPksc/O3o5pMW+\n\
Y9xy6sofg=-uWL3Qn9+Q2liZzCxiLDCePf0efIGizgdQvwveQsNIPiBgvkpntg1/4LHz8JZFNKbg4VXkM\n\
PV7NB3Ad1VGD25mpvTyCIpwS7CEVHOJm7th0nRfNlaQugAqv4bBbPVpiCC+E60cwzeGDvzzd+gl/7dwE2\n\
YE/XwAgX/RIfwJwsP4u8EC81gVOWtBfxhW1zAOdmtcYmooBXJ2haUMaOBrJqd+xPEKid+/OkJJ0jgtYNv\n\
S7g3o2kxID2FwwSK8sB78DZtQqD738OWzB9jAi7Yxw486mwMsaOQzsF1ShPzsnR9T/qcUsX7DN9kpzUn/\n\
b/M86nn6ZyNEoNb4l0BLIXQdwm41F2j9PdoExymkpep38bloYKdM2M6tOZ0576YBh8HHA9L9IBqg2zsUw\n\
QjLxwcpD7nLq9/y9UUxUdQ2VQqGoPqBztO8dTMTtLK1QTNNYGftK/nuViPnfYJlX2GhBj53uhqz39IkjR\n\
0ejnMjSvabTjZgyhn6fGTz1Cg3N3jayiOBc4n3tuQo0V/QPXuxeZYSkWQn7b9C9Cz6io/moBzJQiVFaX0\n\
qrvXvCQvvUhiMIOjrIYxbuRxfYiujVc6dQtp8BF8rwc/e3d5TCeJQARDpRKeQ1DnYveTqiOU4a6T5mCMy\n\
blGExhFzozsO8B/thIJ3gXk+7mVRceHIMTop81klggNqXIvD/jGGHok8iWWlNgTI/TWlzgpb/yPjCSCE0\n\
6r3b+Gog+hjjoIyFMih2v6FYf8xw8ZVBxjSFlY3cGze5uKuq4JMGkZTy3pjnOP1wur9CUuSw2u3qSehAt\n\
CfIlrj0ktJLHOUkxjS0cLRA==\n\
-----END LICENSE ACTIVATION KEY-----";

const Yuni::String ANTARES_OFFLINE_ACTIVATION_KEY = "-----BEGIN LICENSE ACTIVATION KEY-----\n\
GERrD6Ah7M0lVgkYfmz+7XMMIt/nz6E/E5LIt+uncPR602dcf4cGUL+IdIY1u/tHhWC1/DtB4tEA2D4w+\n\
2OD7I9/7Q4Jv6WsWSx0BJ0DKny14ut3wlTP2knuPe8o1g9f3At5jIgunUUY/V0pNqp+Dy+++TVjmfEPyz\n\
hoBNQkknI=-a0RdEEk5vJ18zS1hzrkv8q9I7ouze18nmjzTB/ZKNP9fKLjjdmMW7TuzKmd7lvCJ521zAC\n\
kmlPSsc2Oa8dxFvgYgFhWUwpp+8dznN2mBpe4Hrq/qmJLSWKIJA3aXcR5p17wzVqIy8kurbAXf0tNmsX4\n\
1gMqGHwMl9XkSEfq1ZQ4clhlCUgkuoxtbYJv5/bGK5/h6nJRNQ/m+V/DknewulPUTnzM0+8LGJ+wHDnJA\n\
wy/Pens0bp9mamctFTA18n0phng3WsWSMNNPjqMfKWpna5Ki8PExeNBwIP+wsGBYvMbFAuKaQH2P05tF5\n\
xZSjIyYQ9W7s8/Gl0H5i6uYXWcynQkKKfzmt6ZvvQK5P9qiKM2ZkrjxlZyZs88xPphvX5XJs95+rXkOv9\n\
xeppmOd5orhcnikpokaW7gU42KVCHnNQDLYBg+tleZLbk9MG6aVtUnN595dMNbWBef4XPEILXYOPlOcrm\n\
J/Et07vXlS56c+a565/gmtVHH1uV0HYzGR9Xl8u+tfNsNSMIfCW0tkpMZRPvR40Y0F0r7BTCGqBGRQxfg\n\
Qln78nU5TJsLwB9ScNJ3rUgtXJGNMsopb9m733OK46MBMQJZ8wD7RyO4PoMsp9RoETtsboOUDffALK1gL\n\
E+3yjDOtDN/8RfX5MspJXNYRaSP/1jcdi5m0maGhEOH38BCXlPpuCU6FpgDYvgFCoQdt9xYmiOr7Z2iOH\n\
SHC5SBp/02W98nCrSK7uUTxjzu621RnNBdhGNAXupG2JLyuNAxVB0Vm7OjiExUmcDVpiXcbCigPFIY58p\n\
aYykG4lePPY4AbI7BBeSTVQ==\n\
-----END LICENSE ACTIVATION KEY-----";

namespace Antares
{
namespace Forms
{

	// Internal data of the main form, to reduce dependencies and (re)compile time
	// Forward declaration
	class MainFormData;



	/*!
	** \brief Main Frame of the Antares UI application
	*/
	class ApplWnd final : public Component::Frame::WxLocalFrame, public Yuni::IEventObserver<ApplWnd>
	{
	public:
		/*!
		** \brief Get the instance of the main window
		*/
		static ApplWnd* Instance();


	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ApplWnd();
		//! Destructor
		virtual ~ApplWnd();
		//@}


		//! \name Study
		//@{
		/*!
		** \brief Ask to the user a study folder from a standard dialog box
		**
		** \param autoLoad True to automatically launch the loading of the study
		**   This behavior may not be wanted because we may want to properly destroyed
		**   some components before
		** \return The path to the study. Empty if nothing to do
		*/
		wxString openStudyFolder(bool autoLoad = true);
		//@}

		/*!
		** \brief Starting sequence, delayed after the creation of the main form
		*/
		void startAntares();

		/*!
		** \brief Get the wxMenuItem associated to the recent files
		*/
		wxMenuItem* menuRecentFiles() const;

		/*!
		** \brief Emit an event to update the GUI after a study has been loaded
		** or closed
		**
		** \param studyOpened True if a study has been loaded, false otherwise
		*/
		void requestUpdateGUIAfterStudyIO(const bool studyOpened);

		/*!
		** \brief Get the component used for the main panel
		*/
		Component::MainPanel* mainPanel() const;

		void hideAllComponentsRelatedToTheStudy();

		/*!
		** \brief Get the component used for the map
		*/
		Map::Component* map() const;


		/*!
		** \name Grid operator (for selected cells)
		**
		** A grid operator computes an operation (Sum, average...) on all selected
		** cells of the grid that currently has the focus. The result of this
		** computation is displayed in the status bar.
		**
		** \see Antares::Component::Datagrid::Component::onGridEnter()
		** \see Antares::Component::Datagrid::Component::onGridLeave()
		*/
		//@{
		/*!
		** \brief Get the current grid operator for selected cells
		*/
		Component::Datagrid::Selection::IOperator* gridOperatorSelectedCells() const;

		/*!
		** \brief Set the grid operator for selected cells
		*/
		void gridOperatorSelectedCells(Component::Datagrid::Selection::IOperator* v);

		/*!
		** \brief Update the GUI to display the result of the grid operator
		**
		** This method should be called each time the cells selection changes.
		** \param grid The `wxGrid` that has currently the focus (may be NULL)
		*/
		void gridOperatorSelectedCellsUpdateResult(wxGrid* grid);

		/*!
		** \brief Disable the grid operator
		*/
		void disableGridOperatorIfGrid(wxGrid* grid);
		//@}

		//! \name Title of the Window
		//@{
		void title();
		void title(const wxString& s);
		//@}


		/*!
		** \brief Ask the user if the study must be saved
		** \return False if the operation must be cancelled
		*/
		bool wouldYouLikeToSaveTheStudy();

		/*!
		** \brief Forward command events to the current control, if any
		*/
		virtual bool ProcessEvent(wxEvent& event);


		void loadUserNotes();
		void saveUserNotes();

		//! Save the study
		void saveStudy();
		//! Save the study as...
		void saveStudyAs(const Yuni::String& path, bool copyoutput, bool copyuserdata, bool copylogs);
		//! Export the map
		void exportMap(const Yuni::String& path, bool transparentBackground, const wxColour& backgroundColor, const std::list<uint16_t> &layers, int nbSplitParts, Antares::Map::mapImageFormat format);

		/*!
		** \brief Select the first page of the main notebook
		*/
		void selectSystem();

		/*!
		** \brief Create all components for displaying a study
		*/
		void createAllComponentsNeededByTheMainNotebook();

		/*!
		** \brief Display the logs window
		*/
		void showStudyLogs();
		void refreshStudyLogs();

		void evtOnViewOutput(wxCommandEvent& evt);
		void evtOnOpenOutputInExplorer(wxCommandEvent& evt);

		//! \name Logs
		//@{
		//! Avoid display logs
		void beginUpdateLogs() const;
		//! end update
		void endUpdateLogs() const;
		//! Reconnect the logs to the main callback
		void connectLogCallback();
		//@}

		/*!
		** \brief Select all default pages for all notebooks
		**
		** This method should be used when performing major tasks on the study
		** (new, open)
		*/
		void selectAllDefaultPages();

		void refreshMenuInput();
		/*!
		** \brief Refresh the simulation results list
		*/
		void refreshMenuOutput();

		/*!
		** \brief Assign the focus to this form and refresh it
		*/
		void forceFocus();
		//! Force the focus (delayed)
		void delayForceFocus();

		/*!
		** \brief Reset the drag and drop target
		*/
		void resetDragAndDrop();

		/*!
		** \brief Force the refresh of the window and all its children
		*/
		void forceRefresh();

		/*!
		** \brief Get the AUI manager
		*/
		wxAuiManager& AUIManager() {return pAUIManager;}
		const wxAuiManager& AUIManager() const {return pAUIManager;}

		/*!
		** \brief Reset the default text for the status bar
		*/
		void resetDefaultStatusBarText();

		/*!
		** \brief Launch the processor
		**
		** \param filename A filename which contains all required informations for the analyzer
		*/
		void launchAnalyzer(const Yuni::String& filename);


		//! \name Extras
		//@{
		/*!
		** \brief Get if the scenario builder page is opened
		*/
		bool isScenarioBuilderOpened() const;
		//@}

		//! \name Swap files cleaning
		//@{
		/*!
		** \brief Perform a cleanup operation on orphan swap files
		**
		** This action will be delayed
		*/
		void timerCleanSwapFiles(uint timeout = 5000);

		void timerCleanSwapFilesDestroy();

		/*!
		** \brief Stop all timers
		*/
		void backgroundTimerStop();

		void backgroundTimerStart();
		//@}


		//! \name Main Notebook
		//@{
		//! Get the current equipment page
		uint mainNotebookCurrentEquipmentPage() const;
		//@}


		/*!
		** \brief Select the input data notebook page
		*/
		void backToInputData();


		//! \name Clipboard Support
		//@{
		/*!
		** \brief Copy to the clipboard items present in the inspector
		*/
		void copyToClipboard();

		/*!
		** \brief Try to paste data from the clipboard
		**
		** \param showDialog True to make advanced settings available to the user
		*/
		void pasteFromClipboard(bool showDialog = false);

		/*!
		** \brief Perform the 'paste' operation from a given text
		**
		** \param text       A text, which should come from the clipboard
		** \param showDialog True to make advanced settings available to the user
		*/
		void pasteFromClipboard(const Yuni::String& text, bool showDialog);
		//@}


		//! \name WIP Events (Work In Progress)
		//@{
		/*!
		** \brief Show the WIP status
		**
		** This method should not be called directly. Use Antares::WIP::Leave() instead
		** or the class WIP::Locker.
		*/
		void showWIP();
		/*!
		** \brief Hide the WIP status
		**
		** This method should not be called directly. Use Antares::WIP::Leave() instead
		** or the class WIP::Locker.
		*/
		void hideWIP();
		//! Get if the WIP mode is enabled
		bool isInWIPMode() const;
		//@}

		//! \name Internal data
		//@{
		//! retrieve the internal data
		MainFormData* data();
		//! Retrieve the internal data (const)
		const MainFormData* data() const;
		//@}

	public:
		//! Event: The application is about to quit
		Yuni::Event<void()> onApplicationQuit;

	private:
		//! \name Create / Destroy
		//@{
		//! Initialize the frame from the constructor
		void internalInitialize();

		void installUserLicense(bool online = false);

		bool checkGDPRStatus();

		void setGDPRStatus(bool checkOnline);


		//! Prepare the icon file to use for study folders
		void prepareStudyIconFile();

		//! Create a complete menu for the window
		wxMenuBar* createMenu();
		//! Create a popup menu for all available operators on selected cells (grid)
		wxMenu* createPopupMenuOperatorsOnGrid();

		//! Create menu: File
		wxMenu* createMenuFiles();
		//! Create menu: Edit
		wxMenu* createMenuEdit();
		//! Create menu: View
		wxMenu* createMenuView();
		//! Create menu: Input
		wxMenu* createMenuInput();
		//! Create menu: Output
		wxMenu* createMenuOutput();
		//! Create menu: Simulation
		wxMenu* createMenuSimulation();
		//! Create menu: Options
		wxMenu* createMenuOptions();
		//! Create menu: Scripts
		wxMenu* createMenuScripts();
		//! Create menu: Tools
		wxMenu* createMenuTools();
		//! Create menu: Window
		wxMenu* createMenuWindow();
		//! Create menu: Help
		wxMenu* createMenuHelp();

		//! Execute an event from the Queue service
		void evtOnExecuteQueueEvent(wxCommandEvent&);

		//! Create the page about the settings of the simulation
		void createNBSimulation();
		//! Create the page about the notes of the study
		void createNBNotes();
		//! Create the page about the load
		void createNBLoad();
		//! Create the page about the solar
		void createNBSolar();
		//! Create the page about the hydro (time-series, prepro, correlation...)
		void createNBHydro();
		//! Create the page about the wind (time-series, prepro, correlation...)
		void createNBWind();
		//! Create the page about the thermal clusters
		void createNBThermal();

		//! Create the page about reserves and DSM
		void createNBDSM();
		//! Create the page
		void createNBMisc();
		//! Create the page about interconnections between areas
		void createNBInterconnections();
		//! Create the page about binding constraints
		void createNBBindingConstraints();
		//! Create the page about the nodal optimization
		void createNBNodalOptimization();
		//! Create Scenaro builder page & Output
		void createNBScenarioBuilder();
		//! Create output viewer
		void createNBOutputViewer();
		//@}


		//! \name Logs
		//@{
		//! Initialize the log facility
		void createLogs();
		//! Destroy the log viewer
		void destroyLogsViewer();
		//! Destroy data associated the logs
		void destroyLogs();
		//@}

		//! \name Event: Context menu
		//@{
		//! Show the context menu associated to the status bar
		void evtOnContextMenuStatusBar(wxContextMenuEvent& evt);
		void evtOnContextMenuChangeOperator(wxCommandEvent& evt);
		void evtOnContextMenuMap(int x, int y);
		//@}


		//! \name Event: Menu: Files
		//@{
		//! Show the Wizard
		void evtOnWizard(wxCommandEvent& evt);
		//! Quit
		void evtOnQuit(wxCommandEvent& evt);
		//! Create a new study
		void evtOnNewStudy(wxCommandEvent& evt);
		//! Open a study
		void evtOnOpenStudy(wxCommandEvent& evt);
		//! Toggle the value to show/hide the path of studies in the "recent files"
		void evtOnOpenRecentsToggleShowPaths(wxCommandEvent& evt);
		//! Clear the history of recent files
		void evtOnOpenRecentsClearHistory(wxCommandEvent& evt);
		//! Recents
		void evtOnDropDownRecents(wxAuiToolBarEvent& evt);
		//! Close the current opened study
		void evtOnCloseStudy(wxCommandEvent& evt);
		//! Quick open a study
		void evtOnQuickOpenStudy(wxCommandEvent& evt);
		//! Save the study
		void evtOnSave(wxCommandEvent& evt);
		//! Save the study as
		void evtOnSaveAs(wxCommandEvent& evt);
		//! Save the the map
		void evtOnExportMap(wxCommandEvent& evt);
		//! Fullscreen mode
		void evtOnFullscreen(wxCommandEvent& evt);
		//! Inspector
		void evtOnInspector(wxCommandEvent& evt);
		//! Open the study in Windows Explorer
		void evtOnOpenStudyFolderInExplorer(wxCommandEvent& evt);
		//@}

		//! \name View
		//@{
		void evtOnViewSystem(wxCommandEvent& evt);
		void evtOnViewAllSystem(wxCommandEvent& evt);
		void evtOnViewSimulation(wxCommandEvent& evt);
		void evtOnViewNotes(wxCommandEvent& evt);
		void evtOnViewLoad(wxCommandEvent& evt);
		void evtOnViewSolar(wxCommandEvent& evt);
		void evtOnViewWind(wxCommandEvent& evt);
		void evtOnViewHydro(wxCommandEvent& evt);
		void evtOnViewThermal(wxCommandEvent& evt);
		void evtOnViewReservesDSM(wxCommandEvent& evt);
		void evtOnViewMiscGen(wxCommandEvent& evt);
		void evtOnViewInterconnections(wxCommandEvent& evt);
		void evtOnViewBindingConstraints(wxCommandEvent& evt);
		void evtOnViewNodalOptimization(wxCommandEvent& evt);
		//@}

		//! \name Event: Study
		//@{
		void evtOnSetStudyInfos(wxCommandEvent& evt);
		//! Event: Display all sessions for the study
		void evtOnStudySessions(wxCommandEvent& evt);
		//! Event: Edit: Map: Select all areas
		void evtOnEditMapSelectAll(wxCommandEvent& evt);
		//! Event: Edit: Map: Unselect all
		void evtOnEditMapUnselectAll(wxCommandEvent& evt);
		//! Event: Edit: Map: Reverse the selection
		void evtOnEditMapReverseSelection(wxCommandEvent& evt);
		//! Event: Edit: Copy
		void evtOnEditCopy(wxCommandEvent& evt);
		//! Event: Edit: Paste
		void evtOnEditPaste(wxCommandEvent& evt);
		//! Event: Edit: Paste special
		void evtOnEditPasteSpecial(wxCommandEvent& evt);
		//! Event: Map double click
		void onMapDblClick(Map::Component& sender);
		//@}

		//! \name Event: Menu: Simulation
		//@{
		//! Event: Run the simulation
		void evtOnRunSimulation(wxCommandEvent& evt);
		void evtOnRunTSGenerators(wxCommandEvent& evt);
		void evtOnRunTSGeneratorsDelayed();

		void evtOnRunTSAnalyzer(wxCommandEvent& evt);
		void evtOnRunTSAnalyzerDelayed();
		//@}

		//! \name Event: Menu: Tools
		//@{
		//! Event: Memory used by the current study
		void evtOnMemoryUsedByTheStudy(wxCommandEvent& evt);
		//@}

		//! \name Event: Menu: Options
		//@{
		//! Event: Temp folder
		void evtOnOptionsTempFolder(wxCommandEvent& evt);
		//! MC Playlist
		void evtOnOptionsMCPlaylist(wxCommandEvent& evt);
		//! MC Scenario Builder
		void evtOnOptionsMCScenarioBuilder(wxCommandEvent& evt);
		//! Optimization preferences
		void evtOnOptionsOptimizationPrefs(wxCommandEvent& evt);
		//! Advanced parameters
		void evtOnOptionsAdvanced(wxCommandEvent& evt);
		//@}

		//! \name Event: Menu: Window
		//@{
		//! Event: Study Logs
		void evtOnStudyLogs(wxCommandEvent& evt);
		//! Raise a window
		void evtOnRaiseWindow(wxCommandEvent& evt);
		//@}

		//! \name Event: Menu: Help
		//@{
		//! Event: Help
		//void evtOnHelpVisitRTEWebsite(wxCommandEvent& evt);
		//! Event: About this application
		void evtOnHelpAbout(wxCommandEvent& evt);
		//! Event: PDF General Reference guide
		void evtOnHelpPDFGeneralReferenceGuide(wxCommandEvent& evt);
		//! Event: PDF System Map Editor Reference guide
		void evtOnHelpPDFSystemMapEditorReferenceGuide(wxCommandEvent& evt);

		void evtOnHelpContinueOnline(wxCommandEvent& evt);

		void evtOnHelpContinueOffline(wxCommandEvent& evt);

		void evtOnShowID(wxCommandEvent &);
		//@}

		//! \name Menu event
		//@{
		void evtOnMenuOpen(wxMenuEvent& evt);
		void evtOnMenuClose(wxMenuEvent& evt);
		//@}

		//! Event: The GUI must be updated after an operation on the study
		void evtOnUpdateGUIAfterStudyIO(bool opened);

		//! A page has been selected on the main notebook
		void onMainNotebookPageChanging(Component::Notebook::Page& page);
		//! A page has been selected on the section notebook (input, scenariobuilder...)
		void onSectionNotebookPageChanging(Component::Notebook::Page& page);
		//! A page has been selected on the scenario builder notebook
		void onScenarioBuilderNotebookPageChanging(Component::Notebook::Page& page);
		//! A page has been selected on the output notebook
		void onOutputNotebookPageChanging(Component::Notebook::Page& page);

		//!! The system parameters have changed, some pages have to be hidden
		void onSystemParametersChanged();

		//! Update the Interface after loaded a study
		void evtOnUpdateInterfaceAfterLoadedStudy(wxCommandEvent& evt);

		void evtOnFrameClose(wxCloseEvent& evt);

		void internalFrameClose();

		void evtOnCleanCurrentStudyFolder(wxCommandEvent& evt);

		void evtOnCleanStudyFolder(wxCommandEvent& evt);

		void evtLaunchAnalyzer(wxCommandEvent& evt);

		void onMouseMoved(wxMouseEvent& evt);

		void onLogMessageDeferred(int level, const Yuni::String& message);
		void onLogMessage(wxCommandEvent& evt);

		void onStudyLoaded();

		void purgeAllTemporaryObjects();

		virtual void updateOpenWindowsMenu();
		virtual bool excludeFromMenu();

	private:
		//! Pointer to a study
		typedef Data::Study*  StudyPtr;

	private:
		//! The main panel
		wxPanel* pBigDaddy;
		//! The AUI Manager
		wxAuiManager pAUIManager;
		//! The main sizer
		wxSizer* pMainSizer;
		//! The menu of the window
		wxMenuBar* pMenu;
		//! Menu: File
		wxMenu* pMenuFile;
		wxMenuItem* pMenuFileRecents;
		//! Menu: Edit
		wxMenu* pMenuEdit;
		//! Menu: View
		wxMenu* pMenuView;
		//! Menu: Input
		wxMenu* pMenuInput;
		wxMenuItem* pMenuInputCreation;
		wxMenuItem* pMenuInputLastSaved;
		//! Menu: Output
		wxMenu* pMenuOutput;
		//! Menu: Simulation
		wxMenu* pMenuSimulation;
		//! Menu: Options
		wxMenu* pMenuOptions;
		//! Menu: Scripts
		wxMenu* pMenuScripts;
		//! Menu: Window
		wxMenu* pMenuWindow;
		//! Menu: Tools
		wxMenu* pMenuTools;
		//! Menu: Help
		wxMenu* pMenuHelp;

		//! Popup menu: Operators for selected cells on any grid
		wxMenu* pPopupMenuOperatorsGrid;

		//! The map
		Map::Component* pMainMap;
		Component::Notebook* pNotebook;
		Component::Notebook* pScenarioBuilderNotebook;
		Component::Notebook* pOutputViewerNotebook;
		Component::Notebook* pSectionNotebook;

		//! The main panel - Infos about the current study
		Component::MainPanel* pMainPanel;

		Component::Notebook::Page* pageLoadTimeSeries;
		Component::Notebook::Page* pageLoadPrepro;
		Component::Notebook::Page* pageLoadCorrelation;

		Component::Notebook::Page* pageSolarTimeSeries;
		Component::Notebook::Page* pageSolarPrepro;
		Component::Notebook::Page* pageSolarCorrelation;

		Component::Notebook::Page* pageHydroTimeSeries;
		Component::Notebook::Page* pageHydroPrepro;
		Component::Notebook::Page* pageHydroCorrelation;
		Component::Notebook::Page* pageHydroAllocation;

		Component::Notebook::Page* pageWindTimeSeries;
		Component::Notebook::Page* pageWindPrepro;
		Component::Notebook::Page* pageWindPreproDailyProfile;
		Component::Notebook::Page* pageWindCorrelation;

		Component::Notebook::Page* pageThermalClusterList;
		Component::Notebook::Page* pageThermalTimeSeries;
		Component::Notebook::Page* pageThermalPrepro;
		Component::Notebook::Page* pageThermalCommon;

		Component::Notebook::Page* pageLinksSummary;
		Component::Notebook::Page* pageLinksDetails;

		Component::Notebook::Page* pageNodalOptim;

		Window::BindingConstraint* pWndBindingConstraints;

		//! The current grid operator to use on selected cells
		Component::Datagrid::Selection::IOperator* pGridSelectionOperator;
		wxGrid* pGridSelectionAttachedGrid;

		//! A context menu for the map
		wxMenu* pMapContextMenu;

		//GDPR file path
		Yuni::String GDPR_filename;
		//Time stamp used for hostid
		long timeStamp;
		//user consent on sending data
		int consent;
		//user consent on sending data
		int banned;

		//! User notes
		Window::Notes* pUserNotes;

		//! Lazy creation of components for the main notebook
		bool pMainNotebookAlreadyHasItsComponents;

		//! List of logs to flush
		wxTimer* pLogFlusherTimer;

		//! Timer for the periodic refresh of the memory usage
		wxTimer* pFlushMemoryTimer;

		//! Logs
		Window::StudyLogs* pWndLogs;

		//! Flag to avoid suspicious calls when the GUI is not ready
		bool pGuiReady;

		uint pUpdateCountLocker;

		//! Internals data
		MainFormData* pData;

		//! Current Equipment page
		uint pCurrentEquipmentPage;

		// friends
		template<class WindowT> friend class Disabler;
		friend class MainFormData;
		friend class Window::StudyLogs;

		// Event table
		DECLARE_EVENT_TABLE()

	}; // class ApplWnd



	//! Load a study at startup, if not empty
	extern Yuni::String StudyToLoadAtStartup;






} // namespace Forms
} // namespace Antares

# include "main.hxx"
# include "disabler.h"

#endif // __ANTARES_APPLICATION_MAIN_MAIN_H__
