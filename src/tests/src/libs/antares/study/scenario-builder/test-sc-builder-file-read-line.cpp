// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test read scenario - builder.dat

#define WIN32_LEAN_AND_MEAN

#include <functional>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/study/scenario-builder/rules.h>
#include <antares/study/study.h>

using namespace std;
using namespace Antares::Data;

// ===========================
// Add a cluster to an area
// ===========================

void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
    area->thermal.list.addToCompleteList(cluster);
    area->thermal.list.buildIndexes();
}

void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
    area->renewable.list.addToCompleteList(cluster);
    area->renewable.list.buildIndexes();
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
    auto cluster = std::make_shared<ClusterType>(area);
    cluster->setName(clusterName);

    addClusterToAreaList(area, cluster);

    return cluster;
}

// =================
// The fixture
// =================
struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;

    Fixture():
        my_rule(*study)
    {
        // Set study parameters
        study->parameters.nbYears = 20;
        study->parameters.timeSeriesToGenerate = 0; // No generated time-series, only ready made
                                                    // time-series

        // Add areas
        area_1 = addAreaToListOfAreas(study->areas, "Area 1");
        area_2 = addAreaToListOfAreas(study->areas, "Area 2");
        area_3 = addAreaToListOfAreas(study->areas, "Area 3");
        for (auto* area: {area_1, area_2, area_3})
        {
            if (area)
            {
                area->createMissingData();
            }
        }
        study->areas.rebuildIndexes();

        // Load : set the nb of ready made TS
        uint nbReadyMadeTS = 13;
        area_1->load.series.timeSeries.resize(nbReadyMadeTS, 1);
        area_2->load.series.timeSeries.resize(nbReadyMadeTS, 1);
        area_3->load.series.timeSeries.resize(nbReadyMadeTS, 1);

        // Wind : set the nb of ready made TS
        nbReadyMadeTS = 17;
        area_1->wind.series.timeSeries.resize(nbReadyMadeTS, 1);
        area_2->wind.series.timeSeries.resize(nbReadyMadeTS, 1);
        area_3->wind.series.timeSeries.resize(nbReadyMadeTS, 1);

        // Links
        link_12 = AreaAddLinkBetweenAreas(area_1, area_2, false);
        link_12->directCapacities.resize(15, 1);
        // Solar : set the nb of ready made TS
        nbReadyMadeTS = 9;
        area_1->solar.series.timeSeries.resize(nbReadyMadeTS, 1);
        area_2->solar.series.timeSeries.resize(nbReadyMadeTS, 1);
        area_3->solar.series.timeSeries.resize(nbReadyMadeTS, 1);

        // Hydro : set the nb of ready made TS
        nbReadyMadeTS = 12;
        area_1->hydro.series->resizeTS(nbReadyMadeTS);
        area_2->hydro.series->resizeTS(nbReadyMadeTS);
        area_3->hydro.series->resizeTS(nbReadyMadeTS);

        // Links
        link_12 = AreaAddLinkBetweenAreas(area_1, area_2, false);
        link_12->directCapacities.resize(15, 1);

        link_13 = AreaAddLinkBetweenAreas(area_1, area_3, false);
        link_13->directCapacities.resize(11, 1);

        link_23 = AreaAddLinkBetweenAreas(area_2, area_3, false);
        link_23->directCapacities.resize(8, 1);

        // Add thermal clusters
        thCluster_11 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-11");
        thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
        thCluster_31 = addClusterToArea<ThermalCluster>(area_3, "th-cluster-31");

        // Thermal clusters : set the nb of ready made TS
        thCluster_11->series.timeSeries.resize(14, 1);
        thCluster_12->series.timeSeries.resize(14, 1);
        thCluster_31->series.timeSeries.resize(14, 1);

        // Add renewable clusters
        rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
        rnCluster_31 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-31");
        rnCluster_32 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-32");

        // Renewable clusters : set the nb of ready made TS
        rnCluster_21->series.timeSeries.resize(9, 1);
        rnCluster_31->series.timeSeries.resize(9, 1);
        rnCluster_32->series.timeSeries.resize(9, 1);

        // Resize all TS numbers storage (1 column x nbYears lines)
        area_1->resizeAllTimeseriesNumbers(study->parameters.nbYears);
        area_2->resizeAllTimeseriesNumbers(study->parameters.nbYears);
        area_3->resizeAllTimeseriesNumbers(study->parameters.nbYears);

        auto bc = study->bindingConstraints.add("BC_1");
        bc->group("groupTest");
        auto group = study->bindingConstraintsGroups.add("groupTest");
        group->add(bc);
        study->bindingConstraintsGroups.resizeAllTimeseriesNumbers(study->parameters.nbYears);
        bc->RHSTimeSeries().resize(7, 1);

        auto add1 = std::make_shared<ShortTermStorage::AdditionalConstraints>(
          "add1",
          "add1",
          "st-cluster-1",
          "withdrawal",
          "less",
          true,
          std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>{});

        stCluster1.id = "st-cluster-1";
        stCluster1.series->inflows.resize(12, 12);
        add1->rhs().resize(12, 12);
        stCluster1.additionalConstraints.push_back(add1);
        area_1->shortTermStorage.storagesByIndex.push_back(stCluster1);

        auto add2 = std::make_shared<ShortTermStorage::AdditionalConstraints>(
          "add2",
          "add2",
          "st-cluster-2",
          "withdrawal",
          "less",
          true,
          std::vector<Antares::Data::ShortTermStorage::SingleAdditionalConstraint>{});

        stCluster2.id = "st-cluster-2";
        stCluster2.series->inflows.resize(12, 12);
        add2->rhs().resize(12, 12);
        stCluster2.additionalConstraints.push_back(add2);
        area_2->shortTermStorage.storagesByIndex.push_back(stCluster2);

        // Prepare time series numbers storage
        area_1->shortTermStorage.resizeTimeseriesNumbers(10);
        area_2->shortTermStorage.resizeTimeseriesNumbers(11);
        area_3->shortTermStorage.resizeTimeseriesNumbers(12);

        BOOST_CHECK(my_rule.reset());
    }

    ~Fixture() = default;

    Study::Ptr study = std::make_shared<Study>();
    Area* area_1;
    Area* area_2;
    Area* area_3;
    AreaLink* link_12;
    AreaLink* link_13;
    AreaLink* link_23;
    std::shared_ptr<ThermalCluster> thCluster_11;
    std::shared_ptr<ThermalCluster> thCluster_12;
    std::shared_ptr<ThermalCluster> thCluster_31;
    std::shared_ptr<RenewableCluster> rnCluster_21;
    std::shared_ptr<RenewableCluster> rnCluster_31;
    std::shared_ptr<RenewableCluster> rnCluster_32;
    // Setup short-term storage for testing
    ShortTermStorage::STStorageCluster stCluster1;
    ShortTermStorage::STStorageCluster stCluster2;

    ScenarioBuilder::Rules my_rule;
};

// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(s)

namespace
{
struct AreaTsNumberCase
{
    std::string name;
    std::string key; // "l", "w", "s", "h"
    int area;        // 1, 2 or 3
    std::string year;
    std::string tsNumber;
    // value stored in the rule after readLine
    std::function<unsigned(Fixture&, Area*, unsigned)> valueInRule;
    // value stored in the study after apply
    std::function<unsigned(Area*, unsigned)> valueInStudy;
};

std::ostream& operator<<(std::ostream& os, const AreaTsNumberCase& c)
{
    return os << c.name;
}
} // namespace

BOOST_DATA_TEST_CASE_F(
  Fixture,
  ts_number_is_read_and_applied_per_area,
  boost::unit_test::data::make<AreaTsNumberCase>({
    {"load_area2_year18",
     "l",
     2,
     "18",
     "11",
     [](Fixture& f, Area* a, unsigned y) { return f.my_rule.load.get_value(y, a->index); },
     [](Area* a, unsigned y) { return a->load.series.timeseriesNumbers[y]; }},
    {"wind_area3_year7",
     "w",
     3,
     "7",
     "5",
     [](Fixture& f, Area* a, unsigned y) { return f.my_rule.wind.get_value(y, a->index); },
     [](Area* a, unsigned y) { return a->wind.series.timeseriesNumbers[y]; }},
    {"solar_area1_year4",
     "s",
     1,
     "4",
     "8",
     [](Fixture& f, Area* a, unsigned y) { return f.my_rule.solar.get_value(y, a->index); },
     [](Area* a, unsigned y) { return a->solar.series.timeseriesNumbers[y]; }},
    {"hydro_area2_year15",
     "h",
     2,
     "15",
     "3",
     [](Fixture& f, Area* a, unsigned y) { return f.my_rule.hydro.get_value(y, a->index); },
     [](Area* a, unsigned y) { return a->hydro.series->timeseriesNumbers[y]; }},
  }),
  c)
{
    Area* area = c.area == 1 ? area_1 : (c.area == 2 ? area_2 : area_3);
    const auto year = static_cast<unsigned>(std::stoul(c.year));
    const auto tsNumber = static_cast<unsigned>(std::stoul(c.tsNumber));
    std::vector<std::string> splitKey = {c.key, "area " + std::to_string(c.area), c.year};
    BOOST_CHECK(my_rule.readLine(splitKey, c.tsNumber));
    BOOST_CHECK_EQUAL(c.valueInRule(*this, area, year), tsNumber);

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(c.valueInStudy(area, year), tsNumber - 1);
}

// ===========================
// Tests on Thermal clusters
// ===========================
BOOST_FIXTURE_TEST_CASE(
  on_th_cluster11_of_area1_and_on_year_6__solar_TS_number_3_is_chosen__reading_OK,
  Fixture)
{
    AreaName yearNumber = "6";
    std::string tsNumber = "3";
    std::vector<std::string> splitKey = {"t", "area 1", yearNumber, "th-cluster-11"};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.thermal[area_1->index].get(thCluster_11.get(),
                                                         std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(thCluster_11->series.timeseriesNumbers[std::stoul(yearNumber)],
                      std::stoul(tsNumber) - 1);
}

BOOST_FIXTURE_TEST_CASE(
  on_th_cluster12_of_area1_and_on_year_13__solar_TS_number_5_is_chosen__reading_OK,
  Fixture)
{
    AreaName yearNumber = "13";
    std::string tsNumber = "5";
    std::vector<std::string> splitKey = {"t", "area 1", yearNumber, "th-cluster-12"};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.thermal[area_1->index].get(thCluster_12.get(),
                                                         std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(thCluster_12->series.timeseriesNumbers[std::stoul(yearNumber)],
                      std::stoul(tsNumber) - 1);
}

BOOST_FIXTURE_TEST_CASE(
  on_th_cluster31_of_area3_and_on_year_10__solar_TS_number_7_is_chosen__reading_OK,
  Fixture)
{
    AreaName yearNumber = "10";
    std::string tsNumber = "7";
    std::vector<std::string> splitKey = {"t", "area 3", yearNumber, "th-cluster-31"};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.thermal[area_3->index].get(thCluster_31.get(),
                                                         std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(thCluster_31->series.timeseriesNumbers[std::stoul(yearNumber)],
                      std::stoul(tsNumber) - 1);
}

// =============================
// Tests on Renewable clusters
// =============================
BOOST_FIXTURE_TEST_CASE(
  on_rn_cluster21_of_area2_and_on_year_16__solar_TS_number_8_is_chosen__reading_OK,
  Fixture)
{
    study->parameters.renewableGeneration.toClusters();

    AreaName yearNumber = "16";
    std::string tsNumber = "8";
    std::vector<std::string> splitKey = {"r", "area 2", yearNumber, "rn-cluster-21"};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.renewable[area_2->index].get(rnCluster_21.get(),
                                                           std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(rnCluster_21->series.timeseriesNumbers[std::stoul(yearNumber)],
                      std::stoul(tsNumber) - 1);
}

BOOST_FIXTURE_TEST_CASE(
  on_rn_cluster32_of_area3_and_on_year_2__solar_TS_number_4_is_chosen__reading_OK,
  Fixture)
{
    study->parameters.renewableGeneration.toClusters();

    AreaName yearNumber = "2";
    std::string tsNumber = "4";
    std::vector<std::string> splitKey = {"r", "area 3", yearNumber, "rn-cluster-32"};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.renewable[area_3->index].get(rnCluster_32.get(),
                                                           std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(rnCluster_32->series.timeseriesNumbers[std::stoul(yearNumber)],
                      std::stoul(tsNumber) - 1);
}

// ========================
// Tests on Hydro initial levels
// ========================
namespace
{
struct HydroLevelCase
{
    std::string name;
    std::string key; // "hl": initial levels, "hfl": final levels
    int area;
    std::string year;
    std::string level;
    double expected; // level, clamped to [0, 1]
};

std::ostream& operator<<(std::ostream& os, const HydroLevelCase& c)
{
    return os << c.name;
}
} // namespace

BOOST_DATA_TEST_CASE_F(Fixture,
                       hydro_level_is_read_and_clamped_to_0_1,
                       boost::unit_test::data::make<HydroLevelCase>({
                         {"initial_in_range", "hl", 1, "17", "0.123", 0.123},
                         {"initial_lowered_to_1", "hl", 2, "9", "1.5", 1.},
                         {"initial_raised_to_0", "hl", 3, "5", "-3.5", 0.},
                         {"final_in_range", "hfl", 1, "8", "0.342", 0.342},
                         {"final_lowered_to_1", "hfl", 2, "1", "2.4", 1.},
                         {"final_raised_to_0", "hfl", 3, "3", "-5.2", 0.},
                       }),
                       c)
{
    Area* area = c.area == 1 ? area_1 : (c.area == 2 ? area_2 : area_3);
    const auto year = static_cast<unsigned>(std::stoul(c.year));
    const bool initial = c.key == "hl";
    std::vector<std::string> splitKey = {c.key, "area " + std::to_string(c.area), c.year};
    BOOST_CHECK(my_rule.readLine(splitKey, c.level));

    const double inRule = initial ? my_rule.hydroInitialLevels.get_value(year, area->index)
                                  : my_rule.hydroFinalLevels.get_value(year, area->index);
    BOOST_CHECK_EQUAL(inRule, c.expected);

    BOOST_CHECK(my_rule.apply());
    const auto& inStudy = initial ? study->scenarioInitialHydroLevels
                                  : study->scenarioFinalHydroLevels;
    BOOST_CHECK_EQUAL(inStudy[area->index][year], c.expected);
}

// ========================
// Tests on Hydro final levels
// ========================
// ======================
// Tests on Links NTC
// ======================
BOOST_FIXTURE_TEST_CASE(on_link_area1_area2_and_on_year_0__ntc_TS_number_10_is_chosen__reading_OK,
                        Fixture)
{
    AreaName yearNumber = "0";
    std::string tsNumber = "10";
    std::vector<std::string> splitKey = {"ntc", "area 1", "area 2", yearNumber};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.linksNTC[area_1->index].get(link_12, std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(link_12->timeseriesNumbers[std::stoul(yearNumber)], std::stoul(tsNumber) - 1);
}

BOOST_FIXTURE_TEST_CASE(on_link_area1_area3_and_on_year_15__ntc_TS_number_7_is_chosen__reading_OK,
                        Fixture)
{
    AreaName yearNumber = "15";
    std::string tsNumber = "7";
    std::vector<std::string> splitKey = {"ntc", "area 1", "area 3", yearNumber};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.linksNTC[area_1->index].get(link_13, std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(link_13->timeseriesNumbers[std::stoul(yearNumber)], std::stoul(tsNumber) - 1);
}

BOOST_FIXTURE_TEST_CASE(on_link_area2_area3_and_on_year_19__ntc_TS_number_6_is_chosen__reading_OK,
                        Fixture)
{
    AreaName yearNumber = "19";
    std::string tsNumber = "6";
    std::vector<std::string> splitKey = {"ntc", "area 2", "area 3", yearNumber};
    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(my_rule.linksNTC[area_2->index].get(link_23, std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
    BOOST_CHECK_EQUAL(link_23->timeseriesNumbers[std::stoul(yearNumber)], std::stoul(tsNumber) - 1);
}

// ========================
// Tests on Binding Constraints
// ========================
BOOST_FIXTURE_TEST_CASE(binding_constraints_group_groupTest__Load_TS_4_for_year_3__reading_OK,
                        Fixture)
{
    auto yearNumber = 3;
    uint32_t tsNumber = 4;

    std::vector<std::string> splitKey = {"bc", "groupTest", std::to_string(yearNumber)};
    BOOST_CHECK(my_rule.readLine(splitKey, std::to_string(tsNumber)));
    BOOST_CHECK_EQUAL(my_rule.binding_constraints.get("groupTest", yearNumber), tsNumber);

    BOOST_CHECK(my_rule.apply());
    auto actual = study->bindingConstraintsGroups["groupTest"]->timeseriesNumbers[yearNumber];
    BOOST_CHECK_EQUAL(actual, tsNumber - 1);
}

// ========================
// Tests on TSNumberData
// ========================
BOOST_FIXTURE_TEST_CASE(thermalTSNumberData, Fixture)
{
    ScenarioBuilder::thermalTSNumberData tsdata;
    tsdata.attachArea(area_1);
    tsdata.reset(*study);
    tsdata.setTSnumber(thCluster_12.get(), 2, 22);
    tsdata.setTSnumber(thCluster_12.get(), 5, 32); // out of bounds

    study->parameters.nbTimeSeriesThermal = 1;
    thCluster_12->tsGenBehavior = LocalTSGenerationBehavior::forceNoGen;
    thCluster_12->series.timeSeries.resize(30, 8760);

    tsdata.apply(*study);

    BOOST_CHECK_EQUAL(thCluster_12->series.timeseriesNumbers[2], 21);
    BOOST_CHECK_EQUAL(thCluster_12->series.timeseriesNumbers[5], 0);
}

// ========================
// Tests on Short-Term Storage
// ========================

BOOST_FIXTURE_TEST_CASE(short_term_storage_valid_cluster_and_year__reading_OK, Fixture)
{
    AreaName yearNumber = "5";
    std::string tsNumber = "3";
    std::vector<std::string> splitKey = {"sts", "area 1", yearNumber, "st-cluster-1"};

    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    splitKey = {"sta", "area 1", yearNumber, "st-cluster-1", "add1"};

    BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

    BOOST_CHECK_EQUAL(
      my_rule.shortTermStorageInflows[0].get(&area_1->shortTermStorage.storagesByIndex.back(),
                                             std::stoul(yearNumber)),
      std::stoul(tsNumber));

    auto* addc = area_1->shortTermStorage.storagesByIndex.back().additionalConstraints[0].get();
    BOOST_CHECK_EQUAL(my_rule.shortTermStorageAdditionalConstraints[0].get(addc,
                                                                           std::stoul(yearNumber)),
                      std::stoul(tsNumber));

    BOOST_CHECK(my_rule.apply());
}

BOOST_FIXTURE_TEST_CASE(short_term_storage_nonexistent_cluster, Fixture)
{
    AreaName yearNumber = "3";
    std::string tsNumber = "2";
    std::vector<std::string> splitKey = {"sts", "area 2", yearNumber, "nonexistent-cluster"};
    BOOST_CHECK(!my_rule.readLine(splitKey, tsNumber));
}

BOOST_FIXTURE_TEST_CASE(short_term_storage_nonexistent_area, Fixture)
{
    AreaName yearNumber = "7";
    std::string tsNumber = "1";
    std::vector<std::string> splitKey = {"sts", "nonexistent area", yearNumber, "any-cluster"};
    BOOST_CHECK(!my_rule.readLine(splitKey, tsNumber));
}

BOOST_FIXTURE_TEST_CASE(short_term_storage_large_ts_number__handled_gracefully, Fixture)
{
    // Add a short-term storage cluster to area 3

    AreaName yearNumber = "10";
    std::string veryLarge = "100000000"; // take maxTSnumber := 10'000
    std::vector<std::string> splitKey = {"sts", "area 1", yearNumber, "st-cluster-1"};

    BOOST_CHECK(my_rule.readLine(splitKey, veryLarge));
}

BOOST_AUTO_TEST_SUITE_END()
