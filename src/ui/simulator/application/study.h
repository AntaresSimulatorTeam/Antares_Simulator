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
#ifndef __ANTARES_APPLICATION_STUDY_H__
#define __ANTARES_APPLICATION_STUDY_H__

#include <yuni/yuni.h>
#include "../toolbox/wx-wrapper.h"
#include <antares/study.h>
#include <antares/study/output.h>
#include <yuni/core/event.h>
#include <list>
#include <antares/solver.h>

#include "ui/simulator/toolbox/components/map/settings.h"

namespace Antares
{
enum SaveResult
{
    svsError = 0,
    svsSaved,
    svsDiscard,
    svsCancel
};

/*!
** \brief The path to the last opened study
*/
extern wxString gLastOpenedStudyFolder;

/*!
** \brief Create a new Study
*/
void NewStudy();

/*!
** \brief Open a study from a folder
**
** This method will display a window while loading the study in another thread
*/
void OpenStudyFromFolder(wxString folder);

/*!
** \brief Save the current study
*/
SaveResult SaveStudy();

/*!
** \brief Save the current study into a folder
*/
SaveResult SaveStudyAs(const Yuni::String& path, bool copyoutput, bool copyuserdata, bool copylogs);

/*!
** \brief Export the current study map into a folder
*/
SaveResult ExportMap(const Yuni::String& path,
                     bool transparentBackground,
                     const wxColour& backgroundColor,
                     const std::list<uint16_t>& layers,
                     int nbSplitParts,
                     Antares::Map::mapImageFormat format);

/*!
** \brief Close the study and all the resources
**
** \return True if a study has been closed, false otherwise
*/
bool CloseTheStudy(bool updateGUI = true);

/*!
** \brief Check if a folder is contained within a study
*/
bool CheckIfInsideAStudyFolder(const AnyString& path, bool quiet = false);

/*!
** \brief Run the simulation (solver)
**
** \param study A study
** \param simuName User-Name of the simulation
** \param simuComments Comments for the simulation
** \param ignoreWarnings True if warnings can be silently ignored
** \param useOrtools True if ortools must be used by antares-solver
** \param ortoolsSolver Ortools solver used in case of ortools use by antares-solver
*/
void RunSimulationOnTheStudy(Data::Study::Ptr study,
                             const YString& simuName,
                             const YString& simuComments,
                             bool ignoreWarnings,
                             Solver::Feature features = Solver::standard,
                             bool preproOnly = false,
                             bool useOrtools = false,
                             const std::string& ortoolsSolver = "sirius");

/*!
** \brief Update the state of controls
*/
void UpdateGUIFromStudyState();

/*!
** \brief Rename an area immediatly
*/
bool StudyRenameArea(Data::Area* area, const AnyString& newname, Data::Study::Ptr study = nullptr);

/*!
** \brief Mark the current study as modified
*/
void MarkTheStudyAsModified();

/*!
** \brief Mark the study as modified (only if identical to the current one)
*/
void MarkTheStudyAsModified(const Data::Study::Ptr& study);

void ResetTheModifierState(bool v);

/*!
** \brief Get if the study has been modified
**
** The return value tells if the study has been modified since
** its creation (new/load).
*/
bool StudyHasBeenModified();

/*!
** \brief Get the current in-memory revision value
**
** This value has no real meaning, except that you can know
** if he study has been modified between two points in time
** if this value is different.
*/
Yuni::uint64 StudyInMemoryRevisionID();

/*!
** \brief Refresh the list of all available outputs
*/
void RefreshListOfOutputsForTheCurrentStudy();

void StudyRefreshCalendar();

//! \name Events
//@{
/*!
** \brief Event: A study has been loaded or a new study has been created
**
** This event is mainly used to perform graphical updates.
*/
extern Yuni::Event<void()> OnStudyLoaded;

/*!
** \brief Event: The study has been closed
**
** This event is mainly used to perform graphical updates.
*/
extern Yuni::Event<void()> OnStudyClosed;

/*!
** \brief The study has been saved
*/
extern Yuni::Event<void()> OnStudySaved;

/*!
** \brief The study has been saved as...
*/
extern Yuni::Event<void()> OnStudySavedAs;

/*!
** \brief The state of the study has changed (new, open, save as, run simulation...)
*/

extern Yuni::Event<void(Data::Study&)> OnStudyChanged;

/*!
** \brief The list of areas has changed
*/
extern Yuni::Event<void()> OnStudyAreasChanged;

extern Yuni::Event<void()> OnStudySettingsChanged;

/*!
** \brief Event: The UI should update all controls related to the output info
*/
extern Yuni::Event<void(const Data::Output::List&, const Data::Output::Ptr)>
  OnStudyUpdateOutputInfo;

/*!
** \brief Event: An area has been renamed
*/
extern Yuni::Event<void(Data::Area*)> OnStudyAreaRename;

/*!
** \brief Event: An area has been added
*/
extern Yuni::Event<void(Data::Area*)> OnStudyAreaAdded;

/*!
** \brief Event: An area has been deleted
*/
extern Yuni::Event<void(Data::Area*)> OnStudyAreaDelete;

/*!
** \brief The color of one or several areas has been changed
**
** The param `Data::Area*` will be null if several areas have
** been changed. Otherwise it will indicate the area.
*/
extern Yuni::Event<void(Data::Area*)> OnStudyAreaColorChanged;

/*!
** \brief Event: An area has been added
*/
extern Yuni::Event<void(Data::AreaLink*)> OnStudyLinkAdded;

/*!
** \brief Event: An area has been deleted
*/
extern Yuni::Event<void(Data::AreaLink*)> OnStudyLinkDelete;

/*!
** \brief Event: An area link has been updated
*/
extern Yuni::Event<void(Data::AreaLink*)> OnStudyLinkChanged;

/*!
** \brief Event: A constraint has been added
*/
extern Yuni::Event<void(Data::BindingConstraint*)> OnStudyConstraintAdded;

/*!
** \brief Event: An constraint has been deleted
*/
extern Yuni::Event<void(Data::BindingConstraint*)> OnStudyConstraintDelete;

/*!
** \brief Event: An constraint has been changed (renamed or something else)
*/
extern Yuni::Event<void(Data::BindingConstraint*)> OnStudyConstraintModified;

/*!
** \brief Event: The nodal optimization settings have been changed
**
** This event may concern one or several areas.
*/
extern Yuni::Event<void()> OnStudyNodalOptimizationChanged;

/*!
** \brief Event: The simulation settings have been changed
*/
extern Yuni::Event<void()> OnStudySimulationSettingsChanged;

/*!
** \brief The user's playlist has been updated
*/
extern Yuni::Event<void()> OnStudyUpdatePlaylist;

/*!
** \brief Event: When the study has been saved
*/
extern Yuni::Event<void()> OnStudyReloadAllComponentsWithJITData;

/*!
** \brief Event: The common settings of a thermal cluster has been changed
**
** This event may concern one or several thermal clusters.
*/
extern Yuni::Event<void()> OnStudyThermalClusterCommonSettingsChanged;

extern Yuni::Event<void(Data::ThermalCluster*)> OnStudyThermalClusterRenamed;
extern Yuni::Event<void(Data::Area*)> OnStudyThermalClusterGroupChanged;

/*!
** \brief Event triggered when the data related to the Scenario Builder are loaded
**
** This event means that some components must update their content.
*/
extern Yuni::Event<void()> OnStudyScenarioBuilderDataAreLoaded;

/*!
** \brief Event triggered at the begining of a long and critical update of the study
**
** All components which use data related to the study should avoid
** using them from this moment.
** \see StudyUpdateLocker
*/
extern Yuni::Event<void()> OnStudyBeginUpdate;
/*!
** \brief Event triggered at the end of the a long and critical update of the study
**
** All components may resume their drawing.
*/
extern Yuni::Event<void()> OnStudyEndUpdate;

//! Event to refresh all inspector (except for the pointer given in parameter)
extern Yuni::Event<void(const void*)> OnInspectorRefresh;

//! Event: The selection of the main notebook has changed
extern Yuni::Event<void()> OnMainNotebookChanged;
//@}

//! Event: A UI property of the node is different from the last layer
extern Yuni::Event<void()> OnLayerNodeUIChanged;
//@}

//! Event: The selected layer has changed in the map notebook
extern Yuni::Event<void(const wxString*)> OnMapLayerChanged;
//@}

extern Yuni::Event<void(const wxString*)> OnMapLayerAdded;
extern Yuni::Event<void(const wxString*)> OnMapLayerRemoved;
extern Yuni::Event<void(const wxString*)> OnMapLayerRenamed;

//! \name Memory flush
//
// You should not have to call these routines yourselves
//@{
//! An update is currently performed (inc the ref count)
void MemoryFlushBeginUpdate();
//! The last update is over, we may execute a memory flush (dec the ref count)
void MemoryFlushEndUpdate();
//! Get if a memory flush can be performed
bool CanPerformMemoryFlush();
//@}

extern bool SystemParameterHaveChanged;

/*!
** \brief Flag to force the refresh of the areas when modified
*/
extern bool GUIFlagInvalidateAreas;

//! List of all available outputs for the current study
extern Antares::Data::Output::List ListOfOutputsForTheCurrentStudy;

/*!
** \brief The last path used by the user when opening/saving a file
*/
extern Yuni::String LastPathForOpeningAFile;

/*!
** \brief Avoid the use of the data related to a study for the lifetime
**   of the object (RAII)
*/
class StudyUpdateLocker final
{
public:
    StudyUpdateLocker()
    {
        OnStudyBeginUpdate();
    }
    ~StudyUpdateLocker()
    {
        OnStudyEndUpdate();
    }
};

/*!
** \brief Avoid the execution of a memory flush
*/
class MemoryFlushLocker final
{
public:
    MemoryFlushLocker()
    {
        MemoryFlushBeginUpdate();
    }
    ~MemoryFlushLocker()
    {
        MemoryFlushEndUpdate();
    }
};

} // namespace Antares

#endif // __ANTARES_APPLICATION_STUDY_H__
