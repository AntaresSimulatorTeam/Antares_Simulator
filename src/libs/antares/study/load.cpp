// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>

#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include "antares/exception/LoadingError.hpp"
#include "antares/study/study.h"
#include "antares/study/version.h"

#include "include/antares/study/fwd.h"

namespace fs = std::filesystem;

namespace Antares::Data
{
bool Study::internalLoadHeader(const fs::path& path)
{
    // Header
    auto headerPath = path / "study.antares";
    if (!header.loadFromFile(headerPath))
    {
        logs.error() << path << ": impossible to open the header file";
        return false;
    }

    // Informations about the study
    logs.info();
    logs.notice() << "Preparing " << header.caption << "...";
    logs.info() << "  detected study version: " << header.version.toString();
    logs.info() << "  from `" << path << '`';
    logs.info();
    return true;
}

bool Study::loadFromFolder(const std::string& path,
                           const StudyLoadOptions& options,
                           Benchmarking::DurationCollector& durationCollector)
{
    fs::path normPath = path;
    normPath = normPath.lexically_normal();
    return internalLoadFromFolder(normPath, options, durationCollector);
}

bool Study::internalLoadIni(const fs::path& path, const StudyLoadOptions& options)
{
    if (!internalLoadHeader(path))
    {
        if (options.loadOnlyNeeded)
        {
            return false;
        }
    }

    // The simulation settings
    if (!simulationComments.loadFromFolder(options))
    {
        if (options.loadOnlyNeeded)
        {
            return false;
        }
    }
    // Load the general data
    fs::path generalDataPath = folderSettings / "generaldata.ini";
    bool errorWhileLoading = !parameters.loadFromFile(generalDataPath, header.version);

    parameters.validateOptions(options);

    parameters.fixBadValues();

    if (errorWhileLoading)
    {
        if (options.loadOnlyNeeded)
        {
            return false;
        }
    }

    return true;
}

void Study::parameterFiller(const StudyLoadOptions& options)
{
    if (usedByTheSolver && !options.prepareOutput)
    {
        parameters.noOutput = true;
        parameters.yearByYear = false;
        parameters.timeSeriesToArchive = 0;
        parameters.storeTimeseriesNumbers = false;
        parameters.synthesis = false;
    }

    // We can not run the simulation if the study folder is not in the latest
    // version and that we would like to re-importe the generated timeseries
    if (usedByTheSolver)
    {
        // We have time-series to import
        if (parameters.exportTimeSeriesInInput && header.version != StudyVersion::latest())
        {
            logs.info() << "Stochastic TS stored in input parametrized."
                           " Disabling Store in input because study is not at latest version."
                           " This prevents writing data in unsupported format at the study version";
            parameters.exportTimeSeriesInInput = 0;
        }
    }

    // This settings can only be enabled from the solver
    // Prepare the output for the study
    prepareOutput(); // will abort early if not usedByTheSolver

    // calendar update
    if (usedByTheSolver)
    {
        calendar.reset({parameters.dayOfThe1stJanuary,
                        parameters.firstWeekday,
                        parameters.firstMonthInYear,
                        false});
    }
    else
    {
        calendar.reset({parameters.dayOfThe1stJanuary,
                        parameters.firstWeekday,
                        parameters.firstMonthInYear,
                        parameters.leapYear});
    }

    calendarOutput.reset({parameters.dayOfThe1stJanuary,
                          parameters.firstWeekday,
                          parameters.firstMonthInYear,
                          parameters.leapYear});

    // Reducing memory footprint
    reduceMemoryUsage();
}

bool Study::internalLoadFromFolder(const fs::path& path,
                                   const StudyLoadOptions& options,
                                   Benchmarking::DurationCollector& durationCollector)
{
    // Check if the path is correct
    if (!fs::exists(path))
    {
        logs.error()
          << path << ": The directory does not exist (or not enough privileges to read the folder)";
        return false;
    }

    // Initialize all internal paths
    relocate(path.string());

    // Reserving enough space in buffer to avoid several calls to realloc
    this->dataBuffer.reserve(4 * 1024 * 1024); // For matrices, reserving 4Mo
    this->bufferLoadingTS.reserve(2096);
    assert(this->bufferLoadingTS.capacity() > 0);

    if (!internalLoadIni(path, options))
    {
        return false;
    }

    bool ret = true;

    durationCollector("study_loading") << [this, &options, &ret]
    {
        // -------------------------
        // Logical cores
        // -------------------------
        // Getting the number of logical cores to use before loading and creating the areas :
        // Areas need this number to be up-to-date at construction.
        getNumberOfCores(options.forceParallel, options.maxNbYearsInParallel);

        // In case parallel mode was not chosen, only 1 core is allowed
        if (!options.enableParallel && !options.forceParallel)
        {
            maxNbYearsInParallel = 1;
        }

        // End logical core --------

        // Areas - Raw Data
        ret = areas.loadFromFolder(options) && ret;

        logs.info() << "Loading correlation matrices...";
        // Correlation matrices
        ret = internalLoadCorrelationMatrices(options) && ret;
        // Binding constraints
        ret = internalLoadBindingConstraints(options) && ret;
        // Sets of areas & links
        ret = internalLoadSets() && ret;

        parameterFiller(options);
    };

    // Modeler components for hybrid studies
    durationCollector("modeler_loading") << [this] { loadModelerComponents(); };

    return ret;
}

void Study::loadModelerComponents()
{
    try
    {
        auto data = Solver::LoadFiles::loadAll(folder);
        if (data.has_value())
        {
            // Move the ModelerData out of the optional to avoid copying
            // (ModelerData contains unique_ptr members and is move-only: it can be moved but not
            // copied).
            modelerInput_ = std::make_unique<Solver::ModelerData>(std::move(*data));
            checkModelerDataCompatibility();
        }
    }
    catch (const Error::LoadingError& e)
    {
        logs.error() << "Error while loading modeler components: " << e.what();
    }
    catch (const std::exception& e)
    {
        logs.error() << "No modeler inputs were loaded";
        logs.error() << "Modeler inputs error: " << e.what();
    }

    if (fs::exists(folder / "parameters.yml"))
    {
        logs.warning() << "parameters.yml ignored, use command line to set solver parameters";
    }
}

/**
 * Checks that the modeler data is compatible with the solver for hybrid studies.
 * Currently, unsupported cases in the solver are :
 * - variables that are not scenario dependent (allowed only in benders-decomposition mode)
 */
void Study::checkModelerDataCompatibility() const
{
    // Scenario-independent variables are allowed in benders-decomposition mode for investment
    // studies
    if (modelerInput_->resolutionMode == Solver::ResolutionMode::BENDERS_DECOMPOSITION)
    {
        return;
    }

    // For sequential-subproblems mode, scenario-independent variables are not supported
    for (auto& component: modelerInput_->system->Components())
    {
        for (auto& variable: component.getModel()->Variables())
        {
            if (!variable.IsScenarioDependent())
            {
                throw Error::LoadingError(fmt::format(
                  "Scenario-independent variables are not supported in hybrid studies with "
                  "sequential-subproblems resolution mode. "
                  "Please review variable \"{}\" in model \"{}\" (used in component \"{}\"). "
                  "Use resolution-mode: benders-decomposition for investment studies.",
                  variable.Id(),
                  component.getModel()->Id(),
                  component.Id()));
            }
        }
    }
}

bool Study::internalLoadCorrelationMatrices(const StudyLoadOptions& options)
{
    // Load
    if (!options.loadOnlyNeeded || timeSeriesLoad & parameters.timeSeriesToGenerate)
    {
        fs::path loadPath = folderInput / "load" / "prepro" / "correlation.ini";
        preproLoadCorrelation.loadFromFile(*this, loadPath.string());
    }

    // Solar
    if (!options.loadOnlyNeeded || timeSeriesSolar & parameters.timeSeriesToGenerate)
    {
        fs::path solarPath = folderInput / "solar" / "prepro" / "correlation.ini";
        preproSolarCorrelation.loadFromFile(*this, solarPath.string());
    }

    // Wind
    if (!options.loadOnlyNeeded || timeSeriesWind & parameters.timeSeriesToGenerate)
    {
        fs::path windPath = folderInput / "wind" / "prepro" / "correlation.ini";
        preproWindCorrelation.loadFromFile(*this, windPath.string());
    }

    // Hydro
    if (!options.loadOnlyNeeded || timeSeriesHydro & parameters.timeSeriesToGenerate)
    {
        fs::path hydroPath = folderInput / "hydro" / "prepro" / "correlation.ini";
        preproHydroCorrelation.loadFromFile(*this, hydroPath.string());
    }
    return true;
}

bool Study::internalLoadBindingConstraints(const StudyLoadOptions& options)
{
    // All checks are performed in 'loadFromFolder'
    // (actually internalLoadFromFolder)
    fs::path constraintPath = folderInput / "bindingconstraints";
    bool r = bindingConstraints.loadFromFolder(*this, options, constraintPath);
    if (r)
    {
        r &= bindingConstraintsGroups.buildFrom(bindingConstraints);
    }
    return (!r && options.loadOnlyNeeded) ? false : r;
}

bool Study::internalLoadSets()
{
    // Set of areas
    logs.info();
    logs.info() << "Loading sets of areas...";

    // filename
    const fs::path setPath = folderInput / "areas" / "sets.ini";

    // Load the rules
    if (setsOfAreas.loadFromFile(setPath))
    {
        // Apply the rules
        SetHandlerAreas handler(areas);
        setsOfAreas.rebuildAllFromRules(handler);
        // Write the results into the logs
        setsOfAreas.dumpToLogs();
        return true;
    }

    logs.warning() << "Impossible to load the sets of areas";
    return false;
}

} // namespace Antares::Data
