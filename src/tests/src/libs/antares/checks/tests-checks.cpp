#define BOOST_TEST_MODULE test check input data

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>
#include "antares/checks/checkLoadedInputData.h"
#include "antares/study/area/area.h"
#include "antares/study/version.h"

using namespace Antares::Data;
using namespace Antares::Check;

namespace Antares::Check
{
// Not declared in checkLoadedInputData.h (only used internally by checkMinStablePower), but has
// external linkage: forward-declare it here to unit-test it directly.
bool areasThermalClustersMinStablePowerValidity(const AreaList& areas,
                                                std::map<int, std::string>& areaClusterNames);
} // namespace Antares::Check

BOOST_AUTO_TEST_SUITE(check_study_version)

BOOST_AUTO_TEST_CASE(study_version_is_unknown___exception_raised)
{
    StudyVersion version;
    const std::string StudyFolder; // Whaterver the value
    std::string err_msg = "does not seem to be a valid study";
    BOOST_CHECK_EXCEPTION(checkStudyVersion(version, StudyFolder),
                          std::runtime_error,
                          containsMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(study_version_is_too_high___exception_raised)
{
    StudyVersion version(9999, 8888);
    const std::string StudyFolder; // Whaterver the value
    std::string err_msg = "Invalid version for the study : found";
    BOOST_CHECK_EXCEPTION(checkStudyVersion(version, StudyFolder),
                          std::runtime_error,
                          containsMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(study_version_is_valid___no_exception_raised)
{
    StudyVersion version = StudyVersion::latest();
    const std::string StudyFolder; // Whatever the value
    BOOST_CHECK_NO_THROW(checkStudyVersion(version, StudyFolder));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_simplex_range_and_hydro_pricing)

BOOST_AUTO_TEST_CASE(splx_optim_range_and_hydro_pricing_are_incompatible___exception_raised)
{
    SimplexOptimization splxOptimRange = sorDay;
    HydroPricingMode hydroPricingMode = hpMILP;
    std::string err_msg = "Simplex optimization range and hydro pricing mode : values are not "
                          "compatible ";
    BOOST_CHECK_EXCEPTION(checkSimplexRangeHydroPricing(splxOptimRange, hydroPricingMode),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(splx_optim_range_and_hydro_pricing_are_compatible___no_exception_raised)
{
    // optRange != sorDay, incompatibility can't happen regardless of hpMode
    BOOST_CHECK_NO_THROW(
      checkSimplexRangeHydroPricing(SimplexOptimization::sorWeek, HydroPricingMode::hpMILP));
    // optRange == sorDay but hpMode != hpMILP
    BOOST_CHECK_NO_THROW(
      checkSimplexRangeHydroPricing(SimplexOptimization::sorDay, HydroPricingMode::hpHeuristic));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_simplex_range_and_unit_commitment)

BOOST_AUTO_TEST_CASE(splx_optim_range_and_unit_commitment_are_incompatible___exception_raised)
{
    SimplexOptimization splxOptimRange = sorDay;
    UnitCommitmentMode ucMode = ucMILP;
    std::string err_msg = "Simplexe optimization range and unit commitment mode : values are not "
                          "compatible";
    BOOST_CHECK_EXCEPTION(checkSimplexRangeUnitCommitmentMode(splxOptimRange, ucMode),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(splx_optim_range_and_unit_commitment_are_compatible___no_exception_raised)
{
    // optRange != sorDay, incompatibility can't happen regardless of ucMode
    BOOST_CHECK_NO_THROW(checkSimplexRangeUnitCommitmentMode(SimplexOptimization::sorWeek,
                                                             UnitCommitmentMode::ucMILP));
    // optRange == sorDay but ucMode != ucMILP
    BOOST_CHECK_NO_THROW(checkSimplexRangeUnitCommitmentMode(SimplexOptimization::sorDay,
                                                             UnitCommitmentMode::ucHeuristicFast));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_simplex_range_and_hydro_heuristic)

BOOST_AUTO_TEST_CASE(optRange_not_daily___no_exception_regardless_of_heuristic)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    area->hydro.useHeuristicTarget = false;

    BOOST_CHECK_NO_THROW(
      checkSimplexRangeHydroHeuristic(SimplexOptimization::sorWeek, study->areas));
}

BOOST_AUTO_TEST_CASE(optRange_daily_and_all_areas_use_heuristic___no_exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    area->hydro.useHeuristicTarget = true;

    BOOST_CHECK_NO_THROW(
      checkSimplexRangeHydroHeuristic(SimplexOptimization::sorDay, study->areas));
}

BOOST_AUTO_TEST_CASE(optRange_daily_and_one_area_does_not_use_heuristic___exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area1 = addAreaToListOfAreas(study->areas, "area1");
    area1->hydro.useHeuristicTarget = true;
    auto area2 = addAreaToListOfAreas(study->areas, "area2");
    area2->hydro.useHeuristicTarget = false;

    BOOST_CHECK_EXCEPTION(checkSimplexRangeHydroHeuristic(SimplexOptimization::sorDay,
                                                          study->areas),
                          std::runtime_error,
                          containsMessage(area2->name));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_min_stable_power)

BOOST_AUTO_TEST_CASE(areasThermalClustersMinStablePowerValidity_no_clusters___valid)
{
    auto study = std::make_shared<Study>();
    addAreaToListOfAreas(study->areas, "area1");

    std::map<int, std::string> areaClusterNames;
    BOOST_CHECK(areasThermalClustersMinStablePowerValidity(study->areas, areaClusterNames));
    BOOST_CHECK(areaClusterNames.empty());
}

namespace
{
// Build a thermal cluster whose checkMinStablePower() will fail:
// minStablePower is set higher than nominalCapacity * (1 - spinning / 100), i.e. higher than
// the nominal capacity itself here (spinning == 0).
std::shared_ptr<ThermalCluster> addInvalidMinStablePowerCluster(Area* area, const std::string& name)
{
    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName(name);
    cluster->modulation.resize(thermalModulationMax, HOURS_PER_YEAR);
    cluster->modulation.fill(1.);
    cluster->nominalCapacity = 10.;
    cluster->spinning = 0.;
    cluster->minStablePower = 20.; // > nominalCapacity => invalid
    area->thermal.list.addToCompleteList(cluster);
    area->thermal.list.buildIndexes();
    return cluster;
}

std::shared_ptr<ThermalCluster> addValidMinStablePowerCluster(Area* area, const std::string& name)
{
    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName(name);
    cluster->modulation.resize(thermalModulationMax, HOURS_PER_YEAR);
    cluster->modulation.fill(1.);
    cluster->nominalCapacity = 10.;
    cluster->spinning = 0.;
    cluster->minStablePower = 0.;
    area->thermal.list.addToCompleteList(cluster);
    area->thermal.list.buildIndexes();
    return cluster;
}
} // anonymous namespace

BOOST_AUTO_TEST_CASE(areasThermalClustersMinStablePowerValidity_invalid_cluster___invalid)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addInvalidMinStablePowerCluster(area, "cluster1");

    std::map<int, std::string> areaClusterNames;
    BOOST_CHECK(!areasThermalClustersMinStablePowerValidity(study->areas, areaClusterNames));
    BOOST_REQUIRE_EQUAL(areaClusterNames.size(), 1u);
    BOOST_CHECK(areaClusterNames.at(0).find("area1") != std::string::npos);
    BOOST_CHECK(areaClusterNames.at(0).find("cluster1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(checkMinStablePower_tsGenThermal_true_and_valid___no_exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addValidMinStablePowerCluster(area, "cluster1");

    BOOST_CHECK_NO_THROW(checkMinStablePower(true, study->areas));
}

BOOST_AUTO_TEST_CASE(checkMinStablePower_tsGenThermal_true_and_invalid___exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addInvalidMinStablePowerCluster(area, "cluster1");

    std::string err_msg = "Conflict between Min Stable Power";
    BOOST_CHECK_EXCEPTION(checkMinStablePower(true, study->areas),
                          std::runtime_error,
                          containsMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(checkMinStablePower_tsGenThermal_false___no_exception_raised)
{
    auto study = std::make_shared<Study>();
    addAreaToListOfAreas(study->areas, "area1");

    // tsGenThermal == false takes the checkAndCorrectAvailability() path instead
    BOOST_CHECK_NO_THROW(checkMinStablePower(false, study->areas));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_thermal_column_number)

namespace
{
std::shared_ptr<ThermalCluster> addColumnNumberCluster(Area* area,
                                                       const std::string& name,
                                                       CostGeneration costgeneration,
                                                       uint tsWidth,
                                                       uint otherMatrixWidth)
{
    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName(name);
    cluster->costgeneration = costgeneration;
    cluster->series.timeSeries.reset(tsWidth, HOURS_PER_YEAR);
    cluster->ecoInput.fuelcost.reset(otherMatrixWidth, DAYS_PER_YEAR);
    cluster->ecoInput.co2cost.reset(otherMatrixWidth, DAYS_PER_YEAR);
    area->thermal.list.addToCompleteList(cluster);
    area->thermal.list.buildIndexes();
    return cluster;
}
} // anonymous namespace

BOOST_AUTO_TEST_CASE(setManually_cluster_is_skipped___no_exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    // Width mismatch, but costgeneration == setManually => cluster is skipped entirely
    addColumnNumberCluster(area, "cluster1", setManually, 2, 5);

    BOOST_CHECK_NO_THROW(checkFuelCostColumnNumber(study->areas));
    BOOST_CHECK_NO_THROW(checkCO2CostColumnNumber(study->areas));
}

BOOST_AUTO_TEST_CASE(matrix_width_of_one___no_exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addColumnNumberCluster(area, "cluster1", useCostTimeseries, 3, 1);

    BOOST_CHECK_NO_THROW(checkFuelCostColumnNumber(study->areas));
    BOOST_CHECK_NO_THROW(checkCO2CostColumnNumber(study->areas));
}

BOOST_AUTO_TEST_CASE(matrix_width_matches_ts_width___no_exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addColumnNumberCluster(area, "cluster1", useCostTimeseries, 3, 3);

    BOOST_CHECK_NO_THROW(checkFuelCostColumnNumber(study->areas));
    BOOST_CHECK_NO_THROW(checkCO2CostColumnNumber(study->areas));
}

BOOST_AUTO_TEST_CASE(fuelcost_matrix_width_mismatch___exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addColumnNumberCluster(area, "cluster1", useCostTimeseries, 3, 2);

    BOOST_CHECK_EXCEPTION(checkFuelCostColumnNumber(study->areas),
                          std::runtime_error,
                          containsMessage(
                            "Number of columns for Fuel Cost can be one or same as number of TS"));
}

BOOST_AUTO_TEST_CASE(co2cost_matrix_width_mismatch___exception_raised)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");
    addColumnNumberCluster(area, "cluster1", useCostTimeseries, 3, 2);

    BOOST_CHECK_EXCEPTION(checkCO2CostColumnNumber(study->areas),
                          std::runtime_error,
                          containsMessage(
                            "Number of columns for CO2 Cost can be one or same as number of TS"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(check_thermal_cluster_exceptions)

BOOST_AUTO_TEST_CASE(IncompatibleDailyOptHeuristicForArea_message_contains_area_name)
{
    IncompatibleDailyOptHeuristicForArea ex(AreaName("myArea"));
    std::string what = ex.what();
    BOOST_CHECK(what.find("myArea") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(InvalidParametersForThermalClusters_message_lists_all_clusters)
{
    std::map<int, std::string> clusterNames{{0, "Area : area1 cluster name : cluster1"},
                                            {1, "Area : area2 cluster name : cluster2"}};
    InvalidParametersForThermalClusters ex(clusterNames);
    std::string what = ex.what();
    BOOST_CHECK(what.find("cluster1") != std::string::npos);
    BOOST_CHECK(what.find("cluster2") != std::string::npos);
    // No trailing semicolon after the last entry
    BOOST_CHECK(what.back() != ';');
}

BOOST_AUTO_TEST_CASE(InvalidParametersForThermalClusters_empty_map___no_trailing_semicolon_issue)
{
    std::map<int, std::string> clusterNames;
    InvalidParametersForThermalClusters ex(clusterNames);
    std::string what = ex.what();
    BOOST_CHECK(what.find("Conflict between Min Stable Power") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
