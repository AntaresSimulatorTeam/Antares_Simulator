#define BOOST_TEST_MODULE test save link properties.ini

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <string>

#include <study.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

// =========================
// Output directory finder
// =========================

class outputDirFinder
{
public:
	outputDirFinder() = default;
	~outputDirFinder() = default;
	bool search();
	fs::path get() { return outputDir_; }
private:
	bool findAntaresRoot();
	bool findOutputFolderFromRoot();
private:
	fs::path outputDir_;
	fs::path antaresRootDir_;
};

bool outputDirFinder::search()
{
	if (not findAntaresRoot())
		return false;
	if (not findOutputFolderFromRoot())
		return false;
	return true;
}

bool outputDirFinder::findAntaresRoot()
{
	// Goes up incrementaly and search for a ".git" directory, meaning it has reached the Antares
	// root directory.
	fs::path start_path = fs::current_path();
	fs::path current_path = start_path;
	auto it = start_path.end();
	if (it->string() == "")
		it--;
	for (; it != start_path.begin(); --it)
	{
		for (auto const& dir_entry : fs::directory_iterator{ current_path })
		{
			fs::path entry = dir_entry.path().filename();
			if (dir_entry.path().filename() == ".git")
			{
				antaresRootDir_ = current_path;
				return true;
			}
		}
		current_path = current_path.parent_path();
	}
	return false;
}

bool outputDirFinder::findOutputFolderFromRoot()
{
	outputDir_ = antaresRootDir_;
	outputDir_.append("src").append("tests").append("src").append("libs").append("antares")
			  .append("study").append("area").append("out");
	if (not exists(outputDir_) && not is_directory(outputDir_))
		return false;
	return true;
}

// ==================================
// Clean ouput from generated files
// ==================================

void clean_output(const fs::path & outputDir, vector<string> filesToRemove)
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

	outputDirFinder outputPathFinder;
	BOOST_CHECK(outputPathFinder.search());

	fs::path output_dir = outputPathFinder.get();

	BOOST_CHECK(AreaLinksSaveToFolder(area_1, output_dir.string().c_str()));
	BOOST_CHECK(compare_ini_files(output_dir, "properties.ini", "properties-ref-0.ini"));

	vector<string> filesToRemove = { "area 2.txt", "properties.ini" };
	clean_output(output_dir, filesToRemove);
}
