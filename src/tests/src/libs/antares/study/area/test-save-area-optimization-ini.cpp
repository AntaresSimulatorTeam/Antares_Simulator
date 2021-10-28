#define BOOST_TEST_MODULE test save area optimization.ini

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <string>

#include <study.h>
#include <filter.h>
#include <area/constants.h>

#include "files-helper.h"

using namespace Antares::Data;
namespace fs = std::filesystem;

const string generatedIniFileName = "optimization.ini";
const string referenceIniFileName = "optimization-reference.ini";

struct Fixture
{
	Fixture(const Fixture& f) = delete;
	Fixture(const Fixture&& f) = delete;
	Fixture& operator= (const Fixture& f) = delete;
	Fixture& operator= (const Fixture&& f) = delete;
	Fixture() : area(study->areaAdd("Area"))
	{
		path_to_generated_file << fs::current_path().append(generatedIniFileName).string();
	}

	~Fixture()
	{
		vector<string> filesToRemove = { generatedIniFileName, referenceIniFileName };
		remove_files(filesToRemove);
	}

	Study::Ptr study = new Study();
	Area* area;
	Yuni::Clob path_to_generated_file;
};

class referenceIniFile
{
public:
	referenceIniFile();
	~referenceIniFile() = default;
	string name() const { return name_; }
	void save();

	void set_property(const string& key, const string& value) { properties_[key] = value; }
private:
	void save_section(string_view sectionTitle, vector<string>& section_properties, ofstream& file);

	string name_ = referenceIniFileName;
	vector<string> nodal_property_names_ = {
		"non-dispatchable-power", "dispatchable-hydro-power", "other-dispatchable-power", 
		"spread-unsupplied-energy-cost", "spread-spilled-energy-cost" };
	vector<string> filtering_property_names_ = { "filter-synthesis", "filter-year-by-year" };
	map<string, string, less<>> properties_;
};

referenceIniFile::referenceIniFile()
{
	properties_[nodal_property_names_[0]] = "true";
	properties_[nodal_property_names_[1]] = "true";
	properties_[nodal_property_names_[2]] = "true";
	properties_[nodal_property_names_[3]] = "0.000000";
	properties_[nodal_property_names_[4]] = "0.000000";
	properties_[filtering_property_names_[0]] = "hourly, daily, weekly, monthly, annual";
	properties_[filtering_property_names_[1]] = "hourly, daily, weekly, monthly, annual";
}

void referenceIniFile::save()
{
	ofstream file;
	file.open(name_);
	save_section("[nodal optimization]", nodal_property_names_, file);
	save_section("[filtering]", filtering_property_names_, file);
	file.close();
}

void referenceIniFile::save_section(string_view sectionTitle, vector<string>& sectionProperties, ofstream & file)
{
	file << sectionTitle << endl;
	for (int i = 0; i < sectionProperties.size(); ++i)
		file << sectionProperties[i] << " = " << properties_[sectionProperties[i]] << endl;
	file << endl;
}

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(one_area_with_default_params)
{
	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	referenceIniFile referenceFile;
	referenceFile.save();

	BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_AUTO_TEST_CASE(one_area_with_none_default_params)
{
	area->nodalOptimization = 0;
	area->spreadUnsuppliedEnergyCost = 2.;
	area->spreadSpilledEnergyCost = 3.;
	area->filterSynthesis = filterNone;
	area->filterYearByYear = filterHourly | filterDaily;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	referenceIniFile referenceFile;
	referenceFile.set_property("non-dispatchable-power", "false");
	referenceFile.set_property("dispatchable-hydro-power", "false");
	referenceFile.set_property("other-dispatchable-power", "false");
	referenceFile.set_property("spread-unsupplied-energy-cost", "2.000000");
	referenceFile.set_property("spread-spilled-energy-cost", "3.000000");
	referenceFile.set_property("filter-synthesis", "");
	referenceFile.set_property("filter-year-by-year", "hourly, daily");
	referenceFile.save();

	BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_nonDispatchPower__other_params_to_default)
{
	area->nodalOptimization = anoNonDispatchPower;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	referenceIniFile referenceFile;
	referenceFile.set_property("non-dispatchable-power", "true");
	referenceFile.set_property("dispatchable-hydro-power", "false");
	referenceFile.set_property("other-dispatchable-power", "false");
	referenceFile.save();

	BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}



BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_dispatchHydroPower__other_params_to_default)
{
	area->nodalOptimization = anoDispatchHydroPower;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	referenceIniFile referenceFile;
	referenceFile.set_property("non-dispatchable-power", "false");
	referenceFile.set_property("dispatchable-hydro-power", "true");
	referenceFile.set_property("other-dispatchable-power", "false");
	referenceFile.save();

	BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_otherDispatchablePower__other_params_to_default)
{
	area->nodalOptimization = anoOtherDispatchPower;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	referenceIniFile referenceFile;
	referenceFile.set_property("non-dispatchable-power", "false");
	referenceFile.set_property("dispatchable-hydro-power", "false");
	referenceFile.set_property("other-dispatchable-power", "true");
	referenceFile.save();

	BOOST_CHECK(files_identical(generatedIniFileName, referenceFile.name()));
}

BOOST_AUTO_TEST_CASE(one_area_with_nodal_opt_to_non_or_other_DispatchPower__other_params_to_default)
{
	area->nodalOptimization = anoOtherDispatchPower | anoNonDispatchPower;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	referenceIniFile referenceFile;
	referenceFile.set_property("non-dispatchable-power", "true");
	referenceFile.set_property("dispatchable-hydro-power", "false");
	referenceFile.set_property("other-dispatchable-power", "true");
	referenceFile.save();

	BOOST_CHECK(files_identical(generatedIniFileName, referenceIniFileName));
}

BOOST_AUTO_TEST_CASE(one_area_with_unsupplied_energy_cost_negative__other_params_to_default)
{
	area->spreadUnsuppliedEnergyCost = -1.;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	BOOST_CHECK(fileContainsLine(generatedIniFileName, "spread-unsupplied-energy-cost = -1.000000"));
}

BOOST_AUTO_TEST_CASE(one_area_with_spilled_energy_cost_negative__other_params_to_default)
{
	area->spreadSpilledEnergyCost = -1.;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	BOOST_CHECK(fileContainsLine(generatedIniFileName, "spread-spilled-energy-cost = -1.000000"));
}

BOOST_AUTO_TEST_CASE(one_area_with_synthesis_to_hourly_monthly_annual__other_params_to_default)
{
	area->filterSynthesis = filterWeekly | filterMonthly | filterAnnual;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	BOOST_CHECK(fileContainsLine(generatedIniFileName, "filter-synthesis = weekly, monthly, annual"));
}

BOOST_AUTO_TEST_CASE(one_area_with_year_by_year_to_daily_monthly__other_params_to_default)
{
	area->filterYearByYear = filterDaily | filterMonthly;

	BOOST_CHECK(saveAreaOptimisationIniFile(*area, path_to_generated_file));

	BOOST_CHECK(fileContainsLine(generatedIniFileName, "filter-year-by-year = daily, monthly"));
}

BOOST_AUTO_TEST_SUITE_END()