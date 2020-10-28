#define BOOST_TEST_MODULE test-end-to-end tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/system.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <stdio.h>

using namespace boost;

namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

using namespace std;


BOOST_AUTO_TEST_SUITE(solver_call)

optional<filesystem::path> find_file(const filesystem::path& dir_path, const filesystem::path& file_name) {
	const filesystem::recursive_directory_iterator end;
	const auto it = find_if(filesystem::recursive_directory_iterator(dir_path), end,
		[&file_name](const filesystem::directory_entry& e) {
			return e.path().filename() == file_name;
		});
	return it == end ? optional<filesystem::path>() : it->path();
}

optional<filesystem::path> find_file(const filesystem::path& dir_path, const boost::regex my_filter) {
	const filesystem::recursive_directory_iterator end;
	boost::smatch what;
	const auto it = find_if(filesystem::recursive_directory_iterator(dir_path), end,
		[&my_filter, &what](const filesystem::directory_entry& e) {
			return boost::regex_match(e.path().filename().string(), what, my_filter);
		});
	return it == end ? optional<filesystem::path>() : it->path();
}

optional<filesystem::path> find_file(const std::vector<filesystem::path>& paths, const boost::regex my_filter) {

	optional<filesystem::path> result;

	for (const filesystem::path& path : paths) {

		if (filesystem::is_directory(path))
		{
			result = find_file(path, my_filter);
			if (result) {
				break;
			}
		}
	}

	return result;
}

std::string getBuildDirectory()
{
	std::string result = dll::program_location().parent_path().string();

    //For windows debug or release directory is added
#if defined(_WIN32) || defined(WIN32)
	result += "/../../..";
#else
	result += "/../..";
#endif

	return result;
}

void launchSolver(const std::string& studyPath)
{
	std::string buildDir = getBuildDirectory();
	std::string studyOutputPath = studyPath + "output/";

	//Remove any available output and logs
	filesystem::remove_all(studyOutputPath);
	filesystem::remove_all(studyPath + "logs/");

	//Get solver path
	std::vector<filesystem::path> paths;
    
    //For windows debug or release directory is added
	paths.push_back(filesystem::path(buildDir + "/solver/Debug"));
	paths.push_back(filesystem::path(buildDir + "/solver/Release"));
    
    //For linux no directory is added
	paths.push_back(filesystem::path(buildDir + "/solver"));

	const boost::regex filter("antares-[0-9]*.[0-9]*-solver.*");
	optional<filesystem::path> solverPath = find_file(paths, filter);

	BOOST_REQUIRE_MESSAGE(solverPath, "Can't find antares solver application");

	std::string solverLaunchCommand = solverPath.value().string() + " -i \"" + studyPath + "\"";

	//Launch solver with study
	int result = process::system(solverLaunchCommand);
	BOOST_REQUIRE_MESSAGE(result == 0, "Solver returned error");
}

void checkIntegrityFile(const std::string& studyOutputPath, const std::vector<double>& checkIntegrityExpectedValues)
{
	BOOST_REQUIRE_MESSAGE(checkIntegrityExpectedValues.size() == 8 , "invalid checkIntegrity expected values size. 8 values needed");

	//Parse checkIntegrity.txt file to check objective functions result
	optional<filesystem::path> checkIntegrityFilePath = find_file(studyOutputPath, "checkIntegrity.txt");

	BOOST_REQUIRE_MESSAGE(checkIntegrityFilePath, "Can't find checkIntegrity.txt file in output folder '" << studyOutputPath << "'");
	
	ifstream checkIntegrityFile;
	checkIntegrityFile.open(checkIntegrityFilePath.value().string());

	BOOST_REQUIRE_MESSAGE(checkIntegrityFile.is_open(), "Can't open '" << checkIntegrityFilePath.value().string() << "'");

	std::vector<string> values;
	std::string line;
	while (getline(checkIntegrityFile, line))
	{
		values.push_back(line);
	}
	checkIntegrityFile.close();

	BOOST_REQUIRE(values.size() == checkIntegrityExpectedValues.size());

	for (int i = 0; i < values.size(); i++)
	{
		BOOST_TEST(std::stod(values[i]) == checkIntegrityExpectedValues[i], tt::tolerance(0.001));
	}

	checkIntegrityFile.close();
}

//Test free data sample study objective functions result
BOOST_AUTO_TEST_CASE(free_data_sample)
{
	const std::string& studyPath		= getBuildDirectory() + "/tests/data/free_data_sample/";

	const std::vector<double>& checkIntegrityExpectedValues = { 2.85657392370263e+11,
																0.00000000000000e+00,
																2.85657392370263e+11,
																2.85657392370263e+11,
																2.85657544872729e+11,
																0.00000000000000e+00,
																2.85657544872729e+11,
																2.85657544872729e+11 };

	launchSolver(studyPath);

	std::string studyOutputPath = studyPath + "output/";
	checkIntegrityFile(studyOutputPath, checkIntegrityExpectedValues);

	//Remove any available output and logs
	filesystem::remove_all(studyOutputPath);
	filesystem::remove_all(studyPath + "logs/");
}

BOOST_AUTO_TEST_SUITE_END()
