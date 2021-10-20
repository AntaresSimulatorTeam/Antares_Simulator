#define BOOST_TEST_MODULE test save area optimization.ini

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <string>

#include <study.h>
#include <filter.h>
#include <area/constants.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

// ==================================
// Clean ouput from generated files
// ==================================

void clean_output(const vector<string>& filesToRemove)
{
	for (int i = 0; i != filesToRemove.size(); i++)
	{
		fs::path fileToRemove = filesToRemove[i];
		if (exists(fileToRemove))
			remove(fileToRemove);
	}
}

// ======================================
// Compare ini files in output folder
// ======================================

bool compare_ini_files(const string& fileName, const string& fileNameRef)
{
	ifstream in1(fileName);
	ifstream in2(fileNameRef);

	while ((!in1.eof()) && (!in2.eof()))
	{
		string line1, line2;
		getline(in1, line1);
		getline(in2, line2);
		if (line1 != line2)
		{
			in1.close();
			in2.close();
			return false;
		}
	}

	in1.close();
	in2.close();
	return true;
}

bool fileContainsLine(const string& fileName, const string& line_to_find)
{
	ifstream in(fileName);
	while (!in.eof())
	{
		string line;
		getline(in, line);
		if (line == line_to_find)
		{
			in.close();
			return true;
		}
	}
	in.close();
	return false;
}

BOOST_AUTO_TEST_CASE(one_area_with_default_params)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";
	string referenceIniFileName = "optimization-reference.ini";

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	// Creation of reference .ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[nodal optimization]" << endl;
	referenceFile << "non-dispatchable-power = true" << endl;
	referenceFile << "dispatchable-hydro-power = true" << endl;
	referenceFile << "other-dispatchable-power = true" << endl;
	referenceFile << "spread-unsupplied-energy-cost = 0.000000" << endl;
	referenceFile << "spread-spilled-energy-cost = 0.000000" << endl;
	referenceFile << endl;
	referenceFile << "[filtering]" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(compare_ini_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}


BOOST_AUTO_TEST_CASE(one_area_with_none_default_params)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";
	string referenceIniFileName = "optimization-reference.ini";

	// Setting area's properties
	area->nodalOptimization = 0;
	area->spreadUnsuppliedEnergyCost = 2.;
	area->spreadSpilledEnergyCost = 3.;
	area->filterSynthesis = filterNone;
	area->filterYearByYear = filterHourly | filterDaily;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	// Creation of reference .ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[nodal optimization]" << endl;
	referenceFile << "non-dispatchable-power = false" << endl;
	referenceFile << "dispatchable-hydro-power = false" << endl;
	referenceFile << "other-dispatchable-power = false" << endl;
	referenceFile << "spread-unsupplied-energy-cost = 2.000000" << endl;
	referenceFile << "spread-spilled-energy-cost = 3.000000" << endl;
	referenceFile << endl;
	referenceFile << "[filtering]" << endl;
	referenceFile << "filter-synthesis = " << endl;
	referenceFile << "filter-year-by-year = hourly, daily" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(compare_ini_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_nonDispatchPower__other_params_to_default)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";
	string referenceIniFileName = "optimization-reference.ini";

	// Setting area's properties
	area->nodalOptimization = anoNonDispatchPower;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	// Creation of reference .ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[nodal optimization]" << endl;
	referenceFile << "non-dispatchable-power = true" << endl;
	referenceFile << "dispatchable-hydro-power = false" << endl;
	referenceFile << "other-dispatchable-power = false" << endl;
	referenceFile << "spread-unsupplied-energy-cost = 0.000000" << endl;
	referenceFile << "spread-spilled-energy-cost = 0.000000" << endl;
	referenceFile << endl;
	referenceFile << "[filtering]" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));
	BOOST_CHECK(compare_ini_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}


BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_dispatchHydroPower__other_params_to_default)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";
	string referenceIniFileName = "optimization-reference.ini";

	// Setting area's properties
	area->nodalOptimization = anoDispatchHydroPower;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	// Creation of reference .ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[nodal optimization]" << endl;
	referenceFile << "non-dispatchable-power = false" << endl;
	referenceFile << "dispatchable-hydro-power = true" << endl;
	referenceFile << "other-dispatchable-power = false" << endl;
	referenceFile << "spread-unsupplied-energy-cost = 0.000000" << endl;
	referenceFile << "spread-spilled-energy-cost = 0.000000" << endl;
	referenceFile << endl;
	referenceFile << "[filtering]" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));
	BOOST_CHECK(compare_ini_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_otherDispatchablePower__other_params_to_default)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";
	string referenceIniFileName = "optimization-reference.ini";

	// Setting area's properties
	area->nodalOptimization = anoOtherDispatchPower;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	// Creation of reference .ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[nodal optimization]" << endl;
	referenceFile << "non-dispatchable-power = false" << endl;
	referenceFile << "dispatchable-hydro-power = false" << endl;
	referenceFile << "other-dispatchable-power = true" << endl;
	referenceFile << "spread-unsupplied-energy-cost = 0.000000" << endl;
	referenceFile << "spread-spilled-energy-cost = 0.000000" << endl;
	referenceFile << endl;
	referenceFile << "[filtering]" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));
	BOOST_CHECK(compare_ini_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_non_or_other_DispatchPower__other_params_to_default)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";
	string referenceIniFileName = "optimization-reference.ini";

	// Setting area's properties
	area->nodalOptimization = anoOtherDispatchPower | anoNonDispatchPower;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	// Creation of reference .ini file
	ofstream referenceFile;
	referenceFile.open(referenceIniFileName);
	referenceFile << "[nodal optimization]" << endl;
	referenceFile << "non-dispatchable-power = true" << endl;
	referenceFile << "dispatchable-hydro-power = false" << endl;
	referenceFile << "other-dispatchable-power = true" << endl;
	referenceFile << "spread-unsupplied-energy-cost = 0.000000" << endl;
	referenceFile << "spread-spilled-energy-cost = 0.000000" << endl;
	referenceFile << endl;
	referenceFile << "[filtering]" << endl;
	referenceFile << "filter-synthesis = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << "filter-year-by-year = hourly, daily, weekly, monthly, annual" << endl;
	referenceFile << endl;
	referenceFile.close();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));
	BOOST_CHECK(compare_ini_files(generatedIniFileName, referenceIniFileName));

	vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_area_with_unsupplied_energy_cost_negative__other_params_to_default)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";

	// Setting area's properties
	area->spreadUnsuppliedEnergyCost = -1.;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "spread-unsupplied-energy-cost = -1.000000"));

	vector<string> filesToRemove = { generatedIniFileName };
	clean_output(filesToRemove);
}

BOOST_AUTO_TEST_CASE(one_area_with_spilled_energy_cost_negative__other_params_to_default)
{
	Study::Ptr study = new Study();
	Area* area = study->areaAdd("Area");

	string generatedIniFileName = "optimization.ini";

	// Setting area's properties
	area->spreadSpilledEnergyCost = -1.;

	Yuni::Clob path_to_generated_file;
	path_to_generated_file << fs::current_path().append(generatedIniFileName).string();

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));
	BOOST_CHECK(fileContainsLine(generatedIniFileName, "spread-spilled-energy-cost = -1.000000"));

	vector<string> filesToRemove = { generatedIniFileName };
	clean_output(filesToRemove);
}