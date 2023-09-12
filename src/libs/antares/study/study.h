/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_LIBS_STUDY_STUDY_H__
#define __ANTARES_LIBS_STUDY_STUDY_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/thread/thread.h>
#include <yuni/core/noncopyable.h>
#include <yuni/job/queue/service.h>

#include <antares/writer/i_writer.h>

#include "../antares.h"
#include "../object/object.h"
#include "fwd.h"

#include "simulation.h"
#include "parameters.h"
#include "binding_constraint/BindingConstraint.h"
#include "header.h"
#include "version.h"
#include "sets.h"
#include "progression/progression.h"
#include "load-options.h"
#include <antares/date/date.h>
#include "layerdata.h"
#include <antares/correlation/antares/correlation/correlation.h> //TODO Collision
#include "area/store-timeseries-numbers.h"
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"

#include <memory>

namespace Antares::Data
{
/*!
** \brief Antares Study
*/

class UIRuntimeInfo;
class Study: public Yuni::NonCopyable<Study>, public IObject, public LayerData
{
public:
    using Ptr = std::shared_ptr<Study>;
    //! Set of studies
    using Set = std::set<Ptr>;
    //! List of studies
    using List = std::list<Ptr>;

    //! A single set of areas
    // CompareAreaName : to control the order of areas in a set of areas. This order can have an
    // effect, even if tiny, on the results of aggregations.
    using SingleSetOfAreas = std::set<Area*, CompareAreaName>;

    //! Multiple sets of areas
    using SetsOfAreas = Antares::Data::Sets<SingleSetOfAreas>;

    //! A single set of links
    using SingleSetOfLinks = std::set<AreaLink*>;
    //! Multiple sets of links
    using SetsOfLinks = Antares::Data::Sets<SingleSetOfLinks>;

    //! List of disabled areas
    using DisabledAreaList = std::set<AreaName>;
    //! List of disabled links
    using DisabledAreaLinkList = std::set<AreaLinkName>;
    //! List of disabled thermal clusters
    using DisabledThermalClusterList = std::set<ClusterName>;

    //! Extension filename
    using FileExtension = std::string;

public:
    /*!
    ** \brief Extract the title of a study
    **
    ** \param folder A study folder
    ** \param[out] out      The variable where the title will be written
    ** \param      warnings False to prevent warnings/errors when loading
    ** \return True if the operation succeeded, false otherwise
    */
    static bool TitleFromStudyFolder(const AnyString& folder, YString& out, bool warnings = false);

    /*!
    ** \brief Get if a folder if a study
    **
    ** \param folder A study folder
    ** \return True if the folder is a study, false otherwise
    */
    static bool IsRootStudy(const AnyString& folder);

    /*!
    ** \brief Get if a folder if a study
    **
    ** \param folder A study folder
    ** \param buffer A buffer to reuse for temporary operations
    ** \return True if the folder is a study, false otherwise
    */
    static bool IsRootStudy(const AnyString& folder, YString& buffer);

    /*!
    ** \brief Check if a path is within a study folder
    **
    ** \warning This method assumes that the given path is properly formatted
    **   according to the OS parameters
    **
    ** \param      path     The path to check
    ** \param[out] location The location of the study folder (if any)
    ** \param[out] title    The title of the study folder (if any)
    ** \return True if the path is within a study folder. In this case
    **   the parameters 'location' and 'title' are set.
    */
    static bool IsInsideStudyFolder(const AnyString& path, YString& location, YString& title);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    **
    ** \param forTheSolver True to indicate that the study will be used for a simulation
    **   Consequently some preparations / shortcuts should be done
    */
    Study(bool forTheSolver = false);
    //! Destructor
    virtual ~Study();
    //@}

    //! \name Loading/Saving
    //@{
    /*!
    ** \brief Create a clean study
    */
    void createAsNew();

    /*!
    ** \brief Relocate the study into a new folder
    **
    ** This method does not have any effect except modifying
    ** internal variables (`folder`, `folderInput`, ...).
    */
    void relocate(AnyString newFolder);

    /*!
    ** \brief Load a study from a folder
    **
    ** \param path The path where data are located
    ** \return True if succeeded, false otherwise
    */
    bool loadFromFolder(const AnyString& path, const StudyLoadOptions& options);

    /*!
    ** \brief Clear all ressources held by the study
    */
    void clear();

    /*!
    ** \brief Reload all correlation
    */
    void reloadCorrelation();

    /*!
    ** \brief Reload all XCast Data
    */
    bool reloadXCastData();

    /*!
    ** \brief Save the study into a folder
    **
    ** \param folder The folder where to write data
    ** \return True if succeeded, false otherwise
    */
    bool saveToFolder(const AnyString& newfolder);

    /*!
    ** \brief Reset the folder icon (Windows only)
    **
    ** \return True if the operation succeeded
    */
    bool resetFolderIcon() const;
    //@}

    //! \name Invalidate
    //@{
    /*!
    ** \brief Invalidate the whole study
    **
    ** Mark all JIT structures as invalidated. This will force the loading of missing
    ** data in memory and it will force the rewritten of any matrix.
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the whole study as modified
    */
    void markAsModified() const;
    //@}

    //! \name Areas
    //@{
    /*!
    ** \brief Try to find a name for a new area
    **
    ** \param out      The new name
    ** \param basename The root base name
    ** \return True if a new name has been found, false otherwise
    */
    bool modifyAreaNameIfAlreadyTaken(AreaName& out, const AreaName& basename);

    /*!
    ** \brief Add an area and make all required initialization
    **
    ** It is the safe way to add an area and it is mainly used by the GUI
    **
    ** \param name The name of the new area
    ** \return A pointer to a new area, or NULL if the operation failed
    */
    // TODO no need for the 2nd argument, remove it after the GUI has been removed, keeping the default value
    Area* areaAdd(const AreaName& name, bool update = false);

    /*!
    ** \brief Rename an area
    **
    ** \param area The area. The pointer will no longer be valid after the call to this routine
    ** \return True if the operation succeeded, false otherwise
    ** \see BeautifyName()
    */
    bool areaRename(Area* area, AreaName newName);

    /*!
    ** \brief Delete an area _and_ all its dependancies
    **
    ** It is the safe way to delete an area and it is mainly used by the GUI
    ** \param area The area. The pointer will no longer be valid after the call to this routine
    ** \return True if the operation succeeded, false otherwise
    */
    bool areaDelete(Area* area);

    /*!
    ** \brief Delete an area and all its dependencies
    **
    ** It is the safe way to delete an area and it is mainly used by the GUI
    **
    ** \param s The study
    ** \param area The area. The pointer will no longer be valid after the call to this routine
    ** \return True if the operation succeeded, false otherwise
    */
    void areaDelete(Area::Vector& area);
    //@}

    //! \name Links
    //@{
    /*!
    ** \brief Delete a connection _and_ all its dependencies
    **
    ** It is the safe way to delete a link and it is mainly used by the GUI
    **
    ** \param lnk The link. The pointer will no longer be valid after the call to this routine
    ** \return True if the operation succeeded, false otherwise
    */
    bool linkDelete(AreaLink* lnk);
    //@}

    //! \name Renewable/thermal clusters
    //@{
    /*!
    ** \brief Rename a renewable/thermal cluster
    **
    ** \param cluster The cluster
    ** \return True if the operation succeeded, false otherwise
    */
    bool clusterRename(Cluster* cluster, ClusterName newName);
    //@}

    //! \name Read-only
    //@{
    /*!
    ** \brief Get if the study is in readonly mode
    */
    bool readonly() const;
    //@}

    //! \name Time-series
    //@{
    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    /*!
    ** \brief Store the timeseries numbers
    **
    ** \tparam TimeSeriesT The time-series set to store
    ** \return True if the operation succeeded (the file have been written), false otherwise
    */
    template<unsigned int TimeSeriesT>
    void storeTimeSeriesNumbers() const;
    //@}

    //! \name Simulation
    //@{
    /*!
    ** \brief Perform several modifications required when launching a simulation
    **   (like computing the DSM values)
    **
    ** \return True if the operation succeeded, false otherwise
    */
    void performTransformationsBeforeLaunchingSimulation();

    /*!
    ** \brief Initialize runtime informations required by the solver
    */
    bool initializeRuntimeInfos();

    /*!
    ** \brief Prepare the output where the results of the simulation will be written
    */
    void prepareOutput();

    void saveAboutTheStudy();

    /*!
    ** \brief Initialize the progress meter
    */
    void initializeProgressMeter(bool tsGeneratorOnly);
    //@}

    //! \name Time-series Generators
    //@{
    /*!
    ** \brief Destroy all data of the TS generator '@TS'
    */
    template<enum TimeSeries TS>
    void destroyTSGeneratorData();

    //! Destroy all data of the load TS generator
    void destroyAllLoadTSGeneratorData();
    //! Destroy all data of the solar TS generator
    void destroyAllSolarTSGeneratorData();
    //! Destroy all data of the wind TS generator
    void destroyAllWindTSGeneratorData();
    //! Destroy all data of the hydro TS generator
    void destroyAllHydroTSGeneratorData();
    //! Destroy all data of the thermal TS generator
    void destroyAllThermalTSGeneratorData();

    /*!
    ** \brief Import all time-series into the input folder
    **
    ** This method should be used when the solver has finished its simulation
    ** and the data generated by the preprocessors must be imported in the
    ** `input` folder.
    ** The selection of time-series is controlled by the general data (timeSeriesToImport).
    ** When calling this routine, and if timeSeriesToImport is not equals to 0, the input
    ** folder will be modified.
    **
    ** \return True if the operation suceeded, false otherwise
    */
    bool importTimeseriesIntoInput();
    //@}

    //! \name Scenario Builder
    //@{
    /*!
    ** \brief Load the data related to the scenario builder if not available
    */
    void scenarioRulesLoadIfNotAvailable();
    /*!
    ** \brief Re-Initialize/Re-Load the scenario builder data
    */
    void scenarioRulesCreate();
    /*!
    ** \brief Re-Initialize/Re-Load the scenario builder data but consider a single ruleset only
    */
    void scenarioRulesCreate(const RulesScenarioName& thisoneonly);

    /*!
    ** \brief Release the scenario builder
    */
    void scenarioRulesDestroy();
    //@}

    //! \name Internal Data TS-Generators / Series
    //@{

    /*!
    ** \brief Computes a raw number of cores table.
    **
    ** The table associetes a raw number of cores to each level ("min", "low", "med", "high",
    *"max").
    **
    */
    std::map<std::string, uint> getRawNumberCoresPerLevel();

    /*!
    ** \brief Computes number of cores
    **
    ** From the "Number of Cores" level (in GUI --> Advanced parameters), computes
    ** the real numbers of logical cores to be involved in the MC years parallelisation.
    */
    void getNumberOfCores(const bool forceParallel, const uint nbYearsParallelForced);

    /*!
    ** \brief In case hydro hot start is enabled, checking all conditions are met.
    **
    ** If hydro hot start is enabled, check that :
    ** - For all areas for which reservoir management is enabled :
    **   + Their starting level is initialized on the same day
    **   + This day is the first day of the simulation calendar
    ** - The simulation lasts exactly one year
    ** - All batches (or sets) of simultaneous years have the same size (obvious if a parallel run
    *is not required : answer is yes).
    **
    ** If these conditions are not met, some error message is raised, when attempting to run the
    *study.
    */
    bool checkHydroHotStart();

    /*!
    ** \brief Remove timeseries if ts-generator is enabled
    */
    void removeTimeseriesIfTSGeneratorEnabled();
    //@}

    //! \name
    //@{
    /*!
    ** \brief Check for filename limits restriction on some platforms
    **
    ** On Windows, we can use filename with more than 256 characters.
    ** theoretically possible, but non working.
    **
    ** \param output True for checking output filenames, false for input
    ** \param chfolder The study folder to take into consideration
    */
    bool checkForFilenameLimits(bool output, const YString& chfolder = nullptr) const;
    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Load all matrices within the binding constraints if not already done
    **
    ** This method is required by the interface when a saveAs is performed
    */
    void ensureDataAreLoadedForAllBindingConstraints();

    /*!
    ** \brief Get the amound of memory consummed by the study (in bytes)
    */
    uint64_t memoryUsage() const;

    //! \name Logs
    //@{
    /*!
    ** \brief Copy the log file from the 'logs' folder to the current output folder
    */
    void importLogsToOutputFolder() const;
    //@}
    //! \name Check validity of Min Stable Power of Thermal Clusters
    //@{
    /*!
    ** \brief Check validity between Min Stable Power and capacity modulation for each thermal
    *cluster in the all area in the study.
    ** Should be call then all inforation is suplied in to the thermal clusters.
    */
    void computePThetaInfForThermalClusters() const;

    void prepareWriter(Benchmarking::IDurationCollector* duration_collector);

    //! Header (general information about the study)
    StudyHeader header;

    //! \name Paths
    //@{
    //! The source folder of the study
    YString folder;
    //! The input folder
    YString folderInput;
    //! The output folder
    YString folderOutput;
    //! The settings folder
    YString folderSettings;
    //@}

    //! \name Simulation
    //@{
    //! The current Simulation
    SimulationComments simulationComments;

    int64_t pStartTime;
    // Used in GUI and solver
    // ----------------------
    // Maximum number of years in a set of parallel years.
    // It is a possible reduction of the raw number of cores set by user (simulation cores level).
    // This raw number of cores is possibly reduced by the smallest TS refresh span or the total
    // number of MC years. In GUI, used for RAM estimation only. In solver, it is the max number of
    // years (actually run, not skipped) a set of parallel years can contain.
    uint maxNbYearsInParallel = 1;

    // Used in GUI only.
    // ----------------
    // Allows storing the maximum number of years in a set of parallel years.
    // Useful to estimate the RAM when the run window's parallel mode is chosen.
    uint maxNbYearsInParallel_save = 0;

    // Used in GUI and solver.
    // ----------------------
    // Raw numbers of cores (== nb of MC years run in parallel) based on the number
    // of cores level (see advanced parameters).
    uint nbYearsParallelRaw = 1;

    // Used in GUI only.
    // -----------------
    // Minimum number of years in a set of parallel years.
    // It is a possible reduction of the raw number of cores set by user (simulation cores level).
    // This raw number of cores can be reduced :
    //	- by the smallest TS refresh span
    //	- by the smallest interval between TS refreshes
    //	- In the Run window, if either Default or swap support mode is enabled, then parallel
    //	  computation is disabled, and the number of cores is 1
    // Useful to populate the run window's simulation cores field.
    uint minNbYearsInParallel = 0;

    // Used in GUI only.
    // ----------------
    // Allows storing the minimum number of years in a set of parallel years.
    // Useful to populate the run window's simulation cores field.
    uint minNbYearsInParallel_save = 0;

    //! Parameters
    Parameters parameters;

    /*!
    ** \brief Calendar
    **
    ** When launched from the solver, this calendar does not take into
    ** consideration the leap year
    */
    Date::Calendar calendar;
    /*!
    ** \brief Calendar output
    **
    ** With leap year mode (if any), for the output results
    */
    Date::Calendar calendarOutput;
    //@}

    //! \name Areas
    //@{
    //! All available areas
    AreaList areas;
    //@}

    //! \name Binding constraints
    //@{
    //! Binding constraints
    BindingConstraintsRepository bindingConstraints;
    BindingConstraintGroupRepository bindingConstraintsGroups;
    //@}

    //! \name Correlation matrices used by the prepro
    //@{
    //! Correlation matrix for the load time series generated by the prepro
    Correlation preproLoadCorrelation;
    //! Correlation matrix for the solar time series generated by the prepro
    Correlation preproSolarCorrelation;
    //! Correlation matrix for the wind time series generated by the prepro
    Correlation preproWindCorrelation;
    //! Correlation matrix for the hydro time series generated by the prepro
    Correlation preproHydroCorrelation;
    //@}

    //! \name Groups
    //@{
    //! Sets of areas
    SetsOfAreas setsOfAreas;
    //! Sets of links
    SetsOfLinks setsOfLinks;
    //@}

    //! \name Scenario Builder
    //@{
    //! Rules for building scenarios (can be null)
    ScenarioBuilder::Sets* scenarioRules = nullptr;
    //@}

    Matrix<double> scenarioHydroLevels;

    /*!
    ** \brief Runtime informations
    **
    ** These informations are only needed when a study is processed.
    */
    StudyRuntimeInfos* runtime = nullptr;

    // Antares::Solver::Variable::State* state;

    /*!
    ** \brief Specific data related to the User Interface
    */
    UIRuntimeInfo* uiinfo = nullptr;

    /*!
    ** \brief The file extension for file within the input ('txt' or 'csv')
    **
    ** Since the v3.1, the file extensions in the input have been renamed into .txt,
    ** (instead of .csv)
    */
    FileExtension inputExtension = "txt";

    //! Progression about the current action performed on the study
    mutable Solver::Progression progression;

    /*!
    ** \name Cache
    **
    ** \warning Those variables must not be used outside of a study.
    */
    //@{
    //! A buffer for temporary operations on filename
    mutable YString buffer;
    //! A buffer for temporary operations on large amount of data
    mutable Matrix<>::BufferType dataBuffer;
    //! A buffer used when loading time-series for dealing with filenames (prepro/series only)
    mutable YString bufferLoadingTS;
    //@}

    //! The queue service that runs every set of parallel years
    std::shared_ptr<Yuni::Job::QueueService> pQueueService;

    //! Result writer, required to write residual files (comments, about-the-study, etc.)
    Solver::IResultWriter::Ptr resultWriter = nullptr;

public:
    //! \name TS Generators
    //@{
    /*!
    ** \brief Time-series generators used by the solver
    ** \warning These variables should not be used directly
    */
    void* cacheTSGenerator[timeSeriesCount];
    //@}

    /*!
    ** \brief
    */
    bool gotFatalError = false;

    /*!
    ** \brief A non-zero value when the study will be used by the solver
    **
    ** If it is the case, some data may not be kept in memory, and some calculations
    ** must be done.
    */
    const bool usedByTheSolver;

protected:
    //! \name Loading
    //@{
    //! Load a study from a folder
    bool internalLoadFromFolder(const YString& path, const StudyLoadOptions& options);
    //! Load the study header
    bool internalLoadHeader(const YString& folder);
    //! Load all correlation matrices
    bool internalLoadCorrelationMatrices(const StudyLoadOptions& options);
    //! Load all binding constraints
    virtual bool internalLoadBindingConstraints(const StudyLoadOptions& options);
    //! Load all set of areas and links
    bool internalLoadSets();
    //@}

    bool internalLoadIni(const YString& path, const StudyLoadOptions& options);

    void parameterFiller(const StudyLoadOptions& options);

    //! \name Misc
    //@{
    //! Release all unnecessary buffers
    void reduceMemoryUsage();
    //@}

}; // class Study

/*!
** \brief Icon to use for studies
*/
extern YString StudyIconFile;

YString StudyCreateOutputPath(StudyMode mode,
                              ResultFormat fmt,
                              const YString& folder,
                              const YString& label,
                              int64_t startTime);
} // namespace Antares::Data


#include "study.hxx"
#include "runtime.h"

#endif /* __ANTARES_LIBS_STUDY_STUDY_H__ */
