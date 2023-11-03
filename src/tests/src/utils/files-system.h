#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>

// The following macro is used due to linking issues with <boost/test/unit_test.hpp>
#define CREATE_TMP_DIR_BASED_ON_TEST_NAME() generateAndCreateDirName(boost::unit_test::framework::current_test_case().p_name);

std::filesystem::path generateAndCreateDirName(const std::string&);

void createFolder(const std::string& path, const std::string& folder_name);
void createFile(const std::string& folder_path, const std::string& file_name);
void removeFolder(std::string& path, std::string& folder_name);
