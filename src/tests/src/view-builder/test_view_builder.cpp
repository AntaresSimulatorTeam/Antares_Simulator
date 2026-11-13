// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE view_builder_tests
#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <unordered_set>
#include <yaml-cpp/yaml.h>

#include <boost/test/unit_test.hpp>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/io/inputs/InputError.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/variable/registerThematicTrimmingVariables.h>
#include <antares/study/area/area.h>
#include <antares/study/area/constants.h>
#include <antares/study/area/links.h>
#include <antares/study/parts/renewable/cluster.h>
#include <antares/study/parts/short-term-storage/cluster.h>
#include <antares/study/parts/thermal/cluster.h>
#include <antares/study/study.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/system.h>
#include <antares/view-builder/legacyToYaml.h>
#include <antares/view-builder/viewBuilder.h>
#include <antares/writer/in_memory_writer.h>

using namespace Antares::Data;
using namespace Antares::ViewBuilder;
using namespace Antares::ModelerStudy::SystemModel;

namespace
{

void checkProperties(const YAML::Node& property,
                     const std::string& expectedId,
                     const std::string& expectedValue)
{
    BOOST_CHECK_EQUAL(property["id"].as<std::string>(), expectedId);
    BOOST_CHECK_EQUAL(property["value"].as<std::string>(), expectedValue);
}

YAML::Node findPattern(const YAML::Node& view, const std::string& id)
{
    for (const auto& pattern: view["aggregations-patterns"])
    {
        if (pattern["id"].as<std::string>() == id)
        {
            return pattern;
        }
    }
    return YAML::Node(YAML::NodeType::Undefined);
}

std::set<std::string> locations(const YAML::Node& pattern)
{
    std::set<std::string> result;
    for (const auto& location: pattern["location-filter"])
    {
        result.insert(location.as<std::string>());
    }
    return result;
}

std::set<std::string> metricIds(const YAML::Node& view)
{
    std::set<std::string> result;
    for (const auto& metric: view["metrics"])
    {
        result.insert(metric["id"].as<std::string>());
    }
    return result;
}

} // anonymous namespace

struct ViewBuilderFixture
{
    std::unique_ptr<Study> study;
    Area* fr;
    Area* de;
    std::shared_ptr<ThermalCluster> thermalCluster;
    std::shared_ptr<RenewableCluster> renewableCluster;

    ViewBuilderFixture()
    {
        Antares::Solver::Variable::RegisterThematicTrimmingVariables();
        study = std::make_unique<Study>();
        study->parameters.reset();
        study->parameters.renewableGeneration.toClusters();

        fr = addAreaToListOfAreas(study->areas, "france");
        de = addAreaToListOfAreas(study->areas, "germany");

        for (auto* area: {fr, de})
        {
            area->createMissingData();
            area->load.series.fill(1.0);
            area->wind.series.fill(1.0);
            area->solar.series.fill(1.0);
            area->miscGen.zero();
            for (unsigned column = 0; column < MiscGenIndex::fhhMax; ++column)
            {
                auto* values = area->miscGen[column];
                std::fill(values, values + area->miscGen.height, 1.0);
            }
            area->hydro.reservoirManagement = true;
            area->hydro.series->ror.fill(1.0);
        }
        study->areas.rebuildIndexes();

        thermalCluster = std::make_shared<ThermalCluster>(fr);
        thermalCluster->setName("nuc_fr");
        fr->thermal.list.addToCompleteList(thermalCluster);

        renewableCluster = std::make_shared<RenewableCluster>(fr);
        renewableCluster->setName("wind_fr");
        fr->renewable.list.addToCompleteList(renewableCluster);

        {
            ShortTermStorage::STStorageCluster sts;
            sts.properties.name = "battery_fr";
            sts.id = "battery_fr";
            fr->shortTermStorage.storagesByIndex.push_back(sts);
        }

        AreaAddLinkBetweenAreas(fr, de);
        fr->links.begin()->second->transmissionCapacities = LocalTransmissionCapacities::enabled;
        fr->links.begin()->second->directCapacities.fill(1.0);
        fr->links.begin()->second->indirectCapacities.fill(1.0);
    }
};

BOOST_FIXTURE_TEST_SUITE(view_builder_tests, ViewBuilderFixture)

BOOST_AUTO_TEST_CASE(other_views_thematic_variable_mappings_and_exports)
{
    struct ViewCase
    {
        ViewType type;
        std::string id;
        std::string filename;
        std::string catalog;
        std::vector<std::pair<std::string, std::string>> mappings;
    };

    const std::vector<ViewCase> cases = {
      {ViewType::link,
       "view_link",
       "view_config_link.yml",
       "antares_legacy_link_catalog",
       {{"FLOW LIN.", "FLOW_LIN"},
        {"UCAP LIN.", "UCAP_LIN"},
        {"CONG. FEE (ABS.)", "CONG_FEE_ABS"},
        {"CONG. FEE (ALG.)", "CONG_FEE_ALG"},
        {"MARG. COST", "MARG_COST"},
        {"CONG. PROB +", "CONG_PROB_DIR"},
        {"CONG. PROB -", "CONG_PROB_INDIR"},
        {"HURDLE COST", "HURDLE_COST"},
        {"LOOP FLOW", "LOOP_FLOW"}}},
      {ViewType::thermal,
       "view_thermal",
       "view_config_thermal.yml",
       "antares_legacy_thermal_cluster_catalog",
       {{"DTG by plant", "GENERATION"},
        {"MIN DTG by plant", "MIN_GEN"},
        {"Profit by plant", "PROFIT"},
        {"NODU by plant", "NODU"},
        {"NP Cost by plant", "NP_COST"}}},
      {ViewType::storage,
       "view_storage",
       "view_config_sts.yml",
       "antares_legacy_short_term_storage_catalog",
       {{"STS inj by plant", "P_INJECTION"},
        {"STS withdrawal by plant", "P_WITHDRAWAL"},
        {"STS lvl by plant", "LEVELS"},
        {"STS Cashflow By Cluster", "CASHFLOW"}}},
      {ViewType::renewable,
       "view_renewable",
       "view_config_renewable.yml",
       "antares_legacy_renewable_cluster_catalog",
       {{"RES generation by plant", "GENERATION"}}},
    };
    auto& variables = study->parameters.variablesPrintInfo;
    for (const auto& test: cases)
    {
        BOOST_TEST_CONTEXT(test.id)
        {
            study->parameters.thematicTrimming = false;
            variables.setAllPrintStatusesTo(false);
            Benchmarking::DurationCollector durationCollector;
            Antares::Solver::InMemoryWriter writer(durationCollector);
            BOOST_REQUIRE(exportViewConfig(*study, &writer, test.type));
            const auto view = YAML::Load(writer.getMap().at("views/input/" + test.filename))["view"];
            BOOST_CHECK_EQUAL(view["id"].as<std::string>(), test.id);
            BOOST_CHECK_EQUAL(view["scope"]["location"]["taxonomy-category"].as<std::string>(),
                              test.type == ViewType::link ? "link" : "balance");
            BOOST_CHECK_EQUAL(view["aggregations-patterns"].size(), 10);
            BOOST_CHECK_EQUAL(view["metrics"].size(), test.mappings.size());
            std::set<std::string> expectedMetrics;
            for (const auto& [variable, metric]: test.mappings)
            {
                expectedMetrics.insert(test.catalog + "." + metric);
            }
            BOOST_CHECK(metricIds(view) == expectedMetrics);

            study->parameters.thematicTrimming = true;
            Benchmarking::DurationCollector emptyDurationCollector;
            Antares::Solver::InMemoryWriter emptyWriter(emptyDurationCollector);
            BOOST_CHECK(!exportViewConfig(*study, &emptyWriter, test.type));
            BOOST_CHECK(emptyWriter.getMap().empty());
            for (const auto& [variable, metric]: test.mappings)
            {
                BOOST_TEST_CONTEXT(variable)
                {
                    BOOST_REQUIRE(variables.exists(variable));
                    variables.setAllPrintStatusesTo(false);
                    auto lowercaseVariable = variable;
                    std::transform(lowercaseVariable.begin(),
                                   lowercaseVariable.end(),
                                   lowercaseVariable.begin(),
                                   [](unsigned char ch) { return std::tolower(ch); });
                    variables.setPrintStatus(lowercaseVariable, true);
                    const auto selected = generateViewConfig(*study, test.type)["view"];
                    BOOST_REQUIRE_EQUAL(selected["metrics"].size(), 1);
                    BOOST_CHECK_EQUAL(selected["metrics"][0]["id"].as<std::string>(),
                                      test.catalog + "." + metric);
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(cluster_views_share_area_and_district_aggregations)
{
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = filterHourly;
    de->filterYearByYear = filterNone;
    fr->filterSynthesis = filterNone;
    de->filterSynthesis = filterNone;
    auto members = std::make_shared<Sets::SetAreasType>();
    members->insert(fr);
    Sets::Options options;
    options.filterYearByYear = filterNone;
    options.filterSynthesis = filterDaily;
    study->setsOfAreas.add("eu", members, options);
    for (auto type: {ViewType::thermal, ViewType::storage, ViewType::renewable})
    {
        const auto view = generateViewConfig(*study, type)["view"];
        BOOST_REQUIRE_EQUAL(view["scope"]["extra-locations"].size(), 1);
        BOOST_CHECK_EQUAL(view["scope"]["extra-locations"][0]["id"].as<std::string>(),
                          "district_eu");
        BOOST_REQUIRE_EQUAL(view["aggregations-patterns"].size(), 2);
        BOOST_CHECK(locations(findPattern(view, "hour_ind")) == (std::set<std::string>{"france"}));
        BOOST_CHECK(locations(findPattern(view, "day_all")) == (std::set<std::string>{"eu"}));
    }
}

BOOST_AUTO_TEST_CASE(cluster_views_skipped_without_any_aggregation)
{
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = fr->filterSynthesis = filterNone;
    de->filterYearByYear = de->filterSynthesis = filterNone;
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    for (auto type: {ViewType::thermal, ViewType::storage, ViewType::renewable})
    {
        BOOST_CHECK(!exportViewConfig(*study, &writer, type));
    }
    BOOST_CHECK(writer.getMap().empty());
}

BOOST_AUTO_TEST_CASE(link_view_uses_link_filters_not_area_or_district_filters)
{
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = filterNone;
    de->filterYearByYear = filterNone;
    auto* it = addAreaToListOfAreas(study->areas, "italy");
    AreaAddLinkBetweenAreas(de, it);
    auto& first = *fr->links.begin()->second;
    auto& second = *de->links.begin()->second;
    first.filterYearByYear = filterHourly | filterDaily;
    first.filterSynthesis = filterDaily;
    second.filterYearByYear = filterDaily;
    second.filterSynthesis = filterNone;
    auto members = std::make_shared<Sets::SetAreasType>();
    members->insert(fr);
    Sets::Options options;
    study->setsOfAreas.add("eu", members, options);

    const auto view = generateViewConfig(*study, ViewType::link)["view"];
    BOOST_CHECK(!view["scope"]["extra-locations"].IsDefined());
    BOOST_REQUIRE_EQUAL(view["aggregations-patterns"].size(), 3);
    BOOST_CHECK(locations(findPattern(view, "hour_ind"))
                == (std::set<std::string>{"france_germany"}));
    BOOST_CHECK(!findPattern(view, "day_ind")["location-filter"].IsDefined());
    BOOST_CHECK(locations(findPattern(view, "day_all"))
                == (std::set<std::string>{"france_germany"}));

    first.filterYearByYear = first.filterSynthesis = filterNone;
    second.filterYearByYear = second.filterSynthesis = filterNone;
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    BOOST_CHECK(!exportViewConfig(*study, &writer, ViewType::link));
    BOOST_CHECK(writer.getMap().empty());
}

BOOST_AUTO_TEST_CASE(area_view_thematic_variable_mappings)
{
    study->parameters.thematicTrimming = true;
    auto& variables = study->parameters.variablesPrintInfo;
    const std::vector<std::pair<std::string, std::string>> mappings = {
      {"OV. COST", "OV_COST"},
      {"OP. COST", "OP_COST"},
      {"MRG. PRICE", "MRG_PRICE"},
      {"CO2 EMIS.", "CO2_EMIS"},
      {"NH3 EMIS.", "NH3_EMIS"},
      {"SO2 EMIS.", "SO2_EMIS"},
      {"NOX EMIS.", "NOX_EMIS"},
      {"PM2_5 EMIS.", "PM2_5_EMIS"},
      {"PM5 EMIS.", "PM5_EMIS"},
      {"PM10 EMIS.", "PM10_EMIS"},
      {"NMVOC EMIS.", "NMVOC_EMIS"},
      {"OP1 EMIS.", "OP1_EMIS"},
      {"OP2 EMIS.", "OP2_EMIS"},
      {"OP3 EMIS.", "OP3_EMIS"},
      {"OP4 EMIS.", "OP4_EMIS"},
      {"OP5 EMIS.", "OP5_EMIS"},
      {"LOAD", "LOAD"},
      {"RES LOAD", "RES_LOAD"},
      {"Dispatch. Gen.", "DTG_BY_TECHNOLOGY"},
      {"BALANCE", "BALANCE"},
      {"Renewable Gen.", "RES_GENERATION"},
      {"WIND", "RES_GENERATION"},
      {"SOLAR", "RES_GENERATION"},
      {"H. ROR", "RES_GENERATION"},
      {"UNSP. ENRG", "UNSP_ENRG"},
      {"SPIL. ENRG", "SPIL_ENRG"},
      {"NODU", "NODU"},
      {"NP COST", "NP_COST"},
      {"AVL DTG", "AVL_DTG"},
      {"DTG MRG", "DTG_MRG"},
      {"LOLD", "LOLD"},
      {"LOLP", "LOLP"},
      {"NPCAP HOURS", "NPCAP_HOURS"},
      {"ROW BAL.", "ROW_BAL"},
      {"MISC. NDG", "MISC_NDG"},
      {"PSP", "PSP"},
      {"H. STOR", "H_STOR"},
      {"H. PUMP", "H_PUMP"},
      {"H. OVFL", "H_OVFL"},
      {"H. INFL", "H_INFL"},
      {"H. LEV", "H_LEV"},
      {"H. VAL", "H_VAL"},
      {"H. COST", "H_COST"},
    };
    for (const auto& [variable, metric]: mappings)
    {
        BOOST_TEST_CONTEXT(variable)
        {
            BOOST_REQUIRE(variables.exists(variable));
            variables.setAllPrintStatusesTo(false);
            variables.setPrintStatus(variable, true);
            const auto view = generateAreaViewConfig(*study)["view"];
            BOOST_REQUIRE_EQUAL(view["metrics"].size(), 1);
            BOOST_CHECK_EQUAL(view["metrics"][0]["id"].as<std::string>(),
                              "antares_legacy_area_catalog." + metric);
        }
    }

    variables.setAllPrintStatusesTo(false);
    variables.setPrintStatus("sts BY group", true);
    const auto view = generateAreaViewConfig(*study)["view"];
    BOOST_REQUIRE_EQUAL(view["metrics"].size(), 3);
    BOOST_CHECK(metricIds(view)
                == (std::set<std::string>{"antares_legacy_area_catalog.STS_INJECTION",
                                          "antares_legacy_area_catalog.STS_WITHDRAWAL",
                                          "antares_legacy_area_catalog.STS_LEVEL"}));
}

BOOST_AUTO_TEST_CASE(area_view_thematic_selection_combines_with_geographic_trimming)
{
    study->parameters.thematicTrimming = true;
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = filterHourly;
    de->filterYearByYear = filterNone;
    auto& variables = study->parameters.variablesPrintInfo;
    variables.setAllPrintStatusesTo(false);
    for (const auto* variable: {"wind", "solar", "h. ror", "load"})
    {
        variables.setPrintStatus(variable, true);
    }

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    BOOST_REQUIRE(exportViewConfig(*study, &writer, ViewType::area));
    const auto view = YAML::Load(writer.getMap().at("views/input/view_config_area.yml"))["view"];
    BOOST_REQUIRE_EQUAL(view["metrics"].size(), 2);
    BOOST_CHECK(metricIds(view)
                == (std::set<std::string>{"antares_legacy_area_catalog.LOAD",
                                          "antares_legacy_area_catalog.RES_GENERATION"}));
    BOOST_CHECK(locations(findPattern(view, "hour_ind")) == (std::set<std::string>{"france"}));
}

BOOST_AUTO_TEST_CASE(area_view_skipped_when_no_mapped_metric_is_selected)
{
    study->parameters.thematicTrimming = true;
    auto& variables = study->parameters.variablesPrintInfo;
    variables.setAllPrintStatusesTo(false);
    variables.setPrintStatus("FLOW LIN.", true);
    const auto view = generateAreaViewConfig(*study)["view"];
    BOOST_CHECK(view["metrics"].IsSequence());
    BOOST_CHECK_EQUAL(view["metrics"].size(), 0);
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    BOOST_CHECK(!exportViewConfig(*study, &writer, ViewType::area));
    BOOST_CHECK(writer.getMap().empty());
}

BOOST_AUTO_TEST_CASE(area_view_ignores_variable_statuses_without_thematic_trimming)
{
    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(false);
    BOOST_CHECK_EQUAL(generateAreaViewConfig(*study)["view"]["metrics"].size(), 43);
}

BOOST_AUTO_TEST_CASE(area_view_with_all_thematic_variables_selected)
{
    study->parameters.thematicTrimming = true;
    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    const auto view = generateAreaViewConfig(*study)["view"];
    BOOST_CHECK_EQUAL(view["metrics"].size(), 43);
    BOOST_CHECK_EQUAL(metricIds(view).size(), 43);
}

BOOST_AUTO_TEST_CASE(area_view_uses_loaded_variable_selection)
{
    std::stringstream stream(R"([general]
thematic-trimming = true
[variables selection]
selected_vars_reset = false
select_var + = LoAd
select_var + = WIND
select_var + = SoLaR
select_var - = wind
select_var + = STS by group
[other preferences]
renewable-generation-modelling = aggregated
)");
    Antares::IniFile ini;
    ini.readStream(stream);
    BOOST_REQUIRE(study->parameters.loadFromINI(ini, StudyVersion::latest()));
    study->parameters.validateOptions(StudyLoadOptions{});
    const auto view = generateAreaViewConfig(*study)["view"];
    BOOST_REQUIRE_EQUAL(view["metrics"].size(), 5);
    BOOST_CHECK(metricIds(view)
                == (std::set<std::string>{"antares_legacy_area_catalog.LOAD",
                                          "antares_legacy_area_catalog.RES_GENERATION",
                                          "antares_legacy_area_catalog.STS_INJECTION",
                                          "antares_legacy_area_catalog.STS_WITHDRAWAL",
                                          "antares_legacy_area_catalog.STS_LEVEL"}));

    // The solver excludes aggregated renewable variables in cluster mode.
    study->parameters.renewableGeneration.toClusters();
    study->parameters.validateOptions(StudyLoadOptions{});
    const auto clusterView = generateAreaViewConfig(*study)["view"];
    BOOST_CHECK(metricIds(clusterView)
                == (std::set<std::string>{"antares_legacy_area_catalog.LOAD",
                                          "antares_legacy_area_catalog.STS_INJECTION",
                                          "antares_legacy_area_catalog.STS_WITHDRAWAL",
                                          "antares_legacy_area_catalog.STS_LEVEL"}));
}

BOOST_AUTO_TEST_CASE(area_view_without_geographic_trimming)
{
    fr->filterYearByYear = filterNone;
    de->filterSynthesis = filterNone;
    auto members = std::make_shared<Sets::SetAreasType>();
    members->insert(fr);
    Sets::Options options;
    options.filterYearByYear = filterNone;
    study->setsOfAreas.add("eu", members, options);

    const YAML::Node view = generateAreaViewConfig(*study)["view"];
    BOOST_CHECK_EQUAL(view["id"].as<std::string>(), "view_area");
    BOOST_CHECK_EQUAL(view["taxonomy"].as<std::string>(), "antares_legacy_taxonomy");
    BOOST_CHECK_EQUAL(view["scope"]["location"]["taxonomy-category"].as<std::string>(), "balance");
    BOOST_REQUIRE_EQUAL(view["scope"]["extra-locations"].size(), 1);
    BOOST_CHECK_EQUAL(view["scope"]["extra-locations"][0]["id"].as<std::string>(), "district_eu");
    BOOST_REQUIRE_EQUAL(view["aggregations-patterns"].size(), 10);
    for (const auto& granularity: {"hour", "day", "week", "month", "year"})
    {
        for (bool scenario: {false, true})
        {
            const auto pattern = findPattern(view,
                                             std::string(granularity)
                                               + (scenario ? "_all" : "_ind"));
            BOOST_REQUIRE(pattern.IsDefined());
            BOOST_CHECK_EQUAL(pattern["time-granularity"].as<std::string>(), granularity);
            BOOST_CHECK_EQUAL(pattern["scenario"].as<bool>(), scenario);
            BOOST_CHECK(!pattern["location-filter"].IsDefined());
        }
    }
    BOOST_REQUIRE_EQUAL(view["metrics"].size(), 43);
    BOOST_CHECK_EQUAL(view["metrics"][0]["id"].as<std::string>(),
                      "antares_legacy_area_catalog.OV_COST");
    BOOST_CHECK_EQUAL(view["metrics"][42]["id"].as<std::string>(),
                      "antares_legacy_area_catalog.H_COST");
}

BOOST_AUTO_TEST_CASE(area_view_geographic_trimming_includes_enabled_districts)
{
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = filterHourly | filterDaily;
    de->filterYearByYear = filterDaily;
    fr->filterSynthesis = filterDaily;
    de->filterSynthesis = filterDaily;

    auto members = std::make_shared<Sets::SetAreasType>();
    members->insert(fr);
    members->insert(de);
    Sets::Options options;
    options.filterYearByYear = filterHourly | filterDaily;
    options.filterSynthesis = filterHourly | filterDaily;
    study->setsOfAreas.add("eu", members, options);
    options.output = false;
    study->setsOfAreas.add("disabled", members, options);
    options.output = true;
    study->setsOfAreas.add("empty", std::make_shared<Sets::SetAreasType>(), options);

    const YAML::Node view = generateAreaViewConfig(*study)["view"];
    BOOST_REQUIRE_EQUAL(view["scope"]["extra-locations"].size(), 1);
    BOOST_CHECK_EQUAL(view["scope"]["extra-locations"][0]["id"].as<std::string>(), "district_eu");
    BOOST_REQUIRE_EQUAL(view["aggregations-patterns"].size(), 4);
    BOOST_CHECK(locations(findPattern(view, "hour_ind"))
                == (std::set<std::string>{"eu", "france"}));
    BOOST_CHECK(locations(findPattern(view, "hour_all")) == (std::set<std::string>{"eu"}));
    BOOST_CHECK(!findPattern(view, "day_ind")["location-filter"].IsDefined());
    BOOST_CHECK(!findPattern(view, "day_all")["location-filter"].IsDefined());
    BOOST_CHECK(!findPattern(view, "week_ind").IsDefined());
    BOOST_CHECK(!findPattern(view, "week_all").IsDefined());
}

BOOST_AUTO_TEST_CASE(area_view_skipped_when_no_aggregation_is_selected)
{
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = filterNone;
    de->filterYearByYear = filterNone;
    fr->filterSynthesis = filterNone;
    de->filterSynthesis = filterNone;
    const auto view = generateAreaViewConfig(*study)["view"];
    BOOST_CHECK(view["aggregations-patterns"].IsSequence());
    BOOST_CHECK_EQUAL(view["aggregations-patterns"].size(), 0);
    BOOST_CHECK_GT(view["metrics"].size(), 0);
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    BOOST_CHECK(!exportViewConfig(*study, &writer, ViewType::area));
    BOOST_CHECK(writer.getMap().empty());
}

BOOST_AUTO_TEST_CASE(area_view_written_when_only_district_aggregation_is_selected)
{
    study->parameters.geographicTrimming = true;
    fr->filterYearByYear = filterNone;
    de->filterYearByYear = filterNone;
    fr->filterSynthesis = filterNone;
    de->filterSynthesis = filterNone;
    auto members = std::make_shared<Sets::SetAreasType>();
    members->insert(fr);
    Sets::Options options;
    options.filterYearByYear = filterNone;
    options.filterSynthesis = filterHourly;
    study->setsOfAreas.add("eu", members, options);

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    BOOST_REQUIRE(exportViewConfig(*study, &writer, ViewType::area));
    const auto view = YAML::Load(writer.getMap().at("views/input/view_config_area.yml"))["view"];
    BOOST_REQUIRE_EQUAL(view["aggregations-patterns"].size(), 1);
    BOOST_CHECK(locations(findPattern(view, "hour_all")) == (std::set<std::string>{"eu"}));
}

BOOST_AUTO_TEST_CASE(export_area_view_uses_views_input_directory)
{
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    exportSystemForView(*study, &writer);
    exportViewConfig(*study, &writer, ViewType::area);
    BOOST_REQUIRE_EQUAL(writer.getMap().size(), 2);
    BOOST_CHECK(writer.getMap().contains("views/input/system-for-views.yml"));
    BOOST_REQUIRE(writer.getMap().contains("views/input/view_config_area.yml"));
    const auto view = YAML::Load(writer.getMap().at("views/input/view_config_area.yml"))["view"];
    BOOST_CHECK_EQUAL(view["aggregations-patterns"].size(), 10);
    BOOST_CHECK_EQUAL(view["id"].as<std::string>(), "view_area");
}

BOOST_AUTO_TEST_CASE(study_to_yaml_structure)
{
    YAML::Node root = generateSystemForView(*study);
    BOOST_REQUIRE(root.IsMap());
    BOOST_REQUIRE(root["system"].IsDefined());

    auto sys = root["system"];

    auto components = sys["components"];
    BOOST_REQUIRE(components.IsSequence());

    // fr: area(1) + load(1) + ror(1) + miscGen(8) +
    //     thermal(1) + renewable(1) + sts(1) + hydro(1) + link(1) = 16
    // de: area(1) + load(1) + ror(1) + miscGen(8) +
    //     hydro(1) = 12
    // total: 28
    BOOST_CHECK_EQUAL(components.size(), 28);

    auto connections = sys["connections"];
    BOOST_REQUIRE(connections.IsSequence());
    // fr sub-components: load(1) + ror(1) + miscGen(8) +
    //     thermal(1) + renewable(1) + sts(1) + hydro(1) = 14
    // de sub-components: load(1) + ror(1) + miscGen(8) +
    //     hydro(1) = 11
    // link: in_port + out_port = 2
    // total: 27
    BOOST_CHECK_EQUAL(connections.size(), 27);
}

BOOST_AUTO_TEST_CASE(historical_renewables_are_omitted_in_cluster_mode)
{
    // The fixture keeps both legacy time series non-zero.
    const YAML::Node components = generateSystemForView(*study)["system"]["components"];

    for (const auto& component: components)
    {
        const auto id = component["id"].as<std::string>();
        BOOST_CHECK(id != "france_wind");
        BOOST_CHECK(id != "france_solar");
        BOOST_CHECK(id != "germany_wind");
        BOOST_CHECK(id != "germany_solar");
    }
}

BOOST_FIXTURE_TEST_CASE(disabled_components_are_omitted, ViewBuilderFixture)
{
    thermalCluster->enabled = false;
    renewableCluster->enabled = false;
    fr->shortTermStorage.storagesByIndex.front().properties.enabled = false;

    // Keep the load active to ensure the test also preserves enabled components.
    fr->wind.series.fill(0.0);
    fr->solar.series.fill(0.0);
    fr->hydro.series->ror.fill(0.0);
    fr->hydro.series->storage.fill(0.0);
    fr->hydro.reservoirManagement = false;
    fr->miscGen.zero();
    fr->links.begin()->second->transmissionCapacities = LocalTransmissionCapacities::null;

    const YAML::Node root = generateSystemForView(*study);
    const auto components = root["system"]["components"];
    BOOST_REQUIRE(components.IsSequence());

    const std::vector<std::string> omittedIds = {
      "france_wind",
      "france_solar",
      "france_combined_heat_power",
      "france_biomass",
      "france_biogas",
      "france_waste",
      "france_geothermal",
      "france_other",
      "france_pumped_storage_power",
      "france_rest_world",
      "france_ror",
      "france_thermal_nuc_fr",
      "france_renewable_wind_fr",
      "france_short_term_storage_battery_fr",
      "france_hydro_storage",
      "france_germany",
    };

    const auto componentIsPresent = [&components](const auto& id)
    {
        return std::any_of(components.begin(),
                           components.end(),
                           [&id](const YAML::Node& component)
                           { return component["id"].as<std::string>() == id; });
    };

    // Load = 1 for area `france`
    BOOST_CHECK(componentIsPresent("france_load"));
    for (const auto& id: omittedIds)
    {
        BOOST_CHECK_MESSAGE(!componentIsPresent(id),
                            "Disabled component unexpectedly present: " << id);
    }
}

BOOST_AUTO_TEST_CASE(area_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.area");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            checkProperties(props[0], "carrier", "electricity");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(area_components_have_district_properties)
{
    auto firstDistrict = std::make_shared<Antares::Data::Sets::SetAreasType>();
    firstDistrict->insert(fr);
    Antares::Data::Sets::Options firstOptions;
    firstOptions.caption = "it";
    study->setsOfAreas.add("it", firstDistrict, firstOptions);

    auto secondDistrict = std::make_shared<Antares::Data::Sets::SetAreasType>();
    secondDistrict->insert(fr);
    secondDistrict->insert(de);
    Antares::Data::Sets::Options secondOptions;
    secondOptions.caption = "eu";
    study->setsOfAreas.add("eu", secondDistrict, secondOptions);
    study->setsOfAreas.rebuildIndexes();

    const YAML::Node components = generateSystemForView(*study)["system"]["components"];
    const auto findComponent = [&components](const std::string& id)
    {
        return std::find_if(components.begin(),
                            components.end(),
                            [&id](const YAML::Node& component)
                            { return component["id"].as<std::string>() == id; });
    };

    const auto franceNode = findComponent("france");
    BOOST_REQUIRE(franceNode != components.end());
    BOOST_REQUIRE_EQUAL((*franceNode)["properties"].size(), 3);
    checkProperties((*franceNode)["properties"][1], "district_eu", "eu");
    checkProperties((*franceNode)["properties"][2], "district_it", "it");

    const auto germanyNode = findComponent("germany");
    BOOST_REQUIRE(germanyNode != components.end());
    BOOST_REQUIRE_EQUAL((*germanyNode)["properties"].size(), 2);
    checkProperties((*germanyNode)["properties"][1], "district_eu", "eu");
}

BOOST_AUTO_TEST_CASE(load_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_load")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.load");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            checkProperties(props[0], "carrier", "electricity");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(wind_component)
{
    study->parameters.renewableGeneration.toAggregated();
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_wind")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.renewable");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            checkProperties(props[0], "carrier", "electricity");
            checkProperties(props[1], "technology", "wind");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(solar_component)
{
    study->parameters.renewableGeneration.toAggregated();
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_solar")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.renewable");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            checkProperties(props[0], "carrier", "electricity");
            checkProperties(props[1], "technology", "solar");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(thermal_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_thermal_nuc_fr")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.thermal");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 3);
            checkProperties(props[0], "carrier", "electricity");
            checkProperties(props[1], "name", "france_thermal_nuc_fr");
            checkProperties(props[2], "technology", "other");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(renewable_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_renewable_wind_fr")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.renewable");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 3);
            checkProperties(props[0], "carrier", "electricity");
            checkProperties(props[1], "name", "france_renewable_wind_fr");
            checkProperties(props[2], "technology", "other");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(sts_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_short_term_storage_battery_fr")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(),
                              "antares_legacy_models.short_term_storage");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 3);
            checkProperties(props[0], "carrier", "electricity");
            checkProperties(props[1], "name", "france_short_term_storage_battery_fr");
            checkProperties(props[2], "group", "other1");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(hydro_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_hydro_storage")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(),
                              "antares_legacy_models.long_term_storage");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            checkProperties(props[0], "carrier", "electricity");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(link_component)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    bool found = false;
    for (const auto& comp: components)
    {
        if (comp["id"].as<std::string>() == "france_germany")
        {
            found = true;
            BOOST_CHECK_EQUAL(comp["model"].as<std::string>(), "antares_legacy_models.link");
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 1);
            checkProperties(props[0], "carrier", "electricity");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(misc_gen_components)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    struct MiscGenTestCase
    {
        std::string name;
        std::string expectedTechnology;
        std::string expectedMiscType;
    };

    std::vector<MiscGenTestCase> miscGenCases = {
      {"combined_heat_power", "combined_heat_power", "misc_ndg"},
      {"biomass", "biomass", "misc_ndg"},
      {"biogas", "biogas", "misc_ndg"},
      {"waste", "waste", "misc_ndg"},
      {"geothermal", "geothermal", "misc_ndg"},
      {"other", "other", "misc_ndg"},
      {"pumped_storage_power", "pumped_storage_power", "pumped_storage_power"},
      {"rest_world", "rest_world", "rest_world"},
    };

    for (const auto& [name, expectedTech, expectedMiscType]: miscGenCases)
    {
        std::string expectedId = "france_" + name;
        bool found = false;
        for (const auto& comp: components)
        {
            if (comp["id"].as<std::string>() == expectedId)
            {
                found = true;
                BOOST_CHECK_EQUAL(comp["model"].as<std::string>(),
                                  "antares_legacy_models.miscellaneous_generation");
                auto props = comp["properties"];
                BOOST_REQUIRE(props.IsSequence());
                BOOST_REQUIRE_EQUAL(props.size(), 3);
                checkProperties(props[0], "carrier", "electricity");
                checkProperties(props[1], "technology", expectedTech);
                checkProperties(props[2], "miscellaneous_type", expectedMiscType);
            }
        }
        BOOST_CHECK_MESSAGE(found, "MiscGen component not found: " + expectedId);
    }
}

BOOST_AUTO_TEST_CASE(parameters_are_empty)
{
    YAML::Node root = generateSystemForView(*study);
    auto components = root["system"]["components"];

    for (const auto& comp: components)
    {
        auto params = comp["parameters"];
        BOOST_REQUIRE(params.IsSequence());
        BOOST_CHECK_EQUAL(params.size(), 0);
    }
}

BOOST_AUTO_TEST_CASE(round_trip_yaml_parse)
{
    YAML::Node root = generateSystemForView(*study);
    std::string yamlStr = YAML::Dump(root);

    YAML::Node reparsed = YAML::Load(yamlStr);
    BOOST_REQUIRE(reparsed.IsMap());
    BOOST_REQUIRE(reparsed["system"].IsDefined());

    auto sys = reparsed["system"];
    BOOST_REQUIRE(sys["components"].IsSequence());
    BOOST_CHECK_EQUAL(sys["components"].size(), 28);
    BOOST_REQUIRE(sys["connections"].IsSequence());
    BOOST_CHECK_EQUAL(sys["connections"].size(), 27);
}

BOOST_AUTO_TEST_CASE(connections_structure)
{
    YAML::Node root = generateSystemForView(*study);
    auto connections = root["system"]["connections"];

    BOOST_REQUIRE(connections.IsSequence());
    BOOST_CHECK_EQUAL(connections.size(), 27);

    for (const auto& conn: connections)
    {
        BOOST_REQUIRE(conn["component1"].IsDefined());
        BOOST_REQUIRE(conn["port1"].IsDefined());
        BOOST_REQUIRE(conn["component2"].IsDefined());
        BOOST_REQUIRE(conn["port2"].IsDefined());
        BOOST_CHECK(conn["component1"].as<std::string>() != conn["component2"].as<std::string>());
    }
}

BOOST_AUTO_TEST_CASE(connection_for_load)
{
    YAML::Node root = generateSystemForView(*study);
    auto connections = root["system"]["connections"];

    bool found = false;
    for (const auto& conn: connections)
    {
        if (conn["component1"].as<std::string>() == "france_load"
            && conn["component2"].as<std::string>() == "france")
        {
            found = true;
            BOOST_CHECK_EQUAL(conn["port1"].as<std::string>(), "balance_port");
            BOOST_CHECK_EQUAL(conn["port2"].as<std::string>(), "balance_port");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(connection_for_thermal)
{
    YAML::Node root = generateSystemForView(*study);
    auto connections = root["system"]["connections"];

    bool found = false;
    for (const auto& conn: connections)
    {
        if (conn["component1"].as<std::string>() == "france_thermal_nuc_fr"
            && conn["component2"].as<std::string>() == "france")
        {
            found = true;
            BOOST_CHECK_EQUAL(conn["port1"].as<std::string>(), "balance_port");
            BOOST_CHECK_EQUAL(conn["port2"].as<std::string>(), "balance_port");
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE(connection_for_link)
{
    YAML::Node root = generateSystemForView(*study);
    auto connections = root["system"]["connections"];

    bool foundInPort = false;
    bool foundOutPort = false;
    for (const auto& conn: connections)
    {
        if (conn["component1"].as<std::string>() == "france_germany")
        {
            if (conn["port1"].as<std::string>() == "in_port"
                && conn["component2"].as<std::string>() == "france"
                && conn["port2"].as<std::string>() == "balance_port")
            {
                foundInPort = true;
            }
            if (conn["port1"].as<std::string>() == "out_port"
                && conn["component2"].as<std::string>() == "germany"
                && conn["port2"].as<std::string>() == "balance_port")
            {
                foundOutPort = true;
            }
        }
    }
    BOOST_CHECK_MESSAGE(foundInPort, "Missing link in_port -> france connection");
    BOOST_CHECK_MESSAGE(foundOutPort, "Missing link out_port -> germany connection");
}

using namespace Antares::ModelerStudy::SystemModel;

namespace
{

Antares::ModelerStudy::SystemModel::Model buildDummyModel()
{
    return Antares::ModelerStudy::SystemModel::ModelBuilder()
      .withId("dummy_model")
      .withLibraryId("dummy_library")
      .build();
}

} // anonymous namespace

BOOST_AUTO_TEST_CASE(duplicate_id_between_legacy_and_modeler_throws)
{
    Model model = buildDummyModel();

    Library library = LibraryBuilder().withId("dummy_library").build();

    std::vector<Component> components;
    components.emplace_back(ComponentBuilder().withId("france").withModel(&model).build());

    SystemBuilder systemBuilder;
    auto system = systemBuilder.withId("test_system").withComponents(std::move(components)).build();

    auto modelerData = std::make_unique<Antares::Solver::ModelerData>();
    modelerData->libraries = {library};
    modelerData->system = std::make_unique<System>(std::move(system));

    study->setModelerData(std::move(modelerData));

    BOOST_CHECK_THROW(generateSystemForView(*study), Antares::IO::Inputs::InputError);
}

BOOST_AUTO_TEST_CASE(no_duplicate_id_passes)
{
    Model model = buildDummyModel();

    Library library = LibraryBuilder().withId("dummy_library").build();

    std::vector<Component> components;
    components.emplace_back(
      ComponentBuilder().withId("unique_modeler_component").withModel(&model).build());

    SystemBuilder systemBuilder;
    auto system = systemBuilder.withId("test_system").withComponents(std::move(components)).build();

    auto modelerData = std::make_unique<Antares::Solver::ModelerData>();
    modelerData->libraries = {library};
    modelerData->system = std::make_unique<System>(std::move(system));

    study->setModelerData(std::move(modelerData));

    YAML::Node root = generateSystemForView(*study);
    BOOST_REQUIRE(root.IsMap());
    BOOST_REQUIRE(root["system"].IsDefined());

    auto componentsNode = root["system"]["components"];
    BOOST_REQUIRE(componentsNode.IsSequence());
    BOOST_CHECK_EQUAL(componentsNode.size(), 29);

    const auto it = std::find_if(componentsNode.begin(),
                                 componentsNode.end(),
                                 [](const YAML::Node& comp) {
                                     return comp["id"].as<std::string>()
                                            == "unique_modeler_component";
                                 });
    BOOST_CHECK(it != componentsNode.end());
}

BOOST_AUTO_TEST_CASE(modeler_component_properties_are_output)
{
    Model model = buildDummyModel();

    Library library = LibraryBuilder().withId("dummy_library").build();

    std::vector<Component> components;
    components.emplace_back(ComponentBuilder().withId("my_component").withModel(&model).build());

    SystemBuilder systemBuilder;
    auto system = systemBuilder.withId("test_system").withComponents(std::move(components)).build();

    auto modelerData = std::make_unique<Antares::Solver::ModelerData>();
    modelerData->libraries = {library};
    modelerData->system = std::make_unique<System>(std::move(system));
    modelerData->componentProperties["my_component"] = {{"carrier", "electricity"},
                                                        {"technology", "solar"}};

    study->setModelerData(std::move(modelerData));

    YAML::Node root = generateSystemForView(*study);
    auto componentsNode = root["system"]["components"];

    bool found = false;
    for (const auto& comp: componentsNode)
    {
        if (comp["id"].as<std::string>() == "my_component")
        {
            found = true;
            auto props = comp["properties"];
            BOOST_REQUIRE(props.IsSequence());
            BOOST_REQUIRE_EQUAL(props.size(), 2);
            checkProperties(props[0], "carrier", "electricity");
            checkProperties(props[1], "technology", "solar");
        }
    }
    BOOST_CHECK_MESSAGE(found, "modeler component 'my_component' not found in output");
}

BOOST_AUTO_TEST_CASE(modeler_component_without_properties_has_no_properties_field)
{
    Model model = buildDummyModel();

    Library library = LibraryBuilder().withId("dummy_library").build();

    std::vector<Component> components;
    components.emplace_back(ComponentBuilder().withId("bare_component").withModel(&model).build());

    SystemBuilder systemBuilder;
    auto system = systemBuilder.withId("test_system").withComponents(std::move(components)).build();

    auto modelerData = std::make_unique<Antares::Solver::ModelerData>();
    modelerData->libraries = {library};
    modelerData->system = std::make_unique<System>(std::move(system));

    study->setModelerData(std::move(modelerData));

    YAML::Node root = generateSystemForView(*study);
    auto componentsNode = root["system"]["components"];

    for (const auto& comp: componentsNode)
    {
        if (comp["id"].as<std::string>() == "bare_component")
        {
            BOOST_CHECK(!comp["properties"].IsDefined());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
