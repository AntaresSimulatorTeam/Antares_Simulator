#define BOOST_TEST_MODULE test read scenario-builder.dat 
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <study.h>
#include <rules.h>

using namespace std;
using namespace Antares::Data;


// ===========================
// Add a cluster to an area
// ===========================

void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
	area->thermal.clusters.push_back(cluster.get());
	area->thermal.list.add(cluster);
	area->thermal.list.mapping[cluster->id()] = cluster;
}

void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
	area->renewable.list.add(cluster);
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
	Fixture(const Fixture & f) = delete;
	Fixture(const Fixture && f) = delete;
	Fixture & operator= (const Fixture & f) = delete;
	Fixture& operator= (const Fixture && f) = delete;
	Fixture() : my_rule(*study)
	{
		// Set study parameters
		study->parameters.nbYears = 20;
		study->parameters.timeSeriesToGenerate = 0; // No generated time-series, only ready made time-series

		// Add areas
		area_1 = study->areaAdd("Area 1");
		area_2 = study->areaAdd("Area 2");
		area_3 = study->areaAdd("Area 3");
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

		// Solar : set the nb of ready made TS
		nbReadyMadeTS = 9;
		area_1->solar.series.timeSeries.resize(nbReadyMadeTS, 1);
		area_2->solar.series.timeSeries.resize(nbReadyMadeTS, 1);
		area_3->solar.series.timeSeries.resize(nbReadyMadeTS, 1);

		// Hydro : set the nb of ready made TS
		nbReadyMadeTS = 12;
		area_1->hydro.series->resizeGenerationTS(nbReadyMadeTS, 1);
		area_2->hydro.series->resizeGenerationTS(nbReadyMadeTS, 1);
		area_3->hydro.series->resizeGenerationTS(nbReadyMadeTS, 1);
		
		// Hydro Max Power: set the nb of ready made TS
		nbReadyMadeTS = 15;
		area_1->hydro.series->resizeMaxPowerTS(nbReadyMadeTS, 1);
		area_2->hydro.series->resizeMaxPowerTS(nbReadyMadeTS, 1);
		area_3->hydro.series->resizeMaxPowerTS(nbReadyMadeTS, 1);

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

		// Thermal clusters : update areas local numbering for clusters
		area_1->thermal.prepareAreaWideIndexes();
		area_2->thermal.prepareAreaWideIndexes();
		area_3->thermal.prepareAreaWideIndexes();

		// Add renewable clusters
		rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
		rnCluster_31 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-31");
		rnCluster_32 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-32");

		// Renewable clusters : set the nb of ready made TS
		rnCluster_21->series.timeSeries.resize(9, 1);
		rnCluster_31->series.timeSeries.resize(9, 1);
		rnCluster_32->series.timeSeries.resize(9, 1);

		// Renewable clusters : update areas local numbering for clusters
		area_1->renewable.prepareAreaWideIndexes();
		area_2->renewable.prepareAreaWideIndexes();
		area_3->renewable.prepareAreaWideIndexes();

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

	ScenarioBuilder::Rules my_rule;
};


// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(s)

// =================
// Tests on Load
// =================
BOOST_FIXTURE_TEST_CASE(on_area2_and_on_year_18__load_TS_number_11_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "18";
	String tsNumber = "11";
	AreaName::Vector splitKey = { "l", "area 2", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.load.get_value(yearNumber.to<uint>(), area_2->index), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(area_2->load.series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Wind
// =================
BOOST_FIXTURE_TEST_CASE(on_area3_and_on_year_7__wind_TS_number_5_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "7";
	String tsNumber = "5";
	AreaName::Vector splitKey = { "w", "area 3", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.wind.get_value(yearNumber.to<uint>(), area_3->index), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(area_3->wind.series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Solar
// =================
BOOST_FIXTURE_TEST_CASE(on_area1_and_on_year_4__solar_TS_number_8_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "4";
	String tsNumber = "8";
	AreaName::Vector splitKey = { "s", "area 1", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.solar.get_value(yearNumber.to<uint>(), area_1->index), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(area_1->solar.series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Hydro
// =================
BOOST_FIXTURE_TEST_CASE(on_area2_and_on_year_15__solar_TS_number_3_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "15";
	String tsNumber = "3";
	AreaName::Vector splitKey = { "h", "area 2", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.hydro.get_value(yearNumber.to<uint>(), area_2->index), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(area_2->hydro.series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Hydro Max Power
// =================
BOOST_FIXTURE_TEST_CASE(on_area3_and_on_year_10__hydro_power_credits_TS_number_6_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "7";
	String tsNumber = "6";
	AreaName::Vector splitKey = { "hgp", "area 3", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber, false));

	BOOST_CHECK_EQUAL(my_rule.hydroMaxPower.get_value(yearNumber.to<uint>(), area_3->index), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(area_3->hydro.series->timeseriesNumbersHydroMaxPower[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}


// ===========================
// Tests on Thermal clusters
// ===========================
BOOST_FIXTURE_TEST_CASE(on_th_cluster11_of_area1_and_on_year_6__solar_TS_number_3_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "6";
	String tsNumber = "3";
	AreaName::Vector splitKey = { "t", "area 1", yearNumber, "th-cluster-11" };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.thermal[area_1->index].get(thCluster_11.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(thCluster_11->series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_FIXTURE_TEST_CASE(on_th_cluster12_of_area1_and_on_year_13__solar_TS_number_5_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "13";
	String tsNumber = "5";
	AreaName::Vector splitKey = { "t", "area 1", yearNumber, "th-cluster-12" };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.thermal[area_1->index].get(thCluster_12.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(thCluster_12->series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_FIXTURE_TEST_CASE(on_th_cluster31_of_area3_and_on_year_10__solar_TS_number_7_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "10";
	String tsNumber = "7";
	AreaName::Vector splitKey = { "t", "area 3", yearNumber, "th-cluster-31" };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.thermal[area_3->index].get(thCluster_31.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(thCluster_31->series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}


// =============================
// Tests on Renewable clusters
// =============================
BOOST_FIXTURE_TEST_CASE(on_rn_cluster21_of_area2_and_on_year_16__solar_TS_number_8_is_chosen__reading_OK, Fixture)
{
	study->parameters.renewableGeneration.toClusters();

	AreaName yearNumber = "16";
	String tsNumber = "8";
	AreaName::Vector splitKey = { "r", "area 2", yearNumber, "rn-cluster-21" };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.renewable[area_2->index].get(rnCluster_21.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(rnCluster_21->series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_FIXTURE_TEST_CASE(on_rn_cluster32_of_area3_and_on_year_2__solar_TS_number_4_is_chosen__reading_OK, Fixture)
{
	study->parameters.renewableGeneration.toClusters();

	AreaName yearNumber = "2";
	String tsNumber = "4";
	AreaName::Vector splitKey = { "r", "area 3", yearNumber, "rn-cluster-32" };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.renewable[area_3->index].get(rnCluster_32.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(rnCluster_32->series.timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}


// ========================
// Tests on Hydro levels
// ========================
BOOST_FIXTURE_TEST_CASE(on_area1_and_on_year_17__hydro_level_0_123_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "17";
	String level = "0.123";
	AreaName::Vector splitKey = { "hl", "area 1", yearNumber };
	my_rule.readLine(splitKey, level);

	BOOST_CHECK_EQUAL(my_rule.hydroLevels.get_value(yearNumber.to<uint>(), area_1->index), level.to<double>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(study->scenarioHydroLevels[area_1->index][yearNumber.to<uint>()], level.to<double>());
}

BOOST_FIXTURE_TEST_CASE(on_area2_and_on_year_9__hydro_level_1_5_is_chosen_level_lowered_to_1__reading_OK, Fixture)
{
	AreaName yearNumber = "9";
	String level = "1.5";
	AreaName::Vector splitKey = { "hl", "area 2", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, level));

	BOOST_CHECK_EQUAL(my_rule.hydroLevels.get_value(yearNumber.to<uint>(), area_2->index), 1.);

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(study->scenarioHydroLevels[area_2->index][yearNumber.to<uint>()], 1.);
}

BOOST_FIXTURE_TEST_CASE(on_area3_and_on_year_5__hydro_level_neg_3_5_is_chosen__level_raised_to_0__reading_OK, Fixture)
{
	AreaName yearNumber = "5";
	String level = "-3.5";
	AreaName::Vector splitKey = { "hl", "area 3", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, level));

	BOOST_CHECK_EQUAL(my_rule.hydroLevels.get_value(yearNumber.to<uint>(), area_3->index), 0.);

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(study->scenarioHydroLevels[area_3->index][yearNumber.to<uint>()], 0.);
}

// ======================
// Tests on Links NTC
// ======================
BOOST_FIXTURE_TEST_CASE(on_link_area1_area2_and_on_year_0__ntc_TS_number_10_is_chosen__reading_OK, Fixture)
{	
	AreaName yearNumber = "0";
	String tsNumber = "10";
	AreaName::Vector splitKey = {"ntc", "area 1", "area 2", yearNumber};
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));
	
	BOOST_CHECK_EQUAL(my_rule.linksNTC[area_1->index].get(link_12, yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(link_12->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_FIXTURE_TEST_CASE(on_link_area1_area3_and_on_year_15__ntc_TS_number_7_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "15";
	String tsNumber = "7";
	AreaName::Vector splitKey = { "ntc", "area 1", "area 3", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.linksNTC[area_1->index].get(link_13, yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(link_13->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_FIXTURE_TEST_CASE(on_link_area2_area3_and_on_year_19__ntc_TS_number_6_is_chosen__reading_OK, Fixture)
{
	AreaName yearNumber = "19";
	String tsNumber = "6";
	AreaName::Vector splitKey = { "ntc", "area 2", "area 3", yearNumber };
	BOOST_CHECK(my_rule.readLine(splitKey, tsNumber));

	BOOST_CHECK_EQUAL(my_rule.linksNTC[area_2->index].get(link_23, yearNumber.to<uint>()), tsNumber.to<uint>());

	BOOST_CHECK(my_rule.apply());
	BOOST_CHECK_EQUAL(link_23->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// ========================
// Tests on Binding Constraints
// ========================
BOOST_FIXTURE_TEST_CASE(binding_constraints_group_groupTest__Load_TS_4_for_year_3__reading_OK, Fixture)
{
    auto yearNumber = 3;
    auto tsNumber = 4;

    AreaName::Vector splitKey = { "bc", "groupTest", std::to_string(yearNumber) };
    BOOST_CHECK(my_rule.readLine(splitKey, std::to_string(tsNumber)));
    BOOST_CHECK_EQUAL(my_rule.binding_constraints.get("groupTest", yearNumber), tsNumber);

    BOOST_CHECK(my_rule.apply());
    auto actual = study->bindingConstraintsGroups["groupTest"]->timeseriesNumbers[0][yearNumber];
    BOOST_CHECK_EQUAL(actual, tsNumber-1);
}

BOOST_AUTO_TEST_SUITE_END()
