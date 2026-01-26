// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

// The following macro is used due to linking issues with <boost/test/unit_test.hpp>
#define CREATE_TMP_DIR_BASED_ON_TEST_NAME() \
    generateAndCreateDirName(boost::unit_test::framework::current_test_case().p_name);

std::filesystem::path generateAndCreateDirName(const std::string&);

std::filesystem::path createFolder(const std::string& path, const std::string& folder_name);
void createFile(const std::string& folder_path, const std::string& file_name);
void removeFolder(std::string& path, std::string& folder_name);
