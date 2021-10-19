#define BOOST_TEST_MODULE test save link properties.ini

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <string>

#include <study.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

// ==================================
// Clean ouput from generated files
// ==================================

void clean_output(const fs::path & outputDir, const vector<string> & filesToRemove)
{
	
	for (int i = 0; i != filesToRemove.size(); i++)
	{
		fs::path fileToRemove = outputDir;
		fileToRemove.append(filesToRemove[i]);
		if (exists(fileToRemove))
			remove(fileToRemove);
	}
}

// ======================================
// Compare ini files in output folder
// ======================================

bool compare_ini_files(const fs::path & outputDir, const string & fileName, const string & fileNameRef)
{
	fs::path filePath = outputDir;
	fs::path filePathRef = outputDir;

	filePath.append(fileName);
	filePathRef.append(fileNameRef);

	if (!exists(filePath) || !exists(filePathRef))
		return false;
	
	ifstream in1(filePath.string());
	ifstream in2(filePathRef.string());
	while ((!in1.eof()) && (!in2.eof()))
	{
		string line1, line2;
		getline(in1, line1);
		getline(in2, line2);
		if (line1 != line2)
			return false;
	}

	in1.close();
	in2.close();

	return true;
}

BOOST_AUTO_TEST_CASE(one_link_with_default_values)
{
	Study::Ptr study = new Study();
	Area* area_1 = study->areaAdd("Area 1");
	Area* area_2 = study->areaAdd("Area 2");
	AreaLink* link = AreaAddLinkBetweenAreas(area_1, area_2, false);
	// Reduce size of link's time-series dump (0 Ko)
	link->data.resize(0, 0);

	ofstream referenceFile;
	referenceFile.open("properties-reference.ini");
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
	BOOST_CHECK(compare_ini_files(fs::current_path(), "properties.ini", "properties-reference.ini"));

	vector<string> filesToRemove = { "area 2.txt", "properties.ini", "properties-reference.ini" };
	clean_output(fs::current_path(), filesToRemove);
}
