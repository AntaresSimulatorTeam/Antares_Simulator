// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/study.h"

#include <cassert>
#include <climits>
#include <cmath> // For use of floor(...) and ceil(...)
#include <ctime>
#include <sstream> // std::ostringstream
#include <thread>

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/system/cpu.h> // For use of Yuni::System::CPU::Count()
#include <yuni/core/system/windows.hdr.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/io/file.h>

#include <antares/writer/writer_factory.h>
#include "antares/antares/antares.h"
#include "antares/study/area/constants.h"
#include "antares/study/runtime.h"
#include "antares/study/scenario-builder/sets.h"
#include "antares/utils/utils.h"

using namespace Yuni;

#define SEP IO::Separator

namespace fs = std::filesystem;

namespace Antares::Data
{
//! Clear then shrink a string
template<class StringT>
static inline void ClearAndShrink(StringT& string)
{
    string.clear();
    string.shrink();
}

Study::Study(bool forTheSolver):
    areas(*this),
    pQueueService(std::make_shared<Yuni::Job::QueueService>()),
    usedByTheSolver(forTheSolver)
{
    // TS generators
    for (uint i = 0; i != timeSeriesCount; ++i)
    {
        cacheTSGenerator[i] = nullptr;
    }

    // Correlation names
    preproLoadCorrelation.correlationName = "Correlation: Load";
    preproSolarCorrelation.correlationName = "Correlation: Solar";
    preproWindCorrelation.correlationName = "Correlation: Wind";
    preproHydroCorrelation.correlationName = "Correlation: Hydro";
    // TS
    preproLoadCorrelation.timeSeries = timeSeriesLoad;
    preproSolarCorrelation.timeSeries = timeSeriesSolar;
    preproWindCorrelation.timeSeries = timeSeriesWind;
    preproHydroCorrelation.timeSeries = timeSeriesHydro;
}

Study::~Study()
{
    clear();
}

void Study::clear()
{
    scenarioRules.reset();

    // areas
    setsOfAreas.clear();

    preproLoadCorrelation.clear();
    preproSolarCorrelation.clear();
    preproWindCorrelation.clear();
    preproHydroCorrelation.clear();

    bindingConstraints.clear();
    bindingConstraintsGroups.clear();
    areas.clear();

    // no folder
    ClearAndShrink(header.caption);
    ClearAndShrink(header.author);
    ClearAndShrink(header.editor);
    folder.clear();
    folderInput.clear();
    folderOutput.clear();
    folderSettings.clear();
    inputExtension.clear();
}

void Study::reduceMemoryUsage()
{
    ClearAndShrink(buffer);
    ClearAndShrink(dataBuffer);
    ClearAndShrink(bufferLoadingTS);
}

unsigned Study::getNumberOfCoresPerMode(unsigned nbLogicalCores, int ncMode)
{
    if (!nbLogicalCores)
    {
        logs.fatal() << "Number of logical cores available is 0.";
        return 1;
    }

    switch (ncMode)
    {
    case ncMin:
        return 1;
    case ncLow:
        return std::ceil(nbLogicalCores / 4.);
    case ncAvg:
        return std::ceil(nbLogicalCores / 2.);
    case ncHigh:
        return std::ceil(3 * nbLogicalCores / 4.);
    case ncMax:
        return nbLogicalCores;
    default:
        logs.fatal() << "Simulation cores level not correct : " << ncMode;
        break;
    }

    return 1;
}

/// Getting the number of parallel years based on the number of cores level
void Study::getNumberOfCores(const bool forceParallel, const uint nbYearsParallelForced)
{
    auto& p = parameters;
    unsigned nbLogicalCores = std::thread::hardware_concurrency();
    maxNbYearsInParallel = getNumberOfCoresPerMode(nbLogicalCores, parameters.nbCores.ncMode);

    // In case solver option '--force-parallel n' is used, previous computation is overridden.
    if (forceParallel)
    {
        maxNbYearsInParallel = nbYearsParallelForced;
    }

    // Limiting the number of parallel years by the total number of years
    if (p.nbYears < maxNbYearsInParallel)
    {
        maxNbYearsInParallel = p.nbYears;
    }

    if (maxNbYearsInParallel == 0)
    {
        maxNbYearsInParallel = 1;
    }
}

bool Study::initializeRuntimeInfos()
{
    return runtime.loadFromStudy(*this);
}

void Study::performTransformationsBeforeLaunchingSimulation()
{
    // Those computations are also made from the TS-Generator (ts-generator/xcast/xcast.cpp)
    logs.debug();
    logs.debug() << "applying transformations required by the simulation...";
    logs.debug() << "  > adding DSM values";

    // ForEach area
    areas.each(
      [this](Data::Area& area)
      {
          if (not parameters.geographicTrimming)
          {
              // reset filtering
              area.filterSynthesis = (uint)filterAll;
              area.filterYearByYear = (uint)filterAll;
          }

          // Informations about time-series for the load
          auto& matrix = area.load.series.timeSeries;
          auto& dsmvalues = area.reserves[fhrDSM];

          // Adding DSM values
          for (uint timeSeries = 0; timeSeries < matrix.width; ++timeSeries)
          {
              auto& perHour = matrix[timeSeries];
              for (uint h = 0; h < matrix.height; ++h)
              {
                  perHour[h] += dsmvalues[h];
                  // MBO - 13/05/2014 - #20
                  // Starting v4.5 load can be negative
                  /*if (perHour[h] < 0.)
                  {
                          logs.warning() << area.id << ", hour " << h << ": `load - dsm` can not be
                  negative. Reset to 0"; perHour[h] = 0.;
                  }*/
              }
          }
      });
}

// This function is a helper. It should be completed when adding new formats
static std::string getOutputSuffix(ResultFormat fmt)
{
    switch (fmt)
    {
    case zipArchive:
        return ".zip";
    default:
        return "";
    }
}

fs::path StudyCreateOutputPath(SimulationMode mode,
                               ResultFormat fmt,
                               const fs::path& baseOutFolder,
                               const std::string& label,
                               const std::tm& startTime)
{
    if (fmt == ResultFormat::inMemory)
    {
        return "no_output";
    }

    auto suffix = getOutputSuffix(fmt);

    // Determining the new output folder
    // This folder is composed by the name of the simulation + the current date/time
    fs::path folderOutput = baseOutFolder / formatTime(startTime, "%Y%m%d-%H%M");

    switch (mode)
    {
    case SimulationMode::Economy:
        folderOutput += "eco";
        break;
    case SimulationMode::Adequacy:
        folderOutput += "adq";
        break;
    case SimulationMode::Expansion:
        folderOutput += "exp";
        break;
    case SimulationMode::Unknown:
        break;
    }

    // Folder output
    if (not label.empty())
    {
        folderOutput += '-' + transformNameIntoID(label);
    }

    std::string outpath = folderOutput.string() + suffix;
    // avoid creating the same output twice
    if (fs::exists(outpath))
    {
        std::string newpath;
        uint index = 1; // will start from 2
        do
        {
            ++index;
            newpath = folderOutput.string() + '-' + std::to_string(index) + suffix;
        } while (fs::exists(newpath) and index < 2000);

        folderOutput += '-' + std::to_string(index);
    }
    return folderOutput;
}

void Study::prepareOutput()
{
    pStartTime = DateTime::Now();

    if (parameters.noOutput || !usedByTheSolver)
    {
        return;
    }
    fs::path baseFolderOutput = folder / "output";

    folderOutput = StudyCreateOutputPath(parameters.mode,
                                         parameters.resultFormat,
                                         baseFolderOutput,
                                         simulationName,
                                         getCurrentTime());

    logs.info() << "  Output folder : " << folderOutput;
}

void Study::saveAboutTheStudy(Solver::IResultWriter& resultWriter)
{
    String path;
    path.reserve(1024);

    path.clear() << "about-the-study" << SEP << "comments.txt";
    resultWriter.addEntryFromBuffer(path.c_str(), simulationComments);

    // Write the header as a reminder
    {
        path.clear() << "about-the-study" << SEP << "study.ini";
        Antares::IniFile ini;
        header.CopySettingsToIni(ini, false);

        std::string writeBuffer = ini.toString();
        resultWriter.addEntryFromBuffer(path.c_str(), writeBuffer);
    }

    // Write parameters.ini
    {
        String dest;
        dest << "about-the-study" << SEP << "parameters.ini";

        buffer.clear() << folderSettings << SEP << "generaldata.ini";
        resultWriter.addEntryFromFile(dest.c_str(), buffer.c_str());
    }

    // antares-output.info
    path.clear() << "info.antares-output";
    std::ostringstream f;
    String startTimeStr;
    DateTime::TimestampToString(startTimeStr, "%Y.%m.%d - %H:%M", pStartTime);
    f << "[general]";
    f << "\nversion = " << StudyVersion::latest().toString();
    f << "\nname = " << simulationName;
    f << "\nmode = " << SimulationModeToCString(parameters.mode);
    f << "\ndate = " << startTimeStr;
    f << "\ntitle = " << startTimeStr;
    f << "\ntimestamp = " << pStartTime;
    f << "\n\n";
    auto output = f.str();
    resultWriter.addEntryFromBuffer(path.c_str(), output);

    if (usedByTheSolver and !parameters.noOutput)
    {
        // Write all available areas as a reminder
        {
            Yuni::Clob buffer;
            path.clear() << "about-the-study" << SEP << "areas.txt";
            for (auto i = setsOfAreas.begin(); i != setsOfAreas.end(); ++i)
            {
                if (setsOfAreas.hasOutput(i->first))
                {
                    buffer << "@ " << i->first << "\r\n";
                }
            }
            areas.each([&buffer](const Data::Area& area) { buffer << area.name << "\r\n"; });
            std::string content = buffer.c_str();
            resultWriter.addEntryFromBuffer(path.c_str(), content);
        }

        // Write all available links as a reminder
        {
            path.clear() << "about-the-study" << SEP << "links.txt";
            Yuni::Clob buffer;
            areas.saveLinkListToBuffer(buffer);
            std::string content = buffer.c_str();
            resultWriter.addEntryFromBuffer(path.c_str(), content);
        }
    }
}

Area* Study::areaAdd(const AreaName& name)
{
    if (name.empty())
    {
        return nullptr;
    }
    if (CheckForbiddenCharacterInAreaName(name))
    {
        logs.error() << "character '*' is forbidden in area name: `" << name << "`";
        return nullptr;
    }

    // Result
    Area* area = nullptr;
    logs.info() << "adding new area " << name;

    // The new scope is mandatory to rebuild the correlation matrices
    // and the scenario builder data
    {
        // Adding an area
        AreaName newName;
        if (not modifyAreaNameIfAlreadyTaken(newName, name) or newName.empty())
        {
            logs.error() << "Impossible to find a name for a new area";
            return nullptr;
        }

        // Adding an area
        area = addAreaToListOfAreas(areas, newName);
        if (not area)
        {
            return nullptr;
        }

        // Rebuild indexes for all areas
        areas.rebuildIndexes();

        // Default values for the area
        area->createMissingData();
        area->resetToDefaultValues();
    }

    return area;
}

template<>
inline void Study::destroyTSGeneratorData<TimeSeriesType::timeSeriesLoad>()

{
    areas.each([](Data::Area& area) { area.load.prepro.reset(); });
}

template<>
inline void Study::destroyTSGeneratorData<TimeSeriesType::timeSeriesSolar>()
{
    areas.each([](Data::Area& area) { area.solar.prepro.reset(); });
}

template<>
inline void Study::destroyTSGeneratorData<TimeSeriesType::timeSeriesWind>()
{
    areas.each([](Data::Area& area) { area.wind.prepro.reset(); });
}

template<>
inline void Study::destroyTSGeneratorData<TimeSeriesType::timeSeriesHydro>()
{
    areas.each([](Data::Area& area) { area.hydro.prepro.reset(); });
}

void Study::initializeProgressMeter(bool tsGeneratorOnly)
{
    uint years = tsGeneratorOnly ? 1 : (runtime.rangeLimits.year[rangeEnd] + 1);

    unsigned ticksPerYear = 0;
    unsigned ticksPerOutput = 0;

    if (not tsGeneratorOnly)
    {
        // One tick at the begining and 2 at the end of the year
        // Output - Areas
        ticksPerOutput += areas.size();
        // Output - Links
        ticksPerOutput += runtime.interconnectionsCount();
        // Output - digest
        ticksPerOutput += 1;
        ticksPerYear = 1;
    }

    unsigned n;

    for (uint y = 0; y != years; ++y)
    {
        progression.add(y, Solver::Progression::sectYear, ticksPerYear);

        if (parameters.yearByYear)
        {
            progression.add(y, Solver::Progression::sectOutput, ticksPerOutput);
        }
    }

    // Output
    progression.add(Solver::Progression::sectOutput, ticksPerOutput);

    // Import
    n = 0;
    if (0 != (timeSeriesLoad & parameters.exportTimeSeriesInInput))
    {
        n += areas.size();
    }
    if (0 != (timeSeriesSolar & parameters.exportTimeSeriesInInput))
    {
        n += areas.size();
    }
    if (0 != (timeSeriesWind & parameters.exportTimeSeriesInInput))
    {
        n += areas.size();
    }
    if (0 != (timeSeriesHydro & parameters.exportTimeSeriesInInput))
    {
        n += areas.size();
    }
    if (0 != (timeSeriesThermal & parameters.exportTimeSeriesInInput))
    {
        n += areas.size();
    }
    if (n)
    {
        progression.add(Solver::Progression::sectImportTS, n);
    }

    // Needed by the progression meter thread to retrieve properly
    // messages from all MC years
    progression.setNumberOfParallelYears(maxNbYearsInParallel);
}

void Study::relocate(const fs::path& newFolder)
{
    folder = newFolder;
    folderInput = newFolder / "input";
    folderOutput = newFolder / "output";
    folderSettings = newFolder / "settings";
}

void Study::resizeAllTimeseriesNumbers(uint n)
{
    logs.debug() << "  resizing timeseries numbers";
    areas.resizeAllTimeseriesNumbers(n);
    bindingConstraintsGroups.resizeAllTimeseriesNumbers(n);
}

bool Study::checkForFilenameLimits() const
{
    enum
    {
#ifdef YUNI_OS_WINDOWS
        limit = 255, // zero-terminated
#else
        limit = 4090,
#endif
    };

    if (areas.empty())
    {
        return true;
    }

    String linkname;
    String areaname;

    areas.each(
      [&linkname, &areaname](const Area& area)
      {
          if (areaname.size() < area.id.size())
          {
              areaname = area.id;
          }

          auto end = area.links.end();
          for (auto i = area.links.begin(); i != end; ++i)
          {
              auto& link = *(i->second);
              uint len = link.from->id.size() + link.with->id.size();
              len += 3;
              if (len > linkname.size())
              {
                  linkname.clear();
                  linkname << i->second->from->id;
                  linkname << " - "; // 3
                  linkname << i->second->with->id;
              }
          }
      });

    String filename;
    filename << folder << SEP << "output" << SEP;

    if (linkname.empty())
    {
        if (areaname.empty())
        {
            filename.clear();
        }
        else
        {
            // no links : obtained from areas
            // The maximum filename should be obtained with links :
            // Adequacy/mc-all/areas/languedocroussillon/without-network-hourly.txt
            filename << (parameters.economy() ? "economy" : "adequacy") << SEP;
            filename << "mc-all" << SEP << "areas";
            filename << SEP << areaname << SEP;
            filename << "values-hourly.txt";
        }
    }
    else
    {
        // The maximum filename should be obtained with links :
        // economy/mc-ind/00001/links/pyrennees\ -\ languedocroussillon/values-hourly.txt
        filename << (parameters.adequacy() ? "adequacy" : "economy") << SEP;
        filename << "mc-all" << SEP << "links";
        filename << SEP << linkname << SEP << "values-hourly.txt";
    }

    if (not filename.empty() and filename.size() >= limit)
    {
        logs.error() << "OS Maximum path length limitation obtained with the link '" << linkname
                     << "' (got " << filename.size() << " characters)";
        logs.error() << "You may experience problems while accessing to this file: " << filename;
        return false;
    }
    return true;
}

void Study::computePThetaInfForThermalClusters() const
{
    for (uint i = 0; i != this->areas.size(); i++)
    {
        // Alias de la zone courant
        const auto& area = *(this->areas.byIndex[i]);

        for (auto& c: area.thermal.list.each_enabled_and_not_mustrun())
        {
            for (uint k = 0; k < HOURS_PER_YEAR; k++)
            {
                c->PthetaInf[k] = c->modulation[Data::thermalMinGenModulation][k] * c->unitCount
                                  * c->nominalCapacity;
            }
        }
    }
}
} // namespace Antares::Data
