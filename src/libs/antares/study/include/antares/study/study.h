// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_STUDY_H__
#define __ANTARES_LIBS_STUDY_STUDY_H__

#include <memory>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <yuni/job/queue/service.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/correlation/correlation.h>
#include <antares/date/date.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/runtime/runtime.h>
#include <antares/writer/i_writer.h>
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

#include "area/store-timeseries-numbers.h"
#include "binding_constraint/BindingConstraint.h"
#include "fwd.h"
#include "header.h"
#include "parameters.h"
#include "sets.h"

namespace Antares::Data
{
/*!
** \brief Antares Study
*/

class Study: public Yuni::NonCopyable<Study>
{
public:
    using Ptr = std::shared_ptr<Study>;
    //! Set of studies
    using Set = std::set<Ptr>;
    //! List of studies
    using List = std::list<Ptr>;

    //! Multiple sets of areas
    using SetsOfAreas = Antares::Data::Sets;

    //! Extension filename
    using FileExtension = std::string;

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

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Study();
    //! Destructor
    virtual ~Study();
    //@}

    /*!
    ** \brief Relocate the study into a new folder
    **
    ** This method does not have any effect except modifying
    ** internal variables (`folder`, `folderInput`, ...).
    */
    void relocate(const std::filesystem::path& newFolder);

    /*!
    ** \brief Load a study from a folder
    **
    ** \param path The path where data are located
    ** \return True if succeeded, false otherwise
    */
    bool loadFromFolder(const std::string& path,
                        const StudyLoadOptions& options,
                        Benchmarking::DurationCollector& durationCollector);

    /*!
    ** \brief Clear all ressources held by the study
    */
    void clear();

    /*!
    ** \brief Save the study into a folder
    **
    ** \param folder The folder where to write data
    ** \return True if succeeded, false otherwise
    */
    bool saveToFolder(const AnyString& newfolder);
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
    */
    Area* areaAdd(const AreaName& name);

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
    template<TimeSeriesType TimeSeriesT>
    void storeTimeSeriesNumbers(Solver::IResultWriter& resultWriter) const
    {
        storeTimeseriesNumbers<TimeSeriesT>(resultWriter, areas);
    }

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

    void saveAboutTheStudy(Solver::IResultWriter& resultWriter);

    //@}

    //! \name Time-series Generators
    //@{
    /*!
    ** \brief Destroy all data of the TS generator '@TS'
    */
    template<TimeSeriesType TS>
    inline void destroyTSGeneratorData()
    {
    }

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

    //! \name Internal Data TS-Generators / Series
    //@{

    /*!
    ** \brief Computes a raw number of cores table.
    **
    ** The table associetes a raw number of cores to each level ("min", "low", "med", "high",
    *"max").
    **
    */
    unsigned getNumberOfCoresPerMode(unsigned nbLogicalCores, int ncMode);

    /*!
    ** \brief Computes number of cores
    **
    ** From the "Number of Cores" level (in GUI --> Advanced parameters), computes
    ** the real numbers of logical cores to be involved in the MC years parallelisation.
    */
    void getNumberOfCores(const bool forceParallel, const uint nbYearsParallelForced);

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
    bool checkForFilenameLimits() const;
    //@}

    //! \name Logs
    //@{
    /*!
    ** \brief Copy the log file from the 'logs' folder to the current output folder
    */
    void importLogsToOutputFolder(Solver::IResultWriter& resultWriter) const;
    //@}
    //! \name Check validity of Min Stable Power of Thermal Clusters
    //@{
    /*!
    ** \brief Check validity between Min Stable Power and capacity modulation for each thermal
    *cluster in the all area in the study.
    ** Should be call then all inforation is suplied in to the thermal clusters.
    */
    void computePThetaInfForThermalClusters() const;

    //! Header (general information about the study)
    StudyHeader header;

    //! \name Paths
    //@{
    //! The source folder of the study
    std::filesystem::path folder;
    //! The input folder
    std::filesystem::path folderInput;
    //! The output folder
    std::filesystem::path folderOutput;
    //! The settings folder
    std::filesystem::path folderSettings;
    //@}

    //! \name Simulation
    //@{
    //! Simulation comments (content of comments.txt)
    std::string simulationComments;
    //! Simulation name
    std::string simulationName;

    int64_t pStartTime;
    //! Maximum number of years in a set of parallel years.
    // It is a possible reduction of the raw number of cores set by user (simulation cores level).
    // In solver, it is the max number of years (actually run, not skipped) a set of parallel
    // years can contain.
    uint maxNbYearsInParallel = 1;

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
    //@}

    //! \name Scenario Builder
    //@{
    //! Rules for building scenarios (can be null)
    std::unique_ptr<ScenarioBuilder::Sets> scenarioRules;
    //@}

    TimeSeries::TS scenarioInitialHydroLevels;
    // Hydro Final Levels
    TimeSeries::TS scenarioFinalHydroLevels;
    /*!
    ** \brief Runtime informations
    **
    ** These informations are only needed when a study is processed.
    */
    StudyRuntimeInfos runtime;

    /*!
    ** \brief The file extension for file within the input ('txt' or 'csv')
    **
    ** Since the v3.1, the file extensions in the input have been renamed into .txt,
    ** (instead of .csv)
    */
    FileExtension inputExtension = "txt";

    /*!
    ** \name Cache
    */

    /*!
    ** \brief Mark the whole study as modified
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

    //! \name TS Generators
    //@{
    /*!
    ** \brief Time-series generators used by the solver
    ** \warning These variables should not be used directly
    */
    void* cacheTSGenerator[timeSeriesCount];

    //@}

    Solver::ModelerData* getModelerData() const
    {
        return modelerInput_.get();
    }

    void setModelerData(std::unique_ptr<Solver::ModelerData> modelerData)
    {
        modelerInput_ = std::move(modelerData);
    }

protected:
    //! \name Loading
    //@{
    //! Load a study from a folder
    bool internalLoadFromFolder(const std::filesystem::path& path,
                                const StudyLoadOptions& options,
                                Benchmarking::DurationCollector& durationCollector);
    //! Load the study header
    bool internalLoadHeader(const std::filesystem::path& folder);
    //! Load all correlation matrices
    bool internalLoadCorrelationMatrices();
    //! Load all binding constraints
    virtual bool internalLoadBindingConstraints(const StudyLoadOptions& options);
    //! Load all set of areas and links
    bool internalLoadSets();
    //@}

    bool internalLoadIni(const std::filesystem::path& path, const StudyLoadOptions& options);

    //! Load extra modeler components for hybrid studies and verify compatibility
    void loadModelerComponents();
    void checkModelerDataCompatibility() const;

    void parameterFiller(const StudyLoadOptions& options);

    //! \name Misc
    //@{
    //! Release all unnecessary buffers
    void reduceMemoryUsage();
    //@}

private:
    std::unique_ptr<Solver::ModelerData> modelerInput_;
}; // class Study

std::filesystem::path StudyCreateOutputPath(SimulationMode mode,
                                            ResultFormat fmt,
                                            const std::filesystem::path& folder,
                                            const std::string& label,
                                            const std::tm& startTime);
} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_STUDY_H__ */
