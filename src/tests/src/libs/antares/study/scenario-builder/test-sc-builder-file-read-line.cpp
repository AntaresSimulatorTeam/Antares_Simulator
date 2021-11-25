#define BOOST_TEST_MODULE test read scenario-builder.dat 

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <study.h>
#include <rules.h>

using namespace std;
using namespace Antares::Data;

// Forward declaration
void initializeStudy(Study::Ptr study);

struct Fixture
{
	Fixture(const Fixture & f) = delete;
	Fixture(const Fixture && f) = delete;
	Fixture & operator= (const Fixture & f) = delete;
	Fixture& operator= (const Fixture && f) = delete;
	Fixture() : study(new Study()), my_rule(*study)
	{
		initializeStudy(study);

		area_1 = study->areaAdd("Area 1");
		area_2 = study->areaAdd("Area 2");
		area_3 = study->areaAdd("Area 3");
		study->areas.rebuildIndexes();

		link_12 = AreaAddLinkBetweenAreas(area_1, area_2, false);
		link_12->directCapacities.resize(15, 1);

		link_13 = AreaAddLinkBetweenAreas(area_1, area_3, false);
		link_13->directCapacities.resize(11, 1);

		link_23 = AreaAddLinkBetweenAreas(area_2, area_3, false);
		link_23->directCapacities.resize(8, 1);

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
	ScenarioBuilder::Rules my_rule;
};

void initializeStudy(Study::Ptr study)
{
	study->parameters.nbYears = 20;
	// study->parameters.renewableGeneration.toAggregated(); // Default
}


BOOST_FIXTURE_TEST_SUITE(s, Fixture)

// ===============
// Load
// ===============
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

// ===============
// Links NTC
// ===============
BOOST_AUTO_TEST_CASE(on_link_area1_area2_and_on_year_0__ntc_TS_number_10_is_chosen__reading_OK)
{	
	AreaName yearNumber = "0";
	String tsNumber = "10";
	AreaName::Vector splitKey = {"tc", "area 1", "area 2", yearNumber};
	my_rule.readLine(splitKey, tsNumber, false);
	
	BOOST_CHECK_EQUAL(my_rule.linksNTC->get(link_12, yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(link_12->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_CASE(on_link_area1_area3_and_on_year_15__ntc_TS_number_7_is_chosen__reading_OK)
{
	AreaName yearNumber = "15";
	String tsNumber = "7";
	AreaName::Vector splitKey = { "tc", "area 1", "area 3", yearNumber };
	my_rule.readLine(splitKey, tsNumber, false);

	BOOST_CHECK_EQUAL(my_rule.linksNTC->get(link_13, yearNumber.to<uint>()), tsNumber.to<uint>());

	my_rule.apply();
	BOOST_CHECK_EQUAL(link_13->timeseriesNumbers[0][yearNumber.to<uint>()], tsNumber.to<uint>() - 1);
}

BOOST_AUTO_TEST_SUITE_END()