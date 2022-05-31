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
#ifndef __ANTARES_APPLICATION_MAIN_INTERNAL_IDS_H__
#define __ANTARES_APPLICATION_MAIN_INTERNAL_IDS_H__

#include <ui/common/wx-wrapper.h>

namespace Antares
{
namespace Forms
{
/*!
** \brief Predefined ID
**
** \warning ID > 65535 don't work on Windows ...
*/
enum MenusID
{
    //! \name File
    //@{
    mnIDQuit = wxID_EXIT,
    mnIDExecuteQueueEvent = wxID_HIGHEST + 42 /*arbitrary*/,
    mnIDWizard,
    mnIDNew,
    mnIDOpen,
    mnIDSave,
    mnIDSaveAs,
    mnIDExportMap,
    mnIDClose,
    mnIDOpenRecents, // + max recent files
    mnIDOpenRecents_0,
    mnIDOpenRecents_1,
    mnIDOpenRecents_2,
    mnIDOpenRecents_3,
    mnIDOpenRecents_4,
    mnIDOpenRecents_5,
    mnIDOpenRecents_6,
    mnIDOpenRecents_7,
    mnIDOpenRecents_8,
    mnIDOpenRecents_9,
    mnIDOpenRecents_ToggleShowPaths,
    mnIDOpenRecents_ClearHistory,
    mnIDCleanOpenedStudyFolder,
    mnIDOpenExplorer,
    //@}

    //! \name View
    //@{
    mnIDViewSystem,
    mnIDViewAllSystem,
    mnIDViewSimulation,
    mnIDViewNotes,
    mnIDViewLoad,
    mnIDViewSolar,
    mnIDViewWind,
    mnIDViewRenewable,
    mnIDViewHydro,
    mnIDViewThermal,
    mnIDViewMiscGen,
    mnIDViewReservesDSM,
    mnIDViewInterconnections,
    mnIDViewBindingConstraints,
    mnIDViewNodalOptimization,
    // -
    mnIDMemoryUsedByTheStudy,
    mnIDStudyLogs,
    //@}

    //! \name Study
    //@{
    mnIDStudyEditMapSelectAll,
    mnIDStudyEditMapUnselectAll,
    mnIDStudyEditMapReverseSelection,
    mnIDStudyEditTitle,
    mnIDStudyEditAuthors,
    mnIDStudyEditCopy,
    mnIDStudyEditPaste,
    mnIDStudyEditPasteSpecial,
    mnIDStudySessions,
    mnIDStudyCheck,
    mnIDStudyCreateCommitPoint,
    mnIDStudyRevisionTree,
    mnIDStudyRemoveReadyMadeTS,
    //@}

    //! \name Simulation
    //@{
    mnIDRunTheSimulation,
    mnIDRunTheTSGenerators,
    mnIDRunTheTSAnalyzer,
    mnIDRunTheConstraintsBuilder,
    mnIDOpenOutputInExplorer,
    //@}

    //! \name Tools
    //@{
    mnIDCleanAStudyFolder,
    mnIDToolsStudyManager,
    //@}

    //! \name Options
    //@{
    mnIDOptionTempFolder,
    mnIDOptionConfigurePrefetch,
    mnIDOptionConfigureThematicTrimming,
    mnIDOptionConfigureAreasTrimming,
    mnIDOptionConfigureLinksTrimming,
    mnIDOptionConfigureDistricts,
    mnIDOptionConfigureMCScenarioBuilder,
    mnIDOptionConfigureMCScenarioPlaylist,
    mnIDOptionOptimizationPrefs,
    mnIDOptionAdvanced,
    mnIDOptionAdequacyPatchOptions,
    mnIDOptionAdequacyPatchAreas,

    //@}

    //! \name Window
    //@{
    mnIDFullscreen,
    mnIDInspector,
    mnIDWindowRaise_0,
    mnIDWindowRaise_1,
    mnIDWindowRaise_2,
    mnIDWindowRaise_3,
    mnIDWindowRaise_4,
    mnIDWindowRaise_5,
    mnIDWindowRaise_6,
    mnIDWindowRaise_7,
    mnIDWindowRaise_8,
    mnIDWindowRaise_9,
    mnIDWindowRaise_Other,
    //@}

    //! \name Help
    //@{
    mnIDHelpHelp,
    mnIDHelpPDFGeneralReferenceGuide,
    mnIDHelpPDFOptimizationProblemsFormulation,
    mnIDHelpPDFSystemMapEditorReferenceGuide,
    mnIDHelpPDFExamplesLibrary,
    mnIDHelpAbout,
    mnIDHelpVisitRTEWebSite,
    mnIDHelpOnlineDocumentation,
    //@}

    //! \name Misc
    //@{
    mnIDToolBar,
    //@}

    //
    mnUpdateInterfaceAfterLoadingAStudy,
    mnInternalLogMessage,
    mnRefreshGUI,
    mnIDLaunchAnalyzer,
    mnIDLaunchConstraintsBuilder,

    //! \name Popup Menu Operator for selected cells on any grid
    //@{
    mnIDPopupOpNone,
    mnIDPopupOpAverage,
    mnIDPopupOpCellCount,
    mnIDPopupOpMinimum,
    mnIDPopupOpMaximum,
    mnIDPopupOpSum,
    //@}

    //! \name Popup Menu Operator for selected nodes on any layer
    //@{
    mnIDPopupSelectionHide,
    mnIDPopupSelectionShow,
    //@}

}; // enum MenusID

} // namespace Forms
} // namespace Antares

#endif // __ANTARES_APPLICATION_MAIN_INTERNAL_IDS_H__
