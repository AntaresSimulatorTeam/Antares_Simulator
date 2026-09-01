// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cassert>
#include <fstream>
#include <ranges>
#include <string>
#include <yaml-cpp/yaml.h>

#include <boost/algorithm/string/trim.hpp>

#include <antares/inifile/inifile.h>
#include <antares/io/file.h>
#include <antares/logs/logs.h>
#include <antares/study/area/capacityReservation.h>
#include <antares/study/area/scratchpad.h>
#include "antares/array/matrix.h"
#include "antares/study/area/area.h"
#include "antares/study/parts/load/prepro.h"
#include "antares/study/parts/parts.h"
#include "antares/study/study.h"
#include "antares/utils/utils.h"

namespace fs = std::filesystem;

namespace Antares::Data
{
namespace // anonymous
{
static void toLower(std::string& str)
{
    for (char& c: str)
    {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
}

template<typename T>
bool tryParseYamlField(const YAML::Node& node, T& value, const std::string& warningMsg)
{
    try
    {
        value = node.as<T>();
        return true;
    }
    catch (const YAML::Exception&)
    {
        logs.warning() << warningMsg;
        return false;
    }
}

bool readReservesAreaParameters(Area& area, const YAML::Node& params)
{
    bool ret = true;
    for (const auto& param: params)
    {
        std::string key = param.first.as<std::string>();
        toLower(key);

        if (key == "energy-activation-ratio-up")
        {
            if (!tryParseYamlField(
                  param.second,
                  area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.up,
                  area.name + " : invalid maximum energy activation ratio for UP reserves"))
            {
                ret = false;
            }
        }
        else if (key == "energy-activation-ratio-down")
        {
            if (!tryParseYamlField(
                  param.second,
                  area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.down,
                  area.name + " : invalid maximum energy activation ratio for DOWN reserves"))
            {
                ret = false;
            }
        }
        else if (key == "reference-activation-duration-up")
        {
            if (!tryParseYamlField(
                  param.second,
                  area.allCapacityReservations.value().referenceGlobalActivationDuration.up,
                  area.name + " : invalid reference energy activation duration for UP reserves"))
            {
                ret = false;
            }
        }
        else if (key == "reference-activation-duration-down")
        {
            if (!tryParseYamlField(
                  param.second,
                  area.allCapacityReservations.value().referenceGlobalActivationDuration.down,
                  area.name + " : invalid reference energy activation duration for DOWN reserves"))
            {
                ret = false;
            }
        }
        else
        {
            logs.warning() << area.name << " : invalid key " << key
                           << " inside global reserve parameters";
            ret = false;
        }
    }
    return ret;
}

bool readReserveParameters(const fs::path& folderInput, Area& area, const YAML::Node& reserveNode)
{
    bool ret = true;

    std::string reserveName;
    if (!tryParseYamlField(reserveNode["name"],
                           reserveName,
                           area.name + " : reserve missing required 'name' field"))
    {
        return false;
    }

    ReserveID reserveId = transformNameIntoID(reserveName);
    if (area.allCapacityReservations.value().contains(reserveId))
    {
        logs.error() << area.name << " : reserve name already exists for reserve " << reserveName;
        return false;
    }

    CapacityReservation capacityReservation;
    capacityReservation.setName(reserveName);

    for (const auto& entry: reserveNode)
    {
        std::string key = entry.first.as<std::string>();
        toLower(key);

        if (key == "name")
        {
            continue;
        }
        if (key == "failure-cost")
        {
            if (!tryParseYamlField(entry.second,
                                   capacityReservation.unsuppliedCost,
                                   area.name + " : invalid failure cost for reserve "
                                     + reserveName))
            {
                ret = false;
            }
        }
        else if (key == "spillage-cost")
        {
            if (!tryParseYamlField(entry.second,
                                   capacityReservation.spillageCost,
                                   area.name + " : invalid spillage cost for reserve "
                                     + reserveName))
            {
                ret = false;
            }
        }
        else if (key == "power-activation-ratio")
        {
            if (!tryParseYamlField(entry.second,
                                   capacityReservation.powerActivationRatio,
                                   area.name + " : invalid maximum activation ratio for reserve "
                                     + reserveName))
            {
                ret = false;
            }
        }
        else if (key == "energy-activation-ratio")
        {
            if (!tryParseYamlField(entry.second,
                                   capacityReservation.energyActivationRatio,
                                   area.name + " : invalid energy activation ratio for reserve "
                                     + reserveName))
            {
                ret = false;
            }
        }
        else if (key == "reference-activation-duration")
        {
            if (!tryParseYamlField(entry.second,
                                   capacityReservation.referenceActivationDuration,
                                   area.name
                                     + " : invalid reference activation duration for reserve "
                                     + reserveName))
            {
                ret = false;
            }
        }
        else if (key == "type")
        {
            std::string value;
            if (!tryParseYamlField(entry.second,
                                   value,
                                   area.name + " : invalid type for reserve " + reserveName))
            {
                ret = false;
            }
            else if (value == "up")
            {
                capacityReservation.type = ReserveType::UP;
            }
            else if (value == "down")
            {
                capacityReservation.type = ReserveType::DOWN;
            }
            else
            {
                logs.warning() << area.name << " : invalid type for reserve " << reserveName;
                ret = false;
            }
        }
        else
        {
            logs.warning() << area.name << " : invalid key " << key
                           << " inside reserve parameters for " << reserveName;
            ret = false;
        }
    }
    fs::path filePath = folderInput / "reserves" / area.id / (capacityReservation.id() + ".txt");
    capacityReservation.loadNeedFromFile(filePath);
    area.allCapacityReservations.value().areaCapacityReservations.emplace(capacityReservation.id(),
                                                                          capacityReservation);
    return ret;
}

static bool AreaListLoadThermalDataFromFile(AreaList& list, const fs::path& filename)
{
    // Reset to 0
    list.each(
      [](Area& area)
      {
          area.thermal.unsuppliedEnergyCost = 0.;
          area.thermal.spilledEnergyCost = 0.;
      });

    IniFile ini;
    // Try to load the file
    if (!ini.open(filename))
    {
        return false;
    }

    auto* section = ini.find("unserverdenergycost");

    // Try to find the section
    if (section && section->firstProperty)
    {
        AreaName id;
        for (IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            id = transformNameIntoID(std::string(p->key));
            Area* area = list.find(id);
            if (area)
            {
                // MBO 15/04/2014
                // limit unsuppliedEnergyCost to 5.e4
                area->thermal.unsuppliedEnergyCost = Antares::stringToDouble(std::string(p->value));
                // if (area->thermal.unsuppliedEnergyCost < 0.)
                // logs.error() << "Thermal: `" << area->name << "`: The unsupplied energy cost must
                // be positive";

                // New scheme
                // MBO 30/10/2014 - Allow negative values
                if (std::abs(area->thermal.unsuppliedEnergyCost) < 5.e-3)
                {
                    area->thermal.unsuppliedEnergyCost = 0.;
                }
                else
                {
                    if (area->thermal.unsuppliedEnergyCost > 5.e4)
                    {
                        area->thermal.unsuppliedEnergyCost = 5.e4;
                    }
                    else
                    {
                        if (area->thermal.unsuppliedEnergyCost < -5.e4)
                        {
                            area->thermal.unsuppliedEnergyCost = -5.e4;
                        }
                    }
                }
            }
            else
            {
                logs.warning() << filename << ": `" << p->key << "`: Unknown area";
            }
        }
    }

    section = ini.find("spilledenergycost");

    // Try to find the section
    if (section && section->firstProperty)
    {
        AreaName id;
        for (IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            id = transformNameIntoID(std::string(p->key));
            auto* area = list.find(id);
            if (area)
            {
                // MBO 15/04/2014
                // limit spilledEnergyCost to 5.e4
                area->thermal.spilledEnergyCost = Antares::stringToDouble(std::string(p->value));
                // if (area->thermal.spilledEnergyCost < 0.)
                // logs.error() << "Thermal: `" << area->name << "`: The spilled energy cost must be
                // positive";

                // MBO 30/10/2014 - allow negative values
                if (std::abs(area->thermal.spilledEnergyCost) < 5.e-3)
                {
                    area->thermal.spilledEnergyCost = 0.;
                }
                else
                {
                    if (area->thermal.spilledEnergyCost > 5.e4)
                    {
                        area->thermal.spilledEnergyCost = 5.e4;
                    }
                    else
                    {
                        if (area->thermal.spilledEnergyCost < -5.e4)
                        {
                            area->thermal.spilledEnergyCost = -5.e4;
                        }
                    }
                }
            }
            else
            {
                logs.warning() << filename << ": `" << p->key << "`: Unknown area";
            }
        }
    }

    // This is not a warning !
    // We may have an empty INI file with 0 everywhere
    // logs.warning() << '`' << filename << "`: The section `unsuppliedenergycost` can not be
    // found";
    return true;
}

} // anonymous namespace

AreaList::AreaList(Study& study):
    pStudy(study)
{
}

bool AreaList::empty() const
{
    return areas.empty();
}

const AreaLink* AreaList::findLink(const AreaName& area, const AreaName& with) const
{
    auto i = areas.find(area);
    if (i != areas.end())
    {
        auto j = areas.find(with);
        if (j != areas.end())
        {
            return (*(i->second)).findExistingLinkWith(*(j->second));
        }
    }
    return nullptr;
}

void AreaList::rebuildIndexes()
{
    byIndex.clear();

    byIndex.resize(areas.size());

    unsigned int indx = 0;
    auto end = areas.end();
    for (auto i = areas.begin(); i != end; ++i, ++indx)
    {
        Area* area = i->second.get();
        byIndex[indx] = area;
        area->index = indx;
    }
}

Area* AreaList::add(Area* a)
{
    if (a)
    {
        // Indexing the area
        a->index = (unsigned int)areas.size();

        [[maybe_unused]] unsigned count = (unsigned int)areas.size(); // used for assert

        // Adding the area in the list
        areas[a->id] = std::unique_ptr<Area>(a);
        rebuildIndexes();

        assert(areas.size() == (count + 1) and "Invalid count of areas in the map");
    }
    return a;
}

Area* addAreaToListOfAreas(AreaList& list, const std::string& name)
{
    // Initializing names
    AreaName lname = transformNameIntoID(name);

    // Add the area
    return AreaListAddFromNames(list, name, lname);
}

Area* AreaListAddFromNames(AreaList& list, const std::string& name, const std::string& lname)
{
    if (name.empty())
    {
        logs.warning() << "ignoring invalid area name: `" << name;
        return nullptr;
    }

    if (CheckForbiddenCharacterInAreaName(name))
    {
        logs.error() << "Invalid area name: `" << name << "` contains forbidden character `*`";
        return nullptr;
    }
    // Look up
    if (!AreaListLFind(&list, lname.c_str()))
    {
        Area* area = new Area(name, lname);
        // Adding it
        Area* ret = list.add(area);
        if (!ret)
        {
            // The area has not been added, the pointer is not referenced anywhere
            delete area;
            return nullptr;
        }
        return ret;
    }
    else
    {
        logs.warning() << "The area `" << name << "` can not be added (dupplicate)";
    }
    return nullptr;
}

bool AreaList::loadListFromFile(const fs::path& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        logs.error() << "I/O error: " << filename << ": Impossible to open the file";
        return false;
    }

    // Log entry
    logs.info() << "  Loading the area list from `" << filename << '`';

    // Initialization of the strings
    AreaName name;
    // Each lines in the file
    std::string buffer;
    while (std::getline(file, buffer))
    {
        // The area name
        name = buffer;
        boost::trim(name);
        if (name.empty())
        {
            continue;
        }

        // Add the area in the list
        addAreaToListOfAreas(*this, name);
    }

    switch (areas.size())
    {
    case 0:
        logs.info() << "  No area found";
        break;
    case 1:
        logs.info() << "  1 area found";
        break;
    default:
        logs.info() << "  " << areas.size() << " areas found";
    }
    return true;
}

void AreaList::saveLinkListToBuffer(std::ostream& buffer) const
{
    each(
      [&buffer](const Area& area)
      {
          buffer << area.id << '\n';
          auto end = area.links.end();
          for (auto i = area.links.begin(); i != end; ++i)
          {
              buffer << '\t' << (i->second)->with->id << '\n';
          }
      });
}

static void readAdqPatchMode(Study& study, Area& area)
{
    if (study.header.version < StudyVersion(8, 3))
    {
        return;
    }

    fs::path adqPath = study.folderInput / "areas" / area.id / "adequacy_patch.ini";
    IniFile ini;
    if (ini.open(adqPath))
    {
        auto* section = ini.find("adequacy-patch");
        for (auto* p = section->firstProperty; p; p = p->next)
        {
            const std::string key = Antares::stringToLower(std::string(p->key));
            if (key == "adequacy-patch-mode")
            {
                const std::string value = Antares::stringToLower(std::string(p->value));

                if (value == "virtual")
                {
                    area.adequacyPatchMode = AdequacyPatch::virtualArea;
                }
                else if (value == "inside")
                {
                    area.adequacyPatchMode = AdequacyPatch::physicalAreaInsideAdqPatch;
                }
                else
                {
                    area.adequacyPatchMode = AdequacyPatch::physicalAreaOutsideAdqPatch;
                }
            }
        }
    }
}

static bool checkMatrixPositive(const Matrix<>& m, const std::string& buffer, unsigned limit)
{
    logs.debug() << "Checking : " << buffer;
    if (m.width and m.height and limit)
    {
        for (unsigned x = 0; x < limit; ++x)
        {
            auto& column = m.entry[x];
            for (unsigned y = 0; y < m.height; ++y)
            {
                if (column[y] < 0.)
                {
                    logs.error() << buffer << ": negative value detected (at column " << x
                                 << ", row: " << y << ')';
                    return false;
                }
            }
        }
    }
    return true;
}

template<class StringT>
static bool AreaListLoadFromFolderSingleArea(Study& study,
                                             AreaList* list,
                                             Area& area,
                                             StringT& buffer)
{
    // Reset
    area.filterSynthesis = filterAll;
    area.filterYearByYear = filterAll;

    area.nodalOptimization = 0;
    area.spreadUnsuppliedEnergyCost = 0.;
    area.spreadSpilledEnergyCost = 0.;

    bool ret = true;
    const auto studyVersion = study.header.version;

    // DSM, Reserves, D-1
    fs::path reservesPath = (study.folderInput / "reserves" / area.id).replace_extension("txt");
    ret = area.reserves.loadFromCSVFile(reservesPath.string(),
                                        fhrMax,
                                        HOURS_PER_YEAR,
                                        Matrix<>::optFixedSize)
          && ret;

    // Optimzation preferences
    if (!study.parameters.include.reserve.dayAhead)
    {
        area.reserves.columnToZero(fhrDayBefore);
    }
    if (!study.parameters.include.reserve.strategic)
    {
        area.reserves.columnToZero(fhrStrategicReserve);
    }
    if (!study.parameters.include.reserve.primary)
    {
        area.reserves.columnToZero(fhrPrimaryReserve);
    }

    // Fatal hors hydro - Misc Gen.
    std::string miscgenName = "miscgen-" + area.id + ".txt";
    fs::path miscgenPath = study.folderInput / "misc-gen" / miscgenName;

    ret = area.miscGen.loadFromCSVFile(miscgenPath.string(),
                                       fhhMax,
                                       HOURS_PER_YEAR,
                                       Matrix<>::optFixedSize)
          && ret;

    // Check misc gen
    buffer = "Misc Gen: `" + area.id + '`';
    ret = checkMatrixPositive(area.miscGen, buffer, fhhPSP) && ret;

    // Links
    {
        fs::path folder = study.folderInput / "links" / area.id.c_str();
        ret = AreaLinksLoadFromFolder(study, list, &area, folder) && ret;
    }

    bool averageTs = study.parameters.derated;
    // Load
    {
        if (area.load.prepro) // Prepro
        {
            fs::path loadPath = study.folderInput / "load" / "prepro" / area.id;
            ret = area.load.prepro->loadFromFolder(loadPath) && ret;
        }
        if (!area.load.prepro) // Series
        {
            std::string loadId = "load_" + area.id + ".txt";
            fs::path loadSeriesPath = study.folderInput / "load" / "series" / loadId;

            ret = area.load.series.loadFromFile(loadSeriesPath, averageTs) && ret;
        }
    }

    // Reserves
    if (study.parameters.include.reserves && studyVersion >= StudyVersion(10, 1))
    {
        ret = Antares::Data::loadReservesParameters(study.folderInput, area) && ret;
    }

    // Solar
    {
        if (area.solar.prepro) // Prepro
        {
            fs::path solarPath = study.folderInput / "solar" / "prepro" / area.id;
            ret = area.solar.prepro->loadFromFolder(solarPath) && ret;
        }
        if (!area.solar.prepro) // Series
        {
            std::string solarId = "solar_" + area.id + ".txt";
            fs::path solarPath = study.folderInput / "solar" / "series" / solarId;
            ret = area.solar.series.loadFromFile(solarPath, averageTs) && ret;
        }
    }

    // Hydro
    {
        // Allocation
        std::string areaIdIni = area.id + ".ini";
        fs::path hydroAlloc = study.folderInput / "hydro" / "allocation" / areaIdIni;
        ret = area.hydro.allocation.loadFromFile(area.id, hydroAlloc) && ret;

        fs::path pathHydro = study.folderInput / "hydro";
        fs::path hydroSeries = pathHydro / "series";

        if (area.hydro.prepro) /* Hydro */
        {
            fs::path hydroPrepro = pathHydro / "prepro";
            ret = area.hydro.prepro->loadFromFolder(study, area.id, hydroPrepro) && ret;
            ret = area.hydro.prepro->validate(area.id) && ret;
        }

        if (!area.hydro.prepro) // Series
        {
            ret = area.hydro.series->loadGenerationTS(area.id, hydroSeries, studyVersion) && ret;
        }

        switch (study.parameters.compatibility.hydroPmax)
        {
        case Parameters::Compatibility::HydroPmax::Daily:
        {
            HydroMaxTimeSeriesReader reader(area.hydro, area.id, area.name);
            ret = reader.read(pathHydro.string()) && ret;
            break;
        }
        case Parameters::Compatibility::HydroPmax::Hourly:
        {
            ret = area.hydro.series->LoadMaxPower(area.id, hydroSeries) && ret;
            break;
        }
        default:
            throw std::invalid_argument(
              "Value not supported for study.parameters.compatibility.hydroPmax");
        }

        RuleCurvesLoaderService ruleCurvesLoaderService(area.hydro.series->ruleCurves);

        ret = ruleCurvesLoaderService.LoadFromFolder(area.id,
                                                     pathHydro,
                                                     study.parameters.compatibility.hydroRuleCurves)
              && ret;

        area.hydro.series->resizeTSinDeratedMode(study.parameters.derated,
                                                 studyVersion,
                                                 study.parameters.compatibility.hydroPmax);

        if (study.parameters.unitCommitment.ucMode != UnitCommitmentMode::ucHeuristicFast
            && study.parameters.include.reserves && studyVersion >= StudyVersion(10, 1))
        {
            // the path change after 10.2 to avoid a collision with common/capacity
            fs::path reservesHydroPath = studyVersion == StudyVersion(10, 1)
                                           ? study.folderInput / "hydro" / "common" / area.id
                                               / "reserve-participations.yml"
                                           : study.folderInput / "hydro" / "reserves" / area.id
                                               / "reserve-participations.yml";

            area.hydro.loadReserveParticipations(area, reservesHydroPath);
        }
    }

    // Wind
    {
        if (area.wind.prepro) // Prepro
        {
            fs::path windPath = study.folderInput / "wind" / "prepro" / area.id;
            ret = area.wind.prepro->loadFromFolder(windPath) && ret;
        }
        if (!area.wind.prepro) // Series
        {
            std::string windId = "wind_" + area.id + ".txt";
            fs::path windPath = study.folderInput / "wind" / "series" / windId;
            ret = area.wind.series.loadFromFile(windPath, averageTs) && ret;
        }
    }

    // Thermal cluster list
    {
        fs::path preproPath = study.folderInput / "thermal" / "prepro";
        ret = area.thermal.list.loadPreproFromFolder(study, preproPath) && ret;
        ret = area.thermal.list.validatePrepro(study) && ret;
        fs::path seriesPath = study.folderInput / "thermal" / "series";
        ret = area.thermal.list.loadDataSeriesFromFolder(study, seriesPath) && ret;
        ret = area.thermal.list.loadEconomicCosts(study, seriesPath) && ret;

        // In adequacy mode, all thermal clusters must be in 'mustrun' mode
        if (study.parameters.mode == SimulationMode::Adequacy)
        {
            area.thermal.list.enableMustrunForEveryone();
        }
        if (study.parameters.unitCommitment.ucMode != UnitCommitmentMode::ucHeuristicFast
            && study.parameters.include.reserves && studyVersion >= StudyVersion(10, 1))
        {
            fs::path reservesThermal = study.folderInput / "thermal" / "clusters" / area.id
                                       / "reserve-participations.yml";
            area.thermal.list.loadReserveParticipations(area, reservesThermal);
        }
    }

    // Short term storage
    if (studyVersion >= StudyVersion(8, 6))
    {
        fs::path seriesPath = study.folderInput / "st-storage" / "series" / area.id;

        ret = area.shortTermStorage.loadSeriesFromFolder(seriesPath, studyVersion) && ret;
        ret = area.shortTermStorage.validate(studyVersion) && ret;

        if (study.parameters.unitCommitment.ucMode != UnitCommitmentMode::ucHeuristicFast
            && study.parameters.include.reserves && studyVersion >= StudyVersion(10, 1))
        {
            fs::path reservesFilePath = study.folderInput / "st-storage" / "clusters" / area.id
                                        / "reserve-participations.yml";
            area.shortTermStorage.loadReserveParticipations(area, reservesFilePath);
        }
    }

    // Renewable cluster list
    if (studyVersion >= StudyVersion(8, 1))
    {
        fs::path seriesPath = study.folderInput / "renewables" / "series";
        ret = area.renewable.list.loadDataSeriesFromFolder(study, seriesPath) && ret;
    }

    // Adequacy patch
    readAdqPatchMode(study, area);

    // Nodal Optimization
    fs::path nodalPath = study.folderInput / "areas" / area.id / "optimization.ini";

    IniFile ini;
    if (!ini.open(nodalPath))
    {
        return false;
    }

    ini.each(
      [&area, &nodalPath](const IniFile::Section& section)
      {
          for (auto* p = section.firstProperty; p; p = p->next)
          {
              const bool value = Antares::stringToBool(std::string(p->value));
              const std::string key = Antares::stringToLower(std::string(p->key));
              if (key == "non-dispatchable-power")
              {
                  if (value)
                  {
                      area.nodalOptimization |= anoNonDispatchPower;
                  }
                  continue;
              }
              if (key == "dispatchable-hydro-power")
              {
                  if (value)
                  {
                      area.nodalOptimization |= anoDispatchHydroPower;
                  }
                  continue;
              }
              if (key == "other-dispatchable-power")
              {
                  if (value)
                  {
                      area.nodalOptimization |= anoOtherDispatchPower;
                  }
                  continue;
              }
              if (key == "filter-synthesis")
              {
                  area.filterSynthesis = stringIntoDatePrecision(std::string(p->value));
                  continue;
              }
              if (key == "filter-year-by-year")
              {
                  area.filterYearByYear = stringIntoDatePrecision(std::string(p->value));
                  continue;
              }
              if (key == "spread-unsupplied-energy-cost")
              {
                  if (!Antares::stringToDouble(std::string(p->value),
                                               area.spreadUnsuppliedEnergyCost))
                  {
                      area.spreadUnsuppliedEnergyCost = 0.;
                      logs.warning() << area.name << ": invalid spread for unsupplied energy cost";
                  }
                  continue;
              }
              if (key == "spread-spilled-energy-cost")
              {
                  if (!Antares::stringToDouble(std::string(p->value), area.spreadSpilledEnergyCost))
                  {
                      area.spreadSpilledEnergyCost = 0.;
                      logs.warning() << area.name << ": invalid spread for spilled energy cost";
                  }
                  continue;
              }

              logs.warning() << nodalPath << ": Unknown property '" << p->key << "'";
          }
      });

    return ret;
}

void AreaList::ensureDataIsInitialized(Parameters& params)
{
    AreaListEnsureDataHydroTimeSeries(this);

    // Load
    if (params.isTSGeneratedByPrepro(timeSeriesLoad))
    {
        AreaListEnsureDataLoadPrepro(this);
    }
    // Solar
    if (params.isTSGeneratedByPrepro(timeSeriesSolar))
    {
        AreaListEnsureDataSolarPrepro(this);
    }
    // Hydro
    if (params.isTSGeneratedByPrepro(timeSeriesHydro))
    {
        AreaListEnsureDataHydroPrepro(this);
    }
    // Wind
    if (params.isTSGeneratedByPrepro(timeSeriesWind))
    {
        AreaListEnsureDataWindPrepro(this);
    }
    // Thermal
    AreaListEnsureDataThermalPrepro(this);
}

bool AreaList::loadFromFolder(const StudyLoadOptions& options)
{
    bool ret = true;
    std::string buffer;
    auto studyVersion = pStudy.header.version;

    // Load the list of all available areas
    {
        logs.info() << "Loading the list of areas...";
        fs::path areaListPath = pStudy.folderInput / "areas" / "list.txt";
        ret = loadListFromFile(areaListPath) && ret;
    }

    // Hydro
    {
        logs.info() << "Loading global hydro data...";
        fs::path hydroPath = pStudy.folderInput / "hydro";
        ret = PartHydro::LoadFromFolder(pStudy, hydroPath.string()) && ret;
        ret = PartHydro::validate(pStudy) && ret;
    }

    // Thermal data, specific to areas
    {
        logs.info() << "Loading thermal clusters...";
        fs::path thermalPath = pStudy.folderInput / "thermal";
        fs::path areaIniPath = thermalPath / "areas.ini";
        ret = AreaListLoadThermalDataFromFile(*this, areaIniPath) && ret;

        // The cluster list must be loaded before the method ensureDataIsInitialized is called
        // in order to allocate data with all thermal clusters.
        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            fs::path areaPath = thermalPath / "clusters" / area.id;
            ret = area.thermal.list.loadFromFolder(areaPath, &area) && ret;
            ret = area.thermal.list.validateClusters(pStudy.parameters) && ret;
        }
    }

    // Short term storage data, specific to areas
    if (studyVersion >= StudyVersion(8, 6))
    {
        logs.info() << "Loading short term storage clusters...";
        fs::path stsFolder = pStudy.folderInput / "st-storage";

        if (fs::exists(stsFolder))
        {
            for (const auto& area: areas | std::views::values)
            {
                fs::path cluster_folder = stsFolder / "clusters" / area->id.c_str();
                ret = area->shortTermStorage.createSTStorageClustersFromIniFile(cluster_folder)
                      && ret;
                // Additional constraints were added from version 9.2
                if (studyVersion >= StudyVersion(9, 2))
                {
                    const auto constraints_folder = stsFolder / "constraints" / area->id.c_str();
                    ret = area->shortTermStorage.loadAdditionalConstraints(constraints_folder)
                          && ret;
                }
            }
        }
        else
        {
            logs.info() << "Short term storage not found, skipping";
        }
    }

    // Renewable data, specific to areas
    if (studyVersion >= StudyVersion(8, 1))
    {
        // The cluster list must be loaded before the method ensureDataIsInitialized is called
        // in order to allocate data with all renewable clusters.
        fs::path renewClusterPath = pStudy.folderInput / "renewables" / "clusters";

        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            fs::path areaPath = renewClusterPath / area.id;
            ret = area.renewable.list.loadFromFolder(areaPath, &area) && ret;
            ret = area.renewable.list.validateClusters() && ret;
        }
    }

    // Prepare
    ensureDataIsInitialized(pStudy.parameters);

    // Load all nodes
    unsigned int indx = 0;
    each(
      [&options, &ret, &buffer, &indx, this](Area& area)
      {
          // Progression
          options.logMessage = "Loading the area " + std::to_string(++indx) + '/'
                               + std::to_string(areas.size()) + ": " + area.name;
          logs.info() << options.logMessage;

          // Load a single area
          ret = AreaListLoadFromFolderSingleArea(pStudy, this, area, buffer) && ret;
      });

    // update nameid set
    updateNameIDSet();
    return ret;
}

Area* AreaList::find(const AreaName& id)
{
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second.get() : nullptr;
}

const Area* AreaList::find(const AreaName& id) const
{
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second.get() : nullptr;
}

Area* AreaList::findFromName(const AreaName& name)
{
    AreaName id = transformNameIntoID(name);
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second.get() : nullptr;
}

const Area* AreaList::findFromName(const AreaName& name) const
{
    AreaName id = transformNameIntoID(name);
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second.get() : nullptr;
}

Area* AreaListLFind(AreaList* l, const char lname[])
{
    if (l && !l->areas.empty() && lname)
    {
        auto i = l->areas.find(AreaName(lname));
        return (i != l->areas.end()) ? i->second.get() : nullptr;
    }
    return nullptr;
}

Area* AreaListFindPtr(AreaList* l, const Area* ptr)
{
    if (l && ptr)
    {
        auto end = l->areas.end();
        for (auto i = l->areas.begin(); i != end; ++i)
        {
            if (ptr == i->second.get())
            {
                return i->second.get();
            }
        }
    }
    return nullptr;
}

void AreaListEnsureDataLoadPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Area& area)
      {
          if (!area.load.prepro)
          {
              area.load.prepro = std::make_unique<Load::Prepro>();
          }
      });
}

void AreaListEnsureDataSolarPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Area& area)
      {
          if (!area.solar.prepro)
          {
              area.solar.prepro = std::make_unique<Solar::Prepro>();
          }
      });
}

void AreaListEnsureDataWindPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Area& area)
      {
          if (!area.wind.prepro)
          {
              area.wind.prepro = std::make_unique<Wind::Prepro>();
          }
      });
}

void AreaListEnsureDataHydroTimeSeries(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Area& area)
      {
          if (!area.hydro.series)
          {
              area.hydro.series = std::make_unique<DataSeriesHydro>();
          }
      });
}

void AreaListEnsureDataHydroPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Area& area)
      {
          if (!area.hydro.prepro)
          {
              area.hydro.prepro = std::make_unique<PreproHydro>();
          }
      });
}

void AreaListEnsureDataThermalPrepro(AreaList* l)
{
    l->each([](Area& area) { area.thermal.list.ensureDataPrepro(); });
}

unsigned int AreaList::areaLinkCount() const
{
    unsigned int ret = 0;
    each([&ret](const Area& area) { ret += (unsigned int)area.links.size(); });
    return ret;
}

void AreaList::resizeAllTimeseriesNumbers(unsigned int n)
{
    // Ask to resize the matrices dedicated to the sampled timeseries numbers
    // for each area
    each([n](Area& area) { area.resizeAllTimeseriesNumbers(n); });
}

const AreaLink* AreaList::findLinkFromINIKey(const std::string& key) const
{
    if (key.empty())
    {
        return nullptr;
    }
    auto offset = key.find('%');
    if (offset == std::string::npos || (0 == offset) || (offset == key.size() - 1))
    {
        return nullptr;
    }
    AreaName from(key, 0, offset);
    AreaName to(key, offset + 1, key.size() - (offset + 1));

    return findLink(from, to);
}

ThermalCluster* AreaList::findClusterFromINIKey(const std::string& key)
{
    if (key.empty())
    {
        return nullptr;
    }
    auto offset = key.find('.');
    if (offset == std::string::npos || (0 == offset) || (offset == key.size() - 1))
    {
        return nullptr;
    }
    AreaName parentName(key, 0, offset);
    std::string id(key, offset + 1, key.size() - (offset + 1));
    Area* parentArea = findFromName(parentName);
    if (parentArea == nullptr)
    {
        return nullptr;
    }
    return parentArea->thermal.list.findInAll(id);
}

void AreaList::updateNameIDSet() const
{
    nameidSet.clear();
    auto end = areas.end();
    for (auto i = areas.begin(); i != end; ++i)
    {
        auto& area = *(i->second);
        nameidSet.insert(area.id);
    }
}

Area::ScratchMap AreaList::buildScratchMap(unsigned int numspace) const
{
    Area::ScratchMap scratchmap;
    each([&scratchmap, &numspace](Area& a) { scratchmap.try_emplace(&a, a.scratchpad[numspace]); });
    return scratchmap;
}

void validateCapacityReservations(const Area& area)
{
    if (area.allCapacityReservations)
    {
        errorIfNegativeValue("maxGlobalEnergyActivationRatio up",
                             area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.up,
                             area.name);
        errorIfNegativeValue(
          "maxGlobalEnergyActivationRatio down",
          area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.down,
          area.name);
        errorIfNegativeValue(
          "referenceGlobalActivationDuration up",
          area.allCapacityReservations.value().referenceGlobalActivationDuration.up,
          area.name);
        errorIfNegativeValue(
          "referenceGlobalActivationDuration down",
          area.allCapacityReservations.value().referenceGlobalActivationDuration.down,
          area.name);
        for (const auto& [resID, capacityRes]:
             area.allCapacityReservations.value().areaCapacityReservations)
        {
            errorIfNegativeValue("energyActivationRatio",
                                 capacityRes.energyActivationRatio,
                                 area.name,
                                 std::nullopt,
                                 resID);
            errorIfNegativeValue("powerActivationRatio",
                                 capacityRes.powerActivationRatio,
                                 area.name,
                                 std::nullopt,
                                 resID);
            errorIfNegativeValue("referenceActivationDuration",
                                 capacityRes.referenceActivationDuration,
                                 area.name,
                                 std::nullopt,
                                 resID);
        }
    }
}

bool loadReservesParameters(fs::path& folderInput, Area& area)
{
    bool ret = true;
    fs::path reservesFile = folderInput / "reserves" / area.id / "reserves.yml";
    area.allCapacityReservations.emplace();

    if (!fs::exists(reservesFile))
    {
        logs.info() << "No reserves file found for area " << area.name;
        return ret;
    }

    std::string content;
    try
    {
        content = IO::readFile(reservesFile);
    }
    catch (const std::exception& e)
    {
        logs.error() << "Failed to read reserves file: " << reservesFile << " - " << e.what();
        return false;
    }

    YAML::Node root;
    try
    {
        root = YAML::Load(content);
    }
    catch (const YAML::Exception& e)
    {
        logs.error() << "Invalid reserves config : " << reservesFile << " " << e.what();
        return false;
    }

    if (root["global-parameters"])
    {
        ret = readReservesAreaParameters(area, root["global-parameters"]) && ret;
    }

    if (root["reserves"])
    {
        for (const auto& reserveNode: root["reserves"])
        {
            ret = readReserveParameters(folderInput, area, reserveNode) && ret;
        }
    }

    validateCapacityReservations(area);
    return ret;
}

} // namespace Antares::Data
