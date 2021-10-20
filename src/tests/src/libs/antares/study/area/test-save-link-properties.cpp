#define BOOST_TEST_MODULE test save link properties.ini

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <string>

#include <study.h>
#include <filter.h>

#include "files-helper.h"

using namespace Antares::Data;
namespace fs = std::filesystem;


BOOST_AUTO_TEST_CASE(one_link_with_default_values)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";
	string referenceIniFileName = "properties-reference.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	// link's properties are default 

	// Reference properties.ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[area 2]" << endl;
	referenceFile << "hurdles-cost = false" << endl;
	referenceFile << "loop-flow = false" << endl;
	referenceFile << "use-phase-shifter = false" << endl;
	referenceFile << "transmission-capacities = enabled" << endl;
	referenceFile << "asset-type = ac" << endl;
	referenceFile << "link-style = plain" << endl;
	referenceFile << "link-width = 1" << endl;
	referenceFile << "colorr = 112" << endl;
	referenceFile << "colorg = 112" << endl;
	referenceFile << "colorb = 112" << endl;
	referenceFile << "display-comments = true" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();
	
	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(compare_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_none_default_values)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";
	string referenceIniFileName = "properties-reference.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->useHurdlesCost = true;
	link->useLoopFlow = true;
	link->usePST = true;
	link->transmissionCapacities = tncInfinite;
	link->assetType = atDC;
	link->style = stDash;
	link->linkWidth = 3;
	link->color[0] = 10;
	link->color[1] = 9;
	link->color[2] = 8;
	link->displayComments = false;
	link->filterSynthesis = filterNone;
	link->filterYearByYear = filterHourly | filterDaily;

	// Reference properties.ini file 
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[area 2]" << endl;
	referenceFile << "hurdles-cost = true" << endl;
	referenceFile << "loop-flow = true" << endl;
	referenceFile << "use-phase-shifter = true" << endl;
	referenceFile << "transmission-capacities = infinite" << endl;
	referenceFile << "asset-type = dc" << endl;
	referenceFile << "link-style = dash" << endl;
	referenceFile << "link-width = 3" << endl;
	referenceFile << "colorr = 10" << endl;
	referenceFile << "colorg = 9" << endl;
	referenceFile << "colorb = 8" << endl;
	referenceFile << "display-comments = false" << endl;
	referenceFile << "filter-synthesis = " << endl;
	referenceFile << "filter-year-by-year = hourly, daily" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(compare_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}


BOOST_AUTO_TEST_CASE(one_link_with_transmission_capacity_to_ignore__all_others_properties_are_default)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";
	string referenceIniFileName = "properties-reference.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->transmissionCapacities = tncIgnore;

	// Reference properties.ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[area 2]" << endl;
	referenceFile << "hurdles-cost = false" << endl;
	referenceFile << "loop-flow = false" << endl;
	referenceFile << "use-phase-shifter = false" << endl;
	referenceFile << "transmission-capacities = ignore" << endl; // Value tested here : ignore
	referenceFile << "asset-type = ac" << endl;
	referenceFile << "link-style = plain" << endl;
	referenceFile << "link-width = 1" << endl;
	referenceFile << "colorr = 112" << endl;
	referenceFile << "colorg = 112" << endl;
	referenceFile << "colorb = 112" << endl;
	referenceFile << "display-comments = true" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(compare_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_transmission_capacity_to_ignore__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->transmissionCapacities = tncIgnore;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "transmission-capacities = ignore"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_asset_type_to_gas__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->assetType = atGas;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "asset-type = gaz"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_asset_type_to_virtual__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->assetType = atVirt;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "asset-type = virt"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_asset_type_to_other__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->assetType = atOther;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "asset-type = other"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_style_to_dot__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->style = stDot;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "link-style = dot"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_style_to_dotdash__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->style = stDotDash;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "link-style = dotdash"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_synthesis_to_hourly_monthly_annual__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->filterSynthesis = filterWeekly | filterMonthly | filterAnnual;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "filter-synthesis = weekly, monthly, annual"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_link_with_year_by_year_to_daily_monthly__ini_file_contains_matching_line)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);

	string generatedIniFileName = "properties.ini";

	// Setting link properties
	link->data.resize(0, 0);	// Reduce size of link's time-series dump to 0 Ko
	link->filterYearByYear = filterDaily | filterMonthly;

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, fs::current_path().string().c_str()));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "filter-year-by-year = daily, monthly"));

	vector<string> filesToRemove = { "area 2.txt", generatedIniFileName };
	clean_output(filesToRemove);
}