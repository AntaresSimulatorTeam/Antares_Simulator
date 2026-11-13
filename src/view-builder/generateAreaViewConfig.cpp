// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <array>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <antares/solver/optimisation/opt_rename_problem.h>
#include <antares/study/area/links.h>
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
constexpr std::array areaMetrics = {
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

constexpr std::array linkMetrics = {
  Metric{"FLOW_LIN", {"FLOW LIN."}},
  Metric{"UCAP_LIN", {"UCAP LIN."}},
  Metric{"CONG_FEE_ABS", {"CONG. FEE (ABS.)"}},
  Metric{"CONG_FEE_ALG", {"CONG. FEE (ALG.)"}},
  Metric{"MARG_COST", {"MARG. COST"}},
  Metric{"CONG_PROB_DIR", {"CONG. PROB +"}},
  Metric{"CONG_PROB_INDIR", {"CONG. PROB -"}},
  Metric{"HURDLE_COST", {"HURDLE COST"}},
  Metric{"LOOP_FLOW", {"LOOP FLOW"}},
};

constexpr std::array thermalMetrics = {
  Metric{"GENERATION", {"DTG by plant"}},
  Metric{"MIN_GEN", {"MIN DTG by plant"}},
  Metric{"PROFIT", {"Profit by plant"}},
  Metric{"NODU", {"NODU by plant"}},
  Metric{"NP_COST", {"NP Cost by plant"}},
};

constexpr std::array storageMetrics = {
  Metric{"P_INJECTION", {"STS inj by plant"}},
  Metric{"P_WITHDRAWAL", {"STS withdrawal by plant"}},
  Metric{"LEVELS", {"STS lvl by plant"}},
  Metric{"CASHFLOW", {"STS Cashflow By Cluster"}},
};

constexpr std::array renewableMetrics = {
  Metric{"GENERATION", {"RES generation by plant"}},
};

struct ViewDefinition
{
    std::string_view id;
    std::string_view filename;
    std::string_view catalog;
    std::span<const Metric> metrics;
};

ViewDefinition definition(ViewType type)
{
    switch (type)
    {
    case ViewType::area:
        return {"view_area", "view_config_area.yml", "antares_legacy_area_catalog", areaMetrics};
    case ViewType::link:
        return {"view_link", "view_config_link.yml", "antares_legacy_link_catalog", linkMetrics};
    case ViewType::thermal:
        return {"view_thermal",
                "view_config_thermal.yml",
                "antares_legacy_thermal_cluster_catalog",
                thermalMetrics};
    case ViewType::storage:
        return {"view_storage",
                "view_config_sts.yml",
                "antares_legacy_short_term_storage_catalog",
                storageMetrics};
    case ViewType::renewable:
        return {"view_renewable",
                "view_config_renewable.yml",
                "antares_legacy_renewable_cluster_catalog",
                renewableMetrics};
    }
    throw std::invalid_argument("Unknown view type");
}

} // namespace

YAML::Node generateViewConfig(const Data::Study& study, ViewType type)
{
    const auto def = definition(type);
    const bool isLink = type == ViewType::link;
    YAML::Node view;
    view["id"] = std::string(def.id);
    view["taxonomy"] = "antares_legacy_taxonomy";
    view["scope"]["location"]["taxonomy-category"] = isLink ? "link" : "balance";

    std::vector<std::string> districts;
    if (!isLink)
    {
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
    }

    YAML::Node patterns(YAML::NodeType::Sequence);
    for (const auto& [granularity, flag]: granularities)
    {
        for (bool scenario: {false, true})
        {
            YAML::Node pattern;
            pattern["id"] = std::string(granularity) + (scenario ? "_all" : "_ind");
            pattern["time-granularity"] = std::string(granularity);
            pattern["scenario"] = scenario;

            if (study.parameters.geographicTrimming)
            {
                std::vector<std::string> selected;
                size_t locationCount = districts.size();
                study.areas.each(
                  [&](const Data::Area& area)
                  {
                      if (isLink)
                      {
                          for (const auto& link: area.links | std::views::values)
                          {
                              ++locationCount;
                              const auto filter = scenario ? link->filterSynthesis
                                                           : link->filterYearByYear;
                              if (filter & flag)
                              {
                                  selected.push_back(
                                    BuildLinkComponentId(link->from->id, link->with->id));
                              }
                          }
                      }
                      else
                      {
                          ++locationCount;
                          const auto filter = scenario ? area.filterSynthesis
                                                       : area.filterYearByYear;
                          if (filter & flag)
                          {
                              selected.push_back(areaLocation(area.id));
                          }
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

                if (selected.empty())
                {
                    continue;
                }
                if (selected.size() != locationCount)
                {
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
    for (const auto& metric: def.metrics)
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
        entry["id"] = std::string(def.catalog) + "." + std::string(metric.id);
        selectedMetrics.push_back(entry);
    }
    view["metrics"] = selectedMetrics;
    YAML::Node root;
    root["view"] = view;
    return root;
}

YAML::Node generateAreaViewConfig(const Data::Study& study)
{
    return generateViewConfig(study, ViewType::area);
}

bool exportViewConfig(const Data::Study& study, Solver::IResultWriter* resultWriter, ViewType type)
{
    const auto config = generateViewConfig(study, type);
    if (config["view"]["metrics"].size() == 0
        || config["view"]["aggregations-patterns"].size() == 0)
    {
        return false;
    }
    std::string content = YAML::Dump(config);
    resultWriter->addEntryFromBuffer("views/input/" + std::string(definition(type).filename),
                                     content);
    return true;
}

} // namespace Antares::ViewBuilder
