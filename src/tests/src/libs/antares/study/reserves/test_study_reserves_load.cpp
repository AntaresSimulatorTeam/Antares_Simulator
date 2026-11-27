// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE study
#include <algorithm>
#include <files-system.h>
#include <filesystem>
#include <unit_test_utils.h>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <antares/logs/logs.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/solver/simulation/simulation.h>
#include "antares/study/study.h"

using Antares::UnitTests::CaptureAntaresLogs;
using namespace Antares::Data;

void addThermalCluster(Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

void addShortTermStorage(Area* area, const std::string& name)
{
    ShortTermStorage::STStorageCluster cluster;
    cluster.properties.name = name;
    cluster.id = name;
    area->shortTermStorage.storagesByIndex.push_back(cluster);
}

// Writes a reserve participation yml file.
void writeFile(const std::filesystem::path& path, const std::string& content)
{
    std::ofstream file(path);
    file << content;
}

// Reserve participations loaded for a thermal cluster / for the hydro of an area.
auto& thermalReserves(Area* area, const std::string& cluster)
{
    return area->thermal.list.findInAll(cluster)->reserveParticipationContainer.value();
}

auto& hydroReserves(Area* area)
{
    return area->hydro.reserveParticipationContainer.value();
}

/*!
 * Study with one area named "A"
 */
class OneProblemWithoutReservesOneArea
{
public:
    OneProblemWithoutReservesOneArea()
    {
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
    }

    std::unique_ptr<Study> study;
    Area* areaA;
};

class OneProblemWithoutReservesOneAreaWithLogger: public OneProblemWithoutReservesOneArea,
                                                  public CaptureAntaresLogs
{
};

/*!
 * Study with one area named "A" and reserves
 */

static CapacityReservation makeReservation(TimeSeriesNumbers& tsnum,
                                           ReserveType type,
                                           double unsuppliedCost,
                                           int referenceActivationDuration,
                                           double powerActivationRatio,
                                           double energyActivationRatio)
{
    CapacityReservation res(tsnum);
    res.type = type;
    res.unsuppliedCost = unsuppliedCost;
    res.referenceActivationDuration = referenceActivationDuration;
    res.powerActivationRatio = powerActivationRatio;
    res.energyActivationRatio = energyActivationRatio;
    return res;
}

class OneProblemWithReservesOneArea
{
public:
    OneProblemWithReservesOneArea():
        tsnum()
    {
        tsnum.reset(1);
        tsnum[0] = 0;
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;

        areaA->allCapacityReservations.emplace();
        auto& reservations = areaA->allCapacityReservations->areaCapacityReservations;
        reservations.emplace("reserveup", makeReservation(tsnum, ReserveType::UP, 1, 2, 3, 4));
        reservations.emplace("reserveuptwo",
                             makeReservation(tsnum, ReserveType::UP, 11, 12, 13, 14));
        reservations.emplace("reserveupthree",
                             makeReservation(tsnum, ReserveType::UP, 21, 22, 23, 24));
        reservations.emplace("reservedown", makeReservation(tsnum, ReserveType::DOWN, 5, 6, 7, 8));
        reservations.emplace("reservedowntwo",
                             makeReservation(tsnum, ReserveType::DOWN, 15, 16, 17, 18));
    }

    std::unique_ptr<Study> study;
    Area* areaA;
    TimeSeriesNumbers tsnum;
};

class OneProblemWithReservesOneAreaWithLogger: public OneProblemWithReservesOneArea,
                                               public CaptureAntaresLogs
{
};

/*!
 * Study with two areas named "A" and "B"
 */
struct OneProblemWithReservesTwoAreas
{
    OneProblemWithReservesTwoAreas()
    {
        TimeSeriesNumbers tsnum;
        problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        areaB = addAreaToListOfAreas(study->areas, "B");
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->parameters.include.reserves = true;

        auto tmpCapacityReservationUp = makeReservation(tsnum, ReserveType::UP, 1, 2, 3, 4);
        auto tmpCapacityReservationDown = makeReservation(tsnum, ReserveType::DOWN, 5, 6, 7, 8);
        auto tmpCapacityReservationUpB = makeReservation(tsnum, ReserveType::UP, 11, 12, 13, 14);
        auto tmpCapacityReservationDownB = makeReservation(tsnum,
                                                           ReserveType::DOWN,
                                                           15,
                                                           16,
                                                           17,
                                                           18);

        areaA->allCapacityReservations.emplace();
        areaA->allCapacityReservations->areaCapacityReservations.emplace("reserveup",
                                                                         tmpCapacityReservationUp);
        areaA->allCapacityReservations->areaCapacityReservations
          .emplace("reservedown", tmpCapacityReservationDown);

        areaB->allCapacityReservations.emplace();
        areaB->allCapacityReservations->areaCapacityReservations.emplace("reserveup",
                                                                         tmpCapacityReservationUpB);
        areaB->allCapacityReservations->areaCapacityReservations
          .emplace("reservedown", tmpCapacityReservationDownB);
    }

    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Study> study;
    Area* areaA;
    Area* areaB;
};

BOOST_AUTO_TEST_SUITE(reserves_operations_load)

BOOST_AUTO_TEST_CASE(reserve_add)
{
    auto study = std::make_unique<Study>();
    const auto areaA = addAreaToListOfAreas(study->areas, "A");
    TimeSeriesNumbers tsnum;
    CapacityReservation tmpCapacityReservationUp(tsnum);
    tmpCapacityReservationUp.type = ReserveType::UP;
    tmpCapacityReservationUp.unsuppliedCost = 0;
    areaA->allCapacityReservations.emplace();
    areaA->allCapacityReservations->areaCapacityReservations.emplace("ReserveUp",
                                                                     tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 1);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("ReserveUp") != nullptr);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("ReserveNULL"), nullptr);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->contains("ReserveUp"), true);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("ReserveUp").unsuppliedCost,
      0);
}

BOOST_AUTO_TEST_CASE(reserve_add_double)
{
    auto study = std::make_unique<Study>();
    const auto areaA = addAreaToListOfAreas(study->areas, "A");
    TimeSeriesNumbers tsnum;
    CapacityReservation tmpCapacityReservation(tsnum);

    CapacityReservation tmpCapacityReservationTwo(tsnum);
    areaA->allCapacityReservations.emplace();
    areaA->allCapacityReservations->areaCapacityReservations.emplace("Reserve",
                                                                     tmpCapacityReservation);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->contains("Reserve"), true);
    areaA->allCapacityReservations->areaCapacityReservations.emplace("Reserve",
                                                                     tmpCapacityReservationTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 1);
    areaA->allCapacityReservations->areaCapacityReservations.emplace("ReserveTwo",
                                                                     tmpCapacityReservationTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 2);
}

BOOST_FIXTURE_TEST_CASE(reserve_one_area, OneProblemWithReservesOneArea)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 5);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").unsuppliedCost,
      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reserveup")
                        .referenceActivationDuration,
                      2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").powerActivationRatio,
      3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reserveup")
                        .energyActivationRatio,
                      4);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reservedown").unsuppliedCost,
      5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reservedown")
                        .referenceActivationDuration,
                      6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reservedown")
                        .powerActivationRatio,
                      7);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reservedown")
                        .energyActivationRatio,
                      8);
}

BOOST_FIXTURE_TEST_CASE(reserve_up_two_areas, OneProblemWithReservesTwoAreas)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 2);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations->size(), 2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").unsuppliedCost,
      1);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reservedown").unsuppliedCost,
      5);

    BOOST_CHECK_EQUAL(
      areaB->allCapacityReservations->areaCapacityReservations.at("reserveup").unsuppliedCost,
      11);
    BOOST_CHECK_EQUAL(
      areaB->allCapacityReservations->areaCapacityReservations.at("reservedown").unsuppliedCost,
      15);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_No_Symmetries,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(thermalReserves(areaA, "cluster1").isParticipatingInReserve("reserveup"));
    BOOST_CHECK(!thermalReserves(areaA, "cluster1").isParticipatingInReserve("reservedown"));
    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").reserveMaxPower("reserveup"), 9.9);
    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").reserveCost("reserveup"), 8.8);
    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").reserveMaxPowerOff("reserveup"), 7.7);
    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").reserveCostOff("reserveup"), 6.6);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_Bad_Parameter,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        bad: 9.9
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("invalid thermal reserve property bad"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveUpTwo
        max-power: 19.9
        participation-cost: 18.8
        max-power-off: 17.7
        participation-cost-off: 16.6
      - reserve: ReserveUpThree
        max-power: 19.9
        participation-cost: 18.8
        max-power-off: 17.7
        participation-cost-off: 16.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
      - reserve: ReserveDownTwo
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
      - reserves: [ReserveUpThree, ReserveDown]
      - reserves: [ReserveUpTwo, ReserveDownTwo]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(thermalReserves(areaA, "cluster1").isParticipatingInReserve("reserveup"));
    BOOST_CHECK(thermalReserves(areaA, "cluster1").isParticipatingInReserve("reservedown"));

    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").getNbSymGroups(), 3);

    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").symmetricalIndices("reserveup").size(), 1);

    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").symmetricalIndices("reservedown").size(),
                      2);

    BOOST_CHECK_EQUAL(
      thermalReserves(areaA, "cluster1").symmetricalIndices("reserveupthree").size(),
      1);

    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").symmetricalIndices("reserveuptwo").size(),
                      1);

    BOOST_CHECK_EQUAL(
      thermalReserves(areaA, "cluster1").symmetricalIndices("reservedowntwo").size(),
      1);

    std::vector<int> symDown = thermalReserves(areaA, "cluster1").symmetricalIndices("reservedown");

    int symUp = thermalReserves(areaA, "cluster1").symmetricalIndices("reserveup").at(0);
    int symUpTwo = thermalReserves(areaA, "cluster1").symmetricalIndices("reserveuptwo").at(0);
    int symUpThree = thermalReserves(areaA, "cluster1").symmetricalIndices("reserveupthree").at(0);
    BOOST_CHECK_EQUAL(
      count(symDown.begin(), symDown.end(), symUp),
      1); // Value in symmetricalIndices("ReserveUp") is also in symmetricalIndices("ReserveDown")
    BOOST_CHECK_EQUAL(
      count(symDown.begin(), symDown.end(), symUpTwo),
      0); // Value in symmetricalIndices("ReserveUpTwo") is not in symmetricalIndices("ReserveDown")
    BOOST_CHECK_EQUAL(count(symDown.begin(), symDown.end(), symUpThree),
                      1);             // Value in symmetricalIndices("ReserveUpThree") is also in
                                      // symmetricalIndices("ReserveDown")
    BOOST_CHECK(symUp != symUpThree); // Value in symmetricalIndices("ReserveUp") is not in
                                      // symmetricalIndices("ReserveUpThree")

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

namespace
{
struct ThermalReserveErrorCase
{
    std::string name;
    std::vector<std::string> clusters;
    std::string yaml;
    std::string expectedError;
};

std::ostream& operator<<(std::ostream& os, const ThermalReserveErrorCase& c)
{
    return os << c.name;
}

const std::string certificationUp = R"(      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
)";

std::string certification(const std::string& reserve)
{
    return "      - reserve: " + reserve + R"(
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
)";
}
} // namespace

BOOST_DATA_TEST_CASE_F(
  OneProblemWithReservesOneAreaWithLogger,
  test_thermal_loadReserveParticipations_errors,
  boost::unit_test::data::make<ThermalReserveErrorCase>({
    {"symmetry_on_unknown_cluster",
     {"cluster1"},
     "participations:\n  - cluster: cluster1\n    certifications:\n" + certificationUp
       + certification("ReserveDown")
       + "  - cluster: cluster3\n    symmetries:\n      - reserves: [ReserveUp, ReserveDown]\n",
     "Thermal cluster cluster3 does not exist in area A"},
    {"symmetry_on_unknown_reserve",
     {"cluster1"},
     "participations:\n  - cluster: cluster1\n    certifications:\n" + certificationUp
       + certification("ReserveDown")
       + "    symmetries:\n      - reserves: [ReserveNull, ReserveDown]\n",
     "This entity is not participating to reserve reservenull"},
    {"symmetry_on_reserve_the_cluster_does_not_participate_in",
     {"cluster1", "cluster3"},
     "participations:\n  - cluster: cluster1\n    certifications:\n" + certificationUp
       + "    symmetries:\n      - reserves: [ReserveUp, ReserveDown]\n"
         "  - cluster: cluster3\n    certifications:\n"
       + certification("ReserveDown"),
     "This entity is not participating to reserve reservedown"},
    {"participation_in_unknown_reserve",
     {"cluster1"},
     "participations:\n  - cluster: cluster1\n    certifications:\n" + certificationUp
       + certification("ReserveNull"),
     "A : missing reserve reservenull when loading thermal reserve participations"},
  }),
  c)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    for (const auto& cluster: c.clusters)
    {
        addThermalCluster(areaA, cluster);
    }
    writeFile(studyPath / "myreserve.yml", c.yaml);
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(c.expectedError));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Delete_Double_Sym_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
      - reserves: [ReserveUp, ReserveDown]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Detected duplicate in reserves symmetries"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_No_Cluster_Provided,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
  - cluster: ""
    certifications:
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      getErrors().contains("A : cluster/storage name is missing in a participation entry"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Double_Cluster_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      getErrors().contains("A, cluster cluster1 : duplicate participation to reserve reserveup"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Only_One_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("A : symmetry group must have at least two reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Triple_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDownTwo
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown, ReserveDownTwo]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").symmetricalIndices("reserveup").size(), 1);
    BOOST_CHECK_EQUAL(thermalReserves(areaA, "cluster1").symmetricalIndices("reservedown").size(),
                      1);
    BOOST_CHECK_EQUAL(
      thermalReserves(areaA, "cluster1").symmetricalIndices("reservedowntwo").size(),
      1);
    int symUp = thermalReserves(areaA, "cluster1").symmetricalIndices("reserveup").at(0);
    int symDown = thermalReserves(areaA, "cluster1").symmetricalIndices("reservedown").at(0);
    int symDownTwo = thermalReserves(areaA, "cluster1").symmetricalIndices("reservedowntwo").at(0);
    BOOST_CHECK_EQUAL(symUp, symDown);
    BOOST_CHECK_EQUAL(symUp, symDownTwo); // all of them are participating to the same symmetry
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Double_Symmetry_Same_Line,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveUp]
      - reserves: [ReserveUp, ReserveDown]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("A : symmetry group must have at least two reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Cluster_Participation_No_Init,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - cluster: cluster1
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)");
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "Area A, cluster1 : trying to add symmetries without any reserve participation"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_reserveParticipationsCount,
                        OneProblemWithReservesOneAreaWithLogger)
{
    // No reserveParticipationContainer loaded yet: the optional is empty, count is 0
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationsCount(), 0);

    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationsCount(), 2);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_reserveParticipationAt, OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");

    // The fixture registers 5 reserves on areaA (reserveup/reserveuptwo/reserveupthree/
    // reservedown/reservedowntwo), but only ReserveUp and ReserveDown got a hydro
    // participation above. reserveParticipationAt walks allCapacityReservations in
    // (sorted) key order and returns only the participating ones, so the two
    // participating reserves surface at index 0 and 1 in alphabetical order.
    auto at0 = areaA->hydro.reserveParticipationAt(areaA, 0);
    BOOST_REQUIRE(at0.has_value());
    BOOST_CHECK_EQUAL(*at0, "reservedown");

    auto at1 = areaA->hydro.reserveParticipationAt(areaA, 1);
    BOOST_REQUIRE(at1.has_value());
    BOOST_CHECK_EQUAL(*at1, "reserveup");

    BOOST_CHECK(!areaA->hydro.reserveParticipationAt(areaA, 2).has_value());
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveUpTwo
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveUpThree
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDownTwo
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
    - reserves: [ReserveUpThree, ReserveDown]
    - reserves: [ReserveUpTwo, ReserveDownTwo]
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");

    BOOST_CHECK(areaA->hydro.reserveParticipationContainer.has_value());

    BOOST_CHECK_EQUAL(hydroReserves(areaA).isParticipatingInReserve("reserveup"), true);

    BOOST_CHECK_EQUAL(hydroReserves(areaA).isParticipatingInReserve("reservedown"), true);

    BOOST_CHECK_EQUAL(hydroReserves(areaA).getNbSymGroups(), 3);

    BOOST_CHECK_EQUAL(hydroReserves(areaA).reserveCost("reserveup"), 9.9);
    BOOST_CHECK_EQUAL(hydroReserves(areaA).reserveMaxStore("reserveup"), 8.8);
    BOOST_CHECK_EQUAL(hydroReserves(areaA).reserveMaxRelease("reserveup"), 7.7);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_no_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    writeFile(studyPath / "myreserve.yml", R"(participations:
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "Area A, hydro : trying to add symmetries without any reserve participation"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_missing_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: reservenull
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "A : missing reserve reservenull when loading hydro reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_cluster,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_property,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: ReserveUp
      bad: 9.9
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("invalid hydro reserve property bad"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    writeFile(studyPath / "myreserve.yml", R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveUpTwo
      cluster: cluster1
      max-power: 19.9
      participation-cost: 18.8
      max-power-off: 17.7
      participation-cost-off: 16.6
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
)");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("This entity is not participating to reserve reservedown"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    addShortTermStorage(areaA, "cluster1");
    writeFile(studyPath / "myreserve.yml", R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
        participation-cost: 9.9
        max-store: 8.8
        max-release: 7.7
      - reserve: ReserveUpTwo
        max-power: 19.9
        max-power-off: 17.7
        participation-cost-off: 16.6
      - reserve: ReserveUpThree
      - reserve: ReserveDown
      - reserve: ReserveDownTwo
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
      - reserves: [ReserveUpThree, ReserveDown]
      - reserves: [ReserveUpTwo, ReserveDownTwo]
)");

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    auto* resContainer = &areaA->shortTermStorage.findInAll("cluster1")
                            ->reserveParticipationContainer;

    BOOST_CHECK(resContainer->has_value());
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("reserveup"), true);
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("reservedown"), true);
    BOOST_CHECK_EQUAL(resContainer->value().getNbSymGroups(), 3);
    BOOST_CHECK_EQUAL(resContainer->value().reserveCost("reserveup"), 9.9);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxStore("reserveup"), 8.8);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxRelease("reserveup"), 7.7);
    BOOST_CHECK_EQUAL(getErrors().size(), 3);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("invalid STS reserve property max-power"));
    BOOST_CHECK(getErrors().contains("invalid STS reserve property max-power-off"));
    BOOST_CHECK(getErrors().contains("invalid STS reserve property participation-cost-off"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_cluster_symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    writeFile(studyPath / "myreserve.yml", R"(participations:
  - storage: cluster1
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)");

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Short term storage cluster1 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_no_reserves,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - storage: cluster1
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)");
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "Area A, cluster1 : trying to add symmetries without any reserve participation"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
      - reserve: ReserveDownTwo
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)");
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("This entity is not participating to reserve reservedown"));
}

BOOST_FIXTURE_TEST_CASE(test_sts_loadReserveParticipations_No_Cluster_Provided,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
  - certifications:
      - reserve: ReserveDown
)");
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Short term storage cluster1 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_sts_loadReserveParticipations_Invalid_Cluster,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    writeFile(studyPath / "myreserve.yml", R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
  - storage: cluster4
    certifications:
      - reserve: ReserveDown
)");
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Short term storage cluster4 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok_file_missing_needs,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
)");
    BOOST_CHECK_EXCEPTION(Antares::Data::loadReservesParameters(studyPath, *areaA),
                          std::runtime_error,
                          containsMessage(
                            "Failed to load need from CSV file: "
                            + (studyPath / "reserves" / "a" / "reserveup.txt").string()));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok_minimal, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
)");

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "reserveup.txt");
    for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        fileNeeds << "1\n";
    }
    fileNeeds.close();
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.has_value(), false);
    Antares::Data::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.has_value(), true);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.up, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.down, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.up, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.down, 1);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup")->type
                == ReserveType::DOWN);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->energyActivationRatio,
      1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->powerActivationRatio,
      0);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->referenceActivationDuration,
      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->spillageCost, 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->unsuppliedCost,
                      0);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->TSNumbers.height(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_ini, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
    a: [

global-parameters:
  a: [
)");

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeeds << "\n";
    fileNeeds.close();
    Antares::Data::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(std::any_of(getErrors().begin(),
                            getErrors().end(),
                            [](const std::string& msg)
                            { return msg.find("Invalid reserves config") != std::string::npos; }));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_parameters, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
    a: a

global-parameters:
  b: 2.1
)");

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "reserveup.txt");
    for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        fileNeeds << "1\n";
    }
    fileNeeds.close();
    Antares::Data::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 2);
    BOOST_CHECK(
      getWarnings().contains("A : invalid key a inside reserve parameters for ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A : invalid key b inside global reserve parameters"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(global-parameters:
  energy-activation-ratio-up: 6.6
  energy-activation-ratio-down: 7.7
  reference-activation-duration-up: 8
  reference-activation-duration-down: 9

reserves:
  - name: ReserveUp
    type: up
    reference-activation-duration: 2
    energy-activation-ratio: 2.2
    power-activation-ratio: 3.3
    spillage-cost: 4.4
    failure-cost: 5.5
  - name: ReserveDown
    type: down
)");

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp << "2\n3\n";
    for (unsigned int i = 2; i < HOURS_PER_YEAR; ++i)
    {
        fileNeedsUp << "0\n";
    }
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "reservedown.txt");
    fileNeedsDown << "4\n5\n6\n";
    for (unsigned int i = 3; i < HOURS_PER_YEAR; ++i)
    {
        fileNeedsDown << "0\n";
    }
    fileNeedsDown.close();
    Antares::Data::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.up, 8);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.down, 9);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.up, 6.6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.down, 7.7);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reservedown") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup")->type
                == ReserveType::UP);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reservedown")->type
                == ReserveType::DOWN);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->energyActivationRatio,
      2.2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->powerActivationRatio,
      3.3);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->referenceActivationDuration,
      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->spillageCost,
                      4.4);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->unsuppliedCost,
                      5.5);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->need.timeSeries.height,
      HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->need.timeSeries[0][0],
      2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->need.timeSeries[0][1],
      3);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reservedown")->need.timeSeries[0][0],
      4);
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_negative_parameters_values,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
    type: up
    reference-activation-duration: -1
    energy-activation-ratio: -2
    power-activation-ratio: -3
    spillage-cost: 10
    failure-cost: 10

global-parameters:
  energy-activation-ratio-up: -1
  energy-activation-ratio-down: -1
  reference-activation-duration-up: -2
  reference-activation-duration-down: -2
)");

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "reservedown.txt");
    fileNeedsDown.close();
    Antares::Data::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 7);
    BOOST_CHECK(
      getErrors().contains("A : invalid maxGlobalEnergyActivationRatio down can not be negative"));
    BOOST_CHECK(
      getErrors().contains("A : invalid maxGlobalEnergyActivationRatio up can not be negative"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid referenceGlobalActivationDuration down can not be negative"));
    BOOST_CHECK(
      getErrors().contains("A : invalid referenceGlobalActivationDuration up can not be negative"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid energyActivationRatio can not be negative, for reserve reserveup"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid powerActivationRatio can not be negative, for reserve reserveup"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid referenceActivationDuration can not be negative, for reserve reserveup"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_parameters_values,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
    type: aa
    reference-activation-duration: 1.1
    energy-activation-ratio: a
    power-activation-ratio: a
    spillage-cost: a
    failure-cost: a

global-parameters:
  energy-activation-ratio-up: a
  energy-activation-ratio-down: a
  reference-activation-duration-up: 1.1
  reference-activation-duration-down: 1.1
)");

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "reservedown.txt");
    fileNeedsDown.close();
    Antares::Data::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 10);
    BOOST_CHECK(getWarnings().contains("A : invalid type for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid reference activation duration for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid energy activation ratio for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid maximum activation ratio for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A : invalid spillage cost for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A : invalid failure cost for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid maximum energy activation ratio for UP reserves"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid maximum energy activation ratio for DOWN reserves"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid reference energy activation duration for UP reserves"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid reference energy activation duration for DOWN reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_duplicated, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    writeFile(studyPath / "reserves" / "a" / "reserves.yml", R"(reserves:
  - name: ReserveUp
  - name: ReserveUp
global-parameters:
)");

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp.close();
    Antares::Data::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("A : reserve name already exists for reserve ReserveUp"));
}

BOOST_FIXTURE_TEST_CASE(test_negative_value, CaptureAntaresLogs)
{
    errorIfNegativeValue("string1", 2, "area", "cluster", "reserve");

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);

    errorIfNegativeValue("parameter", -2, "area", "cluster", "reserve");

    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains(
      "area : invalid parameter can not be negative, for cluster cluster, for reserve reserve"));
}

BOOST_FIXTURE_TEST_CASE(test_validateCapacityReservations_noNegative,
                        OneProblemWithReservesOneAreaWithLogger)
{
    Antares::Data::validateCapacityReservations(*areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_validateCapacityReservations_OneNegative,
                        OneProblemWithReservesOneAreaWithLogger)
{
    areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").powerActivationRatio
      = -1;
    Antares::Data::validateCapacityReservations(*areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains(
      "A : invalid powerActivationRatio can not be negative, for reserve reserveup"));
}

BOOST_AUTO_TEST_SUITE_END() // version
