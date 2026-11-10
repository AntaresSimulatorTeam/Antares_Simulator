// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <array>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/filter.h>
#include <antares/view-builder/legacyToYaml.h>
#include <antares/view-builder/viewBuilder.h>
#include <antares/writer/i_writer.h>

namespace Antares::ViewBuilder
{
namespace
{

struct Granularity
{
    std::string_view name;
    Data::FilterFlag flag;
};

constexpr std::array granularities = {
  Granularity{"hour", Data::filterHourly},
  Granularity{"day", Data::filterDaily},
  Granularity{"week", Data::filterWeekly},
  Granularity{"month", Data::filterMonthly},
  Granularity{"year", Data::filterAnnual},
};

constexpr std::array metrics = {
  "OV_COST",
  "OP_COST",
  "MRG_PRICE",
  "CO2_EMIS",
  "NH3_EMIS",
  "SO2_EMIS",
  "NOX_EMIS",
  "PM2_5_EMIS",
  "PM5_EMIS",
  "PM10_EMIS",
  "NMVOC_EMIS",
  "OP1_EMIS",
  "OP2_EMIS",
  "OP3_EMIS",
  "OP4_EMIS",
  "OP5_EMIS",
  "LOAD",
  "RES_LOAD",
  "DTG_BY_TECHNOLOGY",
  "BALANCE",
  "RES_GENERATION",
  "STS_INJECTION",
  "STS_WITHDRAWAL",
  "STS_LEVEL",
  "UNSP_ENRG",
  "SPIL_ENRG",
  "NODU",
  "NP_COST",
  "AVL_DTG",
  "DTG_MRG",
  "LOLD",
  "LOLP",
  "NPCAP_HOURS",
  "ROW_BAL",
  "MISC_NDG",
  "PSP",
  "H_STOR",
  "H_PUMP",
  "H_OVFL",
  "H_INFL",
  "H_LEV",
  "H_VAL",
  "H_COST",
};

// An empty location-filter means all locations in GEMS, so use a name that cannot match.
std::string unusedLocation(const Data::Study& study)
{
    std::set<std::string> names;
    study.areas.each([&names](const Data::Area& area) { names.insert(areaLocation(area.id)); });
    for (const auto& [name, areas]: study.setsOfAreas)
    {
        names.insert(name);
    }
    if (const auto* modelerData = study.getModelerData())
    {
        for (const auto& component: modelerData->system->Components())
        {
            names.insert(component.Id());
        }
        for (const auto& [component, properties]: modelerData->componentProperties)
        {
            for (const auto& [id, value]: properties)
            {
                names.insert(value);
            }
        }
    }
    std::string name = "__antares_no_location__";
    while (names.contains(name))
    {
        name += "_";
    }
    return name;
}

} // namespace

void exportAreaViewConfig(const Data::Study& study, Solver::IResultWriter* resultWriter)
{
    std::string content = YAML::Dump(generateAreaViewConfig(study));
    resultWriter->addEntryFromBuffer("views/input/view_config_area.yml", content);
}

YAML::Node generateAreaViewConfig(const Data::Study& study)
{
    YAML::Node view;
    view["id"] = "view_area";
    view["taxonomy"] = "antares_legacy_taxonomy";
    view["scope"]["location"]["taxonomy-category"] = "balance";

    std::vector<std::string> districts;
    for (const auto& [name, areas]: study.setsOfAreas)
    {
        if (study.setsOfAreas.hasOutput(name) && !areas->empty())
        {
            districts.push_back(name);
            YAML::Node extraLocation;
            extraLocation["id"] = "district_" + name;
            view["scope"]["extra-locations"].push_back(extraLocation);
        }
    }

    const auto noLocation = unusedLocation(study);
    YAML::Node patterns(YAML::NodeType::Sequence);
    for (const auto& [granularity, flag]: granularities)
    {
        for (bool scenario: {false, true})
        {
            YAML::Node pattern;
            pattern["id"] = std::string(granularity) + (scenario ? "-all" : "-ind");
            pattern["time-granularity"] = std::string(granularity);
            pattern["scenario"] = scenario;

            if (study.parameters.geographicTrimming)
            {
                std::vector<std::string> selected;
                study.areas.each(
                  [&](const Data::Area& area)
                  {
                      const auto filter = scenario ? area.filterSynthesis : area.filterYearByYear;
                      if (filter & flag)
                      {
                          selected.push_back(areaLocation(area.id));
                      }
                  });
                const auto report = scenario ? Data::Sets::ReportType::synthesis
                                             : Data::Sets::ReportType::yearByYear;
                for (const auto& district: districts)
                {
                    if (study.setsOfAreas.outputFilter(district, report) & flag)
                    {
                        selected.push_back(district);
                    }
                }

                if (selected.size() != study.areas.size() + districts.size() || selected.empty())
                {
                    if (selected.empty())
                    {
                        selected.push_back(noLocation);
                    }
                    for (const auto& location: selected)
                    {
                        pattern["location-filter"].push_back(location);
                    }
                }
            }
            patterns.push_back(pattern);
        }
    }
    view["aggregations-patterns"] = patterns;

    for (const auto* metric: metrics)
    {
        YAML::Node entry;
        entry["id"] = std::string("antares_legacy_area_catalog.") + metric;
        view["metrics"].push_back(entry);
    }
    YAML::Node root;
    root["view"] = view;
    return root;
}

} // namespace Antares::ViewBuilder
