#define BOOST_TEST_MODULE test read scenario-builder.dat 

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <study.h>
#include <rules.h>

using namespace std;
using namespace Antares::Data;


// ===========================
// Add a cluster to an area
// ===========================
template<class ClusterType>
void addClusterToAreaList(Area* area, std::shared_ptr<ClusterType> cluster)
{}

template<>
void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
	area->thermal.clusters.push_back(cluster.get());
	area->thermal.list.add(cluster);
	area->thermal.list.mapping[cluster->id()] = cluster;
}

template<>
void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
	area->renewable.clusters.push_back(cluster.get());
	area->renewable.list.add(cluster);
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
	auto cluster = std::make_shared<ClusterType>(area);
	cluster->setName(clusterName);
	cluster->series = new DataSeriesCommon();

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
	Fixture() : study(new Study()), my_rule(*study)
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
		area_1->load.series->series.resize(nbReadyMadeTS, 1);
		area_2->load.series->series.resize(nbReadyMadeTS, 1);
		area_3->load.series->series.resize(nbReadyMadeTS, 1);

		// Wind : set the nb of ready made TS
		nbReadyMadeTS = 17;
		area_1->wind.series->series.resize(nbReadyMadeTS, 1);
		area_2->wind.series->series.resize(nbReadyMadeTS, 1);
		area_3->wind.series->series.resize(nbReadyMadeTS, 1);

		// Solar : set the nb of ready made TS
		nbReadyMadeTS = 9;
		area_1->solar.series->series.resize(nbReadyMadeTS, 1);
		area_2->solar.series->series.resize(nbReadyMadeTS, 1);
		area_3->solar.series->series.resize(nbReadyMadeTS, 1);

		// Hydro : set the nb of ready made TS
		nbReadyMadeTS = 12;
		area_1->hydro.series->count = nbReadyMadeTS;
		area_2->hydro.series->count = nbReadyMadeTS;
		area_3->hydro.series->count = nbReadyMadeTS;

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
		thCluster_11->series->series.resize(14, 1);		
		thCluster_12->series->series.resize(14, 1);
		thCluster_31->series->series.resize(14, 1);

		// Thermal clusters : update areas local numbering for clusters
		area_1->thermal.prepareAreaWideIndexes();
		area_2->thermal.prepareAreaWideIndexes();
		area_3->thermal.prepareAreaWideIndexes();

		// Add renewable clusters
		rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
		rnCluster_31 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-31");
		rnCluster_32 = addClusterToArea<RenewableCluster>(area_3, "rn-cluster-32");

		// Renewable clusters : set the nb of ready made TS
		rnCluster_21->series->series.resize(9, 1);
		rnCluster_31->series->series.resize(9, 1);
		rnCluster_32->series->series.resize(9, 1);

		// Renewable clusters : update areas local numbering for clusters
		area_1->renewable.prepareAreaWideIndexes();
		area_2->renewable.prepareAreaWideIndexes();
		area_3->renewable.prepareAreaWideIndexes();

		// Resize all TS numbers storage (1 column x nbYears lines) 
		area_1->resizeAllTimeseriesNumbers(study->parameters.nbYears);
		area_2->resizeAllTimeseriesNumbers(study->parameters.nbYears);
		area_3->resizeAllTimeseriesNumbers(study->parameters.nbYears);

		BOOST_CHECK(my_rule.reset());
	}

	~Fixture() {}

	Study::Ptr study;
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

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

// =================
// Tests on Load
// =================
BOOST_AUTO_TEST_CASE(on_area2_and_on_year_18__load_TS_number_11_is_chosen__reading_OK)
{
	AreaName yearNumber = "18";
	String tsNumber = "11";
	AreaName::Vector splitKey = { "l", "area 2", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.load.get_value(yearNumber.to<uint>(), area_2->index), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(area_2->load.series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Wind
// =================
BOOST_AUTO_TEST_CASE(on_area3_and_on_year_7__wind_TS_number_5_is_chosen__reading_OK)
{
	AreaName yearNumber = "7";
	String tsNumber = "5";
	AreaName::Vector splitKey = { "w", "area 3", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.wind.get_value(yearNumber.to<uint>(), area_3->index), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(area_3->wind.series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Solar
// =================
BOOST_AUTO_TEST_CASE(on_area1_and_on_year_4__solar_TS_number_8_is_chosen__reading_OK)
{
	AreaName yearNumber = "4";
	String tsNumber = "8";
	AreaName::Vector splitKey = { "s", "area 1", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.solar.get_value(yearNumber.to<uint>(), area_1->index), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(area_1->solar.series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

// =================
// Tests on Hydro
// =================
BOOST_AUTO_TEST_CASE(on_area2_and_on_year_15__solar_TS_number_3_is_chosen__reading_OK)
{
	AreaName yearNumber = "15";
	String tsNumber = "3";
	AreaName::Vector splitKey = { "h", "area 2", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.hydro.get_value(yearNumber.to<uint>(), area_2->index), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(area_2->hydro.series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}


// ===========================
// Tests on Thermal clusters
// ===========================
BOOST_AUTO_TEST_CASE(on_th_cluster11_of_area1_and_on_year_6__solar_TS_number_3_is_chosen__reading_OK)
{
	AreaName yearNumber = "6";
	String tsNumber = "3";
	AreaName::Vector splitKey = { "t", "area 1", yearNumber, "th-cluster-11" };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.thermal[area_1->index].get(thCluster_11.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(thCluster_11->series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_CASE(on_th_cluster12_of_area1_and_on_year_13__solar_TS_number_5_is_chosen__reading_OK)
{
	AreaName yearNumber = "13";
	String tsNumber = "5";
	AreaName::Vector splitKey = { "t", "area 1", yearNumber, "th-cluster-12" };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.thermal[area_1->index].get(thCluster_12.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(thCluster_12->series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_CASE(on_th_cluster31_of_area3_and_on_year_10__solar_TS_number_7_is_chosen__reading_OK)
{
	AreaName yearNumber = "10";
	String tsNumber = "7";
	AreaName::Vector splitKey = { "t", "area 3", yearNumber, "th-cluster-31" };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.thermal[area_3->index].get(thCluster_31.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(thCluster_31->series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}


// =============================
// Tests on Renewable clusters
// =============================
BOOST_AUTO_TEST_CASE(on_rn_cluster21_of_area2_and_on_year_16__solar_TS_number_8_is_chosen__reading_OK)
{
	study->parameters.renewableGeneration.toClusters();

	AreaName yearNumber = "16";
	String tsNumber = "8";
	AreaName::Vector splitKey = { "r", "area 2", yearNumber, "rn-cluster-21" };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.renewable[area_2->index].get(rnCluster_21.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(rnCluster_21->series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_CASE(on_rn_cluster32_of_area3_and_on_year_2__solar_TS_number_4_is_chosen__reading_OK)
{
	study->parameters.renewableGeneration.toClusters();

	AreaName yearNumber = "2";
	String tsNumber = "4";
	AreaName::Vector splitKey = { "r", "area 3", yearNumber, "rn-cluster-32" };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.renewable[area_3->index].get(rnCluster_32.get(), yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(rnCluster_32->series->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}


// ========================
// Tests on Hydro levels
// ========================
BOOST_AUTO_TEST_CASE(on_area1_and_on_year_17__hydro_level_0_123_is_chosen__reading_OK)
{
	AreaName yearNumber = "17";
	String level = "0.123";
	AreaName::Vector splitKey = { "hl", "area 1", yearNumber };
	my_rule.readLine(splitKey, level, false);

	BOOST_CHECK_EQUAL(my_rule.hydroLevels.get_value(yearNumber.to<uint>(), area_1->index), level.to<double>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(study->scenarioHydroLevels[area_1->index][yearNumber.to<uint>()], level.to<double>());
}

BOOST_AUTO_TEST_CASE(on_area2_and_on_year_9__hydro_level_1_5_is_chosen_level_lowered_to_1__reading_OK)
{
	AreaName yearNumber = "9";
	String level = "1.5";
	AreaName::Vector splitKey = { "hl", "area 2", yearNumber };
	my_rule.readLine(splitKey, level, false);

	BOOST_CHECK_EQUAL(my_rule.hydroLevels.get_value(yearNumber.to<uint>(), area_2->index), 1.);

	my_rule.apply();
	BOOST_CHECK_EQUAL(study->scenarioHydroLevels[area_2->index][yearNumber.to<uint>()], 1.);
}

BOOST_AUTO_TEST_CASE(on_area3_and_on_year_5__hydro_level_neg_3_5_is_chosen__level_raised_to_0__reading_OK)
{
	AreaName yearNumber = "5";
	String level = "-3.5";
	AreaName::Vector splitKey = { "hl", "area 3", yearNumber };
	my_rule.readLine(splitKey, level, false);

	BOOST_CHECK_EQUAL(my_rule.hydroLevels.get_value(yearNumber.to<uint>(), area_3->index), 0.);

	my_rule.apply();
	BOOST_CHECK_EQUAL(study->scenarioHydroLevels[area_3->index][yearNumber.to<uint>()], 0.);
}

// ======================
// Tests on Links NTC
// ======================
BOOST_AUTO_TEST_CASE(on_link_area1_area2_and_on_year_0__ntc_TS_number_10_is_chosen__reading_OK)
{	
	AreaName yearNumber = "0";
	String tsNumber = "10";
	AreaName::Vector splitKey = {"tc", "area 1", "area 2", yearNumber};
	my_rule.readLine(splitKey, tsNumber, false);
	
	BOOST_CHECK_EQUAL(my_rule.linksNTC[area_1->index].get(link_12, yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(link_12->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_CASE(on_link_area1_area3_and_on_year_15__ntc_TS_number_7_is_chosen__reading_OK)
{
	AreaName yearNumber = "15";
	String tsNumber = "7";
	AreaName::Vector splitKey = { "tc", "area 1", "area 3", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.linksNTC[area_1->index].get(link_13, yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(link_13->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_CASE(on_link_area2_area3_and_on_year_19__ntc_TS_number_6_is_chosen__reading_OK)
{
	AreaName yearNumber = "19";
	String tsNumber = "6";
	AreaName::Vector splitKey = { "tc", "area 2", "area 3", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.linksNTC[area_2->index].get(link_23, yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(link_23->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_SUITE_END()