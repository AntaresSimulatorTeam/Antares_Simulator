// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_MAIN_INTERNAL_IDS_H__
#define __ANTARES_APPLICATION_MAIN_INTERNAL_IDS_H__

#include <ui/common/wx-wrapper.h>

namespace Antares::Forms
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
    mnIDStudyEditEditor,
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

} // namespace Antares::Forms

#endif // __ANTARES_APPLICATION_MAIN_INTERNAL_IDS_H__
