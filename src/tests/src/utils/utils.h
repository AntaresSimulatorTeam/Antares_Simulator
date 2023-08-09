#pragma once

#include <filesystem>

#define CREATE_TMP_DIR_BASED_ON_TEST_NAME() generateAndCreateDirName(boost::unit_test::framework::current_test_case().p_name);

std::filesystem::path generateAndCreateDirName(std::string);
