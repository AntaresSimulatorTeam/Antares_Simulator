#define BOOST_TEST_MODULE test save link properties.ini

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <string>

#include <study.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

fs::path getAntaresRootFromCurrentFolder(const fs::path & path)
{
	// Goes up incrementaly and search for a ".git" directory, meaning it has reached the Antares
	// root directory.
	fs::path path_to_return = path;
	auto it = path.end();
	if (it->string() == "")
		it--;
	for (; it != path.begin(); --it)
	{
		bool gitHiddenDirFound = false;
		for (auto const& dir_entry : fs::directory_iterator{ path_to_return })
		{
			fs::path entry = dir_entry.path().filename();
			if (dir_entry.path().filename() == ".git")
			{
				gitHiddenDirFound = true;
				break;
			}
		}
		if (gitHiddenDirFound)
			break;
		path_to_return = path_to_return.parent_path();
	}

	return path_to_return;
}

fs::path getSourcefolderFromRoot(const fs::path& rootPath)
{
	fs::path path_to_return(rootPath);
	path_to_return.append("src").append("tests").append("src").append("libs").append("antares").append("study").append("area");
	if (!exists(path_to_return) || !is_directory(path_to_return))
		return rootPath;
	return path_to_return;
}

fs::path getOutputfolderFromCurrentDir(const fs::path & path)
{
	fs::path antaresRootPath = getAntaresRootFromCurrentFolder(path);
	fs::path sourceFolder = getSourcefolderFromRoot(antaresRootPath);
	fs::path outputFolder = sourceFolder.append("out");
	if (!exists(outputFolder))
		create_directory(outputFolder);
	return outputFolder;
}

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

bool compare_ini_files(const fs::path & outputDir, string fileName, string fileNameRef)
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
	// Disable link's time-series dump
	link->data.resize(0, 0);

	fs::path current_dir(fs::current_path());
	fs::path output_dir = getOutputfolderFromCurrentDir(current_dir);


	BOOST_CHECK(AreaLinksSaveToFolder(area_1, output_dir.string().c_str()));

	BOOST_CHECK(compare_ini_files(output_dir, "properties.ini", "properties-ref-0.ini"));

	vector<string> filesToRemove = { "area 2.txt", "properties.ini" };
	clean_output(output_dir, filesToRemove);
}
