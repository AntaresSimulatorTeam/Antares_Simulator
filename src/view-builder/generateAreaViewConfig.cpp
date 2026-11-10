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

struct Metric
{
    std::string_view id;
    std::array<std::string_view, 4> variables;
};

// Legacy variable-to-metric correspondence from the GEMS converter's view documentation.
constexpr std::array metrics = {
  Metric{"OV_COST", {"OV. COST"}},
  Metric{"OP_COST", {"OP. COST"}},
  Metric{"MRG_PRICE", {"MRG. PRICE"}},
  Metric{"CO2_EMIS", {"CO2 EMIS."}},
  Metric{"NH3_EMIS", {"NH3 EMIS."}},
  Metric{"SO2_EMIS", {"SO2 EMIS."}},
  Metric{"NOX_EMIS", {"NOX EMIS."}},
  Metric{"PM2_5_EMIS", {"PM2_5 EMIS."}},
  Metric{"PM5_EMIS", {"PM5 EMIS."}},
  Metric{"PM10_EMIS", {"PM10 EMIS."}},
  Metric{"NMVOC_EMIS", {"NMVOC EMIS."}},
  Metric{"OP1_EMIS", {"OP1 EMIS."}},
  Metric{"OP2_EMIS", {"OP2 EMIS."}},
  Metric{"OP3_EMIS", {"OP3 EMIS."}},
  Metric{"OP4_EMIS", {"OP4 EMIS."}},
  Metric{"OP5_EMIS", {"OP5 EMIS."}},
  Metric{"LOAD", {"LOAD"}},
  Metric{"RES_LOAD", {"RES LOAD"}},
  Metric{"DTG_BY_TECHNOLOGY", {"Dispatch. Gen."}},
  Metric{"BALANCE", {"BALANCE"}},
  Metric{"RES_GENERATION", {"Renewable Gen.", "WIND", "SOLAR", "H. ROR"}},
  Metric{"STS_INJECTION", {"STS by group"}},
  Metric{"STS_WITHDRAWAL", {"STS by group"}},
  Metric{"STS_LEVEL", {"STS by group"}},
  Metric{"UNSP_ENRG", {"UNSP. ENRG"}},
  Metric{"SPIL_ENRG", {"SPIL. ENRG"}},
  Metric{"NODU", {"NODU"}},
  Metric{"NP_COST", {"NP COST"}},
  Metric{"AVL_DTG", {"AVL DTG"}},
  Metric{"DTG_MRG", {"DTG MRG"}},
  Metric{"LOLD", {"LOLD"}},
  Metric{"LOLP", {"LOLP"}},
  Metric{"NPCAP_HOURS", {"NPCAP HOURS"}},
  Metric{"ROW_BAL", {"ROW BAL."}},
  Metric{"MISC_NDG", {"MISC. NDG"}},
  Metric{"PSP", {"PSP"}},
  Metric{"H_STOR", {"H. STOR"}},
  Metric{"H_PUMP", {"H. PUMP"}},
  Metric{"H_OVFL", {"H. OVFL"}},
  Metric{"H_INFL", {"H. INFL"}},
  Metric{"H_LEV", {"H. LEV"}},
  Metric{"H_VAL", {"H. VAL"}},
  Metric{"H_COST", {"H. COST"}},
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

bool exportAreaViewConfig(const Data::Study& study, Solver::IResultWriter* resultWriter)
{
    const auto config = generateAreaViewConfig(study);
    if (config["view"]["metrics"].size() == 0)
    {
        return false;
    }
    std::string content = YAML::Dump(config);
    resultWriter->addEntryFromBuffer("views/input/view_config_area.yml", content);
    return true;
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

    YAML::Node selectedMetrics(YAML::NodeType::Sequence);
    for (const auto& metric: metrics)
    {
        if (study.parameters.thematicTrimming)
        {
            bool selected = false;
            for (const auto variable: metric.variables)
            {
                if (!variable.empty()
                    && study.parameters.variablesPrintInfo.exists(std::string(variable))
                    && study.parameters.variablesPrintInfo.isPrinted(std::string(variable)))
                {
                    selected = true;
                    break;
                }
            }
            if (!selected)
            {
                continue;
            }
        }
        YAML::Node entry;
        entry["id"] = std::string("antares_legacy_area_catalog.") + std::string(metric.id);
        selectedMetrics.push_back(entry);
    }
    view["metrics"] = selectedMetrics;
    YAML::Node root;
    root["view"] = view;
    return root;
}

} // namespace Antares::ViewBuilder
