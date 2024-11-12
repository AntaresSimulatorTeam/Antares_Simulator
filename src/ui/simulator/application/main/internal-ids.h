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
