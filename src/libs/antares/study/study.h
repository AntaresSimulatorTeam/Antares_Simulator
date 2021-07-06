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
#ifndef __ANTARES_LIBS_STUDY_STUDY_H__
#define __ANTARES_LIBS_STUDY_STUDY_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/thread/thread.h>
#include <yuni/core/noncopyable.h>

#include "../antares.h"
#include "../object/object.h"
#include "fwd.h"

#include "simulation.h"
#include "parameters.h"
#include "constraint/constraint.h"
#include "header.h"
#include "version.h"
#include "sets.h"
#include "ui-runtimeinfos.h"
#include "../array/correlation.h"
#include "progression/progression.h"
#include "load-options.h"
#include "../date.h"

#include "equipments/equipments.h" // experimental

//# include "../../../solver/variable/state.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Antares Study
*/
class Study final : public Yuni::NonCopyable<Study>, public IObject
{
public:
    //! The most suitable smart pointer for the class
    typedef IObject::SmartPtr<Study>::Ptr Ptr;
    // typedef Yuni::SmartPtr<Study>  Ptr;
    //! Set of studies
    typedef std::set<Ptr> Set;
    //! List of studies
    typedef std::list<Ptr> List;

    //! A single set of areas
    // CompareAreaName : to control the order of areas in a set of areas. This order can have an
    // effect, even if tiny, on the results of aggregations.
    typedef std::set<Area*, CompareAreaName> SingleSetOfAreas;

    //! Multiple sets of areas
    typedef Antares::Data::Sets<SingleSetOfAreas> SetsOfAreas;

    //! A single set of links
    typedef std::set<AreaLink*> SingleSetOfLinks;
    //! Multiple sets of links
    typedef Antares::Data::Sets<SingleSetOfLinks> SetsOfLinks;

    //! List of disabled areas
    typedef std::set<AreaName> DisabledAreaList;
    //! List of disabled links
    typedef std::set<AreaLinkName> DisabledAreaLinkList;
    //! List of disabled thermal clusters
    typedef std::set<ClusterName> DisabledThermalClusterList;

    //! Extension filename
    typedef Yuni::CString<8, false> FileExtension;

public:
    /*!
    ** \brief Operations related to the global current study
    **
    ** \warning These methods should be removed as soon as possible
    */
    struct Current
    {
        /*!
        ** \brief Retrieve the current Study (if any)
        */
        static Study::Ptr Get();
        /*!
        ** \brief Set the current study
        */
        static void Set(Study::Ptr study);
        //! Get if the current study is valid
        static bool Valid();

    }; // Current

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
    explicit Study(bool forTheSolver = false);
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
    bool invalidate(bool reload = false) const;

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
    bool areaFindNameForANewArea(AreaName& out, const AreaName& basename);

    /*!
    ** \brief Add an area and make all required initialization
    **
    ** It is the safe way to add an area and it is mainly used by the GUI
    **
    ** \param name The name of the new area
    ** \return A pointer to a new area, or NULL if the operation failed
    */
    Area* areaAdd(const AreaName& name);

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

    //! \name Thermal clusters
    //@{
    /*!
    ** \brief Rename a thermal cluster
    **
    ** \param cluster The cluster
    ** \return True if the operation succeeded, false otherwise
    */
    // TODO FO : clusterRename
    bool thermalClusterRename(Cluster* cluster, ClusterName newName, bool force = false);
    //@}

    //! \name Renewable clusters
    //@{
    /*!
    ** \brief Rename a renewable cluster
    **
    ** \param cluster The cluster
    ** \return True if the operation succeeded, false otherwise
    */
    bool renewableClusterRename(RenewableCluster* cluster,
                                ClusterName newName,
                                bool force = false);
    //@}

    //! \name Read-only
    //@{
    /*!
    ** \brief Get if the study is in readonly mode
    */
    bool readonly() const;
    //@}

    //! \name Locks
    //@{
    /*!
    ** \brief Release all locks held by the study
    **
    ** This method should only be used when the program want to abort
    ** immediatly without freing allocated data.
    */
    void releaseAllLocks();
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
    template<int TimeSeriesT>
    bool storeTimeSeriesNumbers();
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
    bool prepareOutput();

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
    ** \brief Ensure data for time series/prepro are initalized if they should be
    **
    ** It initializes data for each area so it would be better to call this
    ** routine when areas are already loaded.
    */
    void ensureDataAreInitializedAccordingParameters();

    /*!
    ** \brief Ensure data for time series/prepro are initalized
    **
    ** It initializes data (without any exceptions) for each area so it would be
    ** better to call this routine when areas are already loaded.
    **
    ** This routine should only be used by the interface.
    */
    void ensureDataAreAllInitialized();

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

    //! \name Simulation output Files creation
    //@{
    /*!
    ** \brief Create and open (`w+`) a file into the output for dumping the current linear problem
    **
    **
    ** \return a FILE structure (which may be null if any error occured)
    */
    FILE* createFileIntoOutputWithExtension(const YString& prefix,
                                            const YString& extension,
                                            uint numSpace) const;

    /*!
    ** \brief Create and open (`w+`) a file into the output for dumping the current linear problem
    **
    ** This file should receive a linear problem using the MPS file
    ** format. (see `opt/AppelDuSolveurLineairePasVariable`)
    **
    ** \return a FILE structure (which may be null if any error occured)
    */
    FILE* createMPSFileIntoOutput(uint numSpace) const
    {
        return createFileIntoOutputWithExtension("problem", "mps", numSpace);
    };
    //@}

    /*!
    ** \brief Create and open (`w+`) a file into the output for dumping the current linear problem
    **
    ** This file should receive a linear problem using the MPS file
    ** format. (see `opt/AppelDuSolveurLineairePasVariable`)
    **
    ** \return a FILE structure (which may be null if any error occured)
    */
    FILE* createCriterionFileIntoOutput(uint numSpace) const
    {
        return createFileIntoOutputWithExtension("criterion", "txt", numSpace);
    };
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
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Estimate the memory required by the input to launch a simulation
    **
    ** The real amount of memory required to launch this study
    ** will be less than the returned value, but in the worst case
    ** it can be equal (or nearly).
    **
    ** \param mode The mode of the study
    ** \return A size in bytes, -1 when an error has occured.
    */
    void estimateMemoryUsageForInput(StudyMemoryUsage& u) const;

    /*!
    ** \brief Estimate the memory required by the output to launch a simulation
    **
    ** The real amount of memory required to launch this study
    ** will be less than the returned value, but in the worst case
    ** it can be equal (or nearly).
    **
    ** \param mode The mode of the study
    ** \return A size in bytes, -1 when an error has occured.
    */
    void estimateMemoryUsageForOutput(StudyMemoryUsage& u) const;

    /*!
    ** \brief Create a thread to estimate the memory footprint of the input
    **
    ** This thread is actually a way to process in the background
    ** all costly operations and to avoid the freeze from the interface
    */
    Yuni::Thread::IThread::Ptr createThreadToEstimateInputMemoryUsage() const;
    //@}

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
    bool areasThermalClustersMinStablePowerValidity(std::map<int, YString>& areaClusterNames) const;

public:
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
    Simulation simulation;

    // Used in GUI and solver
    // ----------------------
    // Maximum number of years in a set of parallel years.
    // It is a possible reduction of the raw number of cores set by user (simulation cores level).
    // This raw number of cores is possibly reduced by the smallest TS refresh span or the total
    // number of MC years. In GUI, used for RAM estimation only. In solver, it is the max number of
    // years (actually run, not skipped) a set of parallel years can contain.
    uint maxNbYearsInParallel;

    // Used in GUI only.
    // ----------------
    // Allows storing the maximum number of years in a set of parallel years.
    // Useful to estimate the RAM when the run window's parallel mode is chosen.
    uint maxNbYearsInParallel_save;

    // Used in GUI and solver.
    // ----------------------
    // Raw numbers of cores (== nb of MC years run in parallel) based on the number
    // of cores level (see advanced parameters).
    uint nbYearsParallelRaw;

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
    uint minNbYearsInParallel;

    // Used in GUI only.
    // ----------------
    // Allows storing the minimum number of years in a set of parallel years.
    // Useful to populate the run window's simulation cores field.
    uint minNbYearsInParallel_save;

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
    BindConstList bindingConstraints;
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
    ScenarioBuilder::Sets* scenarioRules;
    //@}

    Matrix<double> scenarioHydroLevels;

    /*!
    ** \brief Runtime informations
    **
    ** These informations are only needed when a study is processed.
    */
    StudyRuntimeInfos* runtime;

    // Antares::Solver::Variable::State* state;

    /*!
    ** \brief Specific data related to the User Interface
    */
    UIRuntimeInfo* uiinfo;

    /*!
    ** \brief The file extension for file within the input ('txt' or 'csv')
    **
    ** Since the v3.1, the file extensions in the input have been renamed into .txt,
    ** (instead of .csv)
    */
    FileExtension inputExtension;

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

public:
    //! \name TS Generators
    //@{
    /*!
    ** \brief Time-series generators used by the solver
    ** \warning These variables should not be used directly
    */
    void* cacheTSGenerator[timeSeriesCount];
    //@}

    //! \name Layers
    //@{
    //! All available layers
    std::map<size_t, std::string> layers;
    //@}
    size_t activeLayerID;
    bool showAllLayer;
    /*!
    ** \brief
    */
    bool gotFatalError;

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
    bool internalLoadBindingConstraints(const StudyLoadOptions& options);
    //! Load all set of areas and links
    bool internalLoadSets();
    //@}

    //! \name Misc
    //@{
    //! Reset the input extension according the study version
    void inputExtensionCompatibility();
    //! Release all unnecessary buffers
    void reduceMemoryUsage();
    //@}

private:
    //! Load all layers
    bool saveLayers(const AnyString& filename);
    void loadLayers(const AnyString& filename);
    //! \name Disabled items
    //@{
    //! List of all disabled areas
    // DisabledAreaList         pDisabledAreaList;
    //! List of all disabled area links
    // DisabledAreaLinkList     pDisabledAreaLinkList;
    //! List of all disabled thermal clusters
    // DisabledThermalClusterList pDisabledThermalClusterList;
    //@}

}; // class Study

/*!
** \brief Icon to use for studies
*/
extern YString StudyIconFile;

/*!
** \brief Ensure data for load time-series are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataLoadTimeSeries(Study* s);

/*!
** \brief Ensure data for load prepro are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
**
** \warning Actually do nothing as long as the prepro is not implemented
**   for the load
*/
void StudyEnsureDataLoadPrepro(Study* s);

/*!
** \brief Ensure data for solar time-series are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataSolarTimeSeries(Study* s);

/*!
** \brief Ensure data for solar prepro are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
**
** \warning Actually do nothing as long as the prepro is not implemented
**   for the load
*/
void StudyEnsureDataSolarPrepro(Study* s);

/*!
** \brief Ensure data for wind time-series are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataWindTimeSeries(Study* s);

/*!
** \brief Ensure data for wind prepro are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataWindPrepro(Study* s);

/*!
** \brief Ensure data for hydro time-series are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataHydroTimeSeries(Study* s);

/*!
** \brief Ensure data for hydro prepro are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataHydroPrepro(Study* s);

/*!
** \brief Ensure data for thermal time-series are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataThermalTimeSeries(Study* s);

/*!
** \brief Ensure data for thermal prepro are initialized
** \ingroup study
**
** \param s The study structure where data are stored
** \see Study::ensureDataAreAllInitializedAccordingParameters()
*/
void StudyEnsureDataThermalPrepro(Study* s);

} // namespace Data
} // namespace Antares

#include "study.hxx"
#include "runtime.h"

#endif /* __ANTARES_LIBS_STUDY_STUDY_H__ */
