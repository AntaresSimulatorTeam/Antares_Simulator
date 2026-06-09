// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/study.h"

#include <cassert>
#include <climits>
#include <cmath> // For use of floor(...) and ceil(...)
#include <ctime>
#include <sstream> // std::ostringstream
#include <string>
#include <thread>

#include <antares/writer/writer_factory.h>
#include "antares/study/area/constants.h"
#include "antares/study/scenario-builder/sets.h"
#include "antares/utils/utils.h"

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

Study::Study():
    areas(*this),
    pQueueService(std::make_shared<Yuni::Job::QueueService>())
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

    bindingConstraintsGroups.clear();

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

    // avoid overwriting existing output by adding a suffix (-2, -3, etc.)
    if (!Utils::generatePathWithSuffix(folderOutput, suffix))
    {
        throw Error::LoadingError("Output folder already exists: " + folderOutput.string());
    }

    return folderOutput;
}

void Study::prepareOutput()
{
    pStartTime = std::time(nullptr);

    if (parameters.noOutput)
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
    fs::path aboutPath = fs::path("about-the-study") / "comments.txt";
    resultWriter.addEntryFromBuffer(aboutPath, simulationComments);

    // Write the header as a reminder
    {
        aboutPath = fs::path("about-the-study") / "study.ini";
        Antares::IniFile ini;
        header.CopySettingsToIni(ini, false);

        std::string writeBuffer = ini.toString();
        resultWriter.addEntryFromBuffer(aboutPath, writeBuffer);
    }

    // Write parameters.ini
    {
        fs::path dest = fs::path("about-the-study") / "parameters.ini";
        fs::path src = fs::path(folderSettings) / "generaldata.ini";
        resultWriter.addEntryFromFile(dest, src);
    }

    // antares-output.info
    aboutPath = "info.antares-output";
    std::ostringstream f;
    char timeBuffer[64];
    time_t startTime = static_cast<time_t>(pStartTime);
    std::tm timeInfo{};
#ifdef _WIN32
    localtime_s(&timeInfo, &startTime);
#else
    localtime_r(&startTime, &timeInfo);
#endif
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y.%m.%d - %H:%M", &timeInfo);
    std::string startTimeStr(timeBuffer);
    f << "[general]";
    f << "\nversion = " << StudyVersion::latest().toString();
    f << "\nname = " << simulationName;
    f << "\nmode = " << SimulationModeToCString(parameters.mode);
    f << "\ndate = " << startTimeStr;
    f << "\ntitle = " << startTimeStr;
    f << "\ntimestamp = " << pStartTime;
    f << "\n\n";
    std::string output = f.str();
    resultWriter.addEntryFromBuffer(aboutPath, output);

    if (!parameters.noOutput)
    {
        // Write all available areas as a reminder
        {
            std::string buffer;
            aboutPath = fs::path("about-the-study") / "areas.txt";
            for (auto i = setsOfAreas.begin(); i != setsOfAreas.end(); ++i)
            {
                if (setsOfAreas.hasOutput(i->first))
                {
                    buffer += "@ " + i->first + "\r\n";
                }
            }
            areas.each([&buffer](const Data::Area& area) { buffer += area.name + "\r\n"; });
            resultWriter.addEntryFromBuffer(aboutPath, buffer);
        }

        // Write all available links as a reminder
        {
            aboutPath = fs::path("about-the-study") / "links.txt";
            std::ostringstream linkBuffer;
            areas.saveLinkListToBuffer(linkBuffer);
            std::string linkContent = linkBuffer.str();
            resultWriter.addEntryFromBuffer(aboutPath, linkContent);
        }
    }
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
#ifdef _WIN32
        limit = 255, // zero-terminated
#else
        limit = 4090,
#endif
    };

    if (areas.empty())
    {
        return true;
    }

    std::string linkname;
    std::string areaname;

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
                  linkname += i->second->from->id;
                  linkname += " - "; // 3
                  linkname += i->second->with->id;
              }
          }
      });

    fs::path filename = folder / "output";

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
            filename /= (parameters.economy() ? "economy" : "adequacy");
            filename /= fs::path("mc-all") / "areas" / areaname / "values-hourly.txt";
        }
    }
    else
    {
        // The maximum filename should be obtained with links :
        // economy/mc-ind/00001/links/pyrennees\ -\ languedocroussillon/values-hourly.txt
        filename /= (parameters.adequacy() ? "adequacy" : "economy");
        filename /= fs::path("mc-all") / "links" / linkname / "values-hourly.txt";
    }

    auto filenameStr = filename.string();
    if (not filenameStr.empty() and filenameStr.size() >= limit)
    {
        logs.error() << "OS Maximum path length limitation obtained with the link '" << linkname
                     << "' (got " << filenameStr.size() << " characters)";
        logs.error() << "You may experience problems while accessing to this file: " << filenameStr;
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
