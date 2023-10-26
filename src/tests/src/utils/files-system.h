#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>

using stringT = std::string;

// The following macro is used due to linking issues with <boost/test/unit_test.hpp>
#define CREATE_TMP_DIR_BASED_ON_TEST_NAME() generateAndCreateDirName(boost::unit_test::framework::current_test_case().p_name);

std::filesystem::path generateAndCreateDirName(const std::string&);

void createFolder(const stringT& path, const stringT& folder_name);
void createFile(const stringT& folder_path, const stringT& file_name);
void removeFolder(stringT& path, stringT& folder_name);
