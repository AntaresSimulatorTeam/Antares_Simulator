#pragma once

#include <filesystem>

// The following macro is used due to linking issues with <boost/test/unit_test.hpp>
#define CREATE_TMP_DIR_BASED_ON_TEST_NAME() generateAndCreateDirName(boost::unit_test::framework::current_test_case().p_name);

std::filesystem::path generateAndCreateDirName(const std::string&);
