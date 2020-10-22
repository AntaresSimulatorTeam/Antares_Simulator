#define BOOST_TEST_MODULE test-end-to-end tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/system.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/optional.hpp>

#include <iostream>
#include <stdio.h>

using namespace boost;

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

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

void launchSolver(const std::string& studyPath)
{
	std::string studyOutputPath = studyPath + "output/";

	//Remove any available output and logs
	filesystem::remove_all(studyOutputPath);
	filesystem::remove_all(studyPath + "logs/");

	//Get solver path
	std::vector<filesystem::path> paths;
	paths.push_back(filesystem::path(dll::program_location().parent_path().string() + "/../../../solver/Debug"));
	paths.push_back(filesystem::path(dll::program_location().parent_path().string() + "/../../../solver/Release"));

	filesystem::path solverPath = process::search_path("antares-7.2-solver", paths);

	std::string solverLaunchCommand = solverPath.string() + " -i \"" + studyPath + "\"";

	//Launch solver with study
	int result = process::system(solverLaunchCommand);
	BOOST_REQUIRE_MESSAGE(result == 0, "Solver returned error");
}

void checkIntegrityFile(const std::string& studyOutputPath, const std::vector<double>& checkIntegrityExpectedValues)
{
	BOOST_REQUIRE_MESSAGE(checkIntegrityExpectedValues.size() == 8 , "invalid checkIntegrity expected values size. 8 values needed");

	//Parse checkIntegrity.txt file to check objective functions result
	optional<filesystem::path> checkIntegrityFilePath = find_file(studyOutputPath, "checkIntegrity.txt");

	BOOST_REQUIRE_MESSAGE(checkIntegrityFilePath.has_value(), "Can't find checkIntegrity.txt file in output folder '" << studyOutputPath << "'");
	
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
	const std::string& studyPath		= dll::program_location().parent_path().string() + "/../../data/free_data_sample/";

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
