#include "utils.h"
#include <boost/test/included/unit_test.hpp>

namespace fs = std::filesystem;

fs::path generateAndCreateDirName()
{
    std::string dir = boost::unit_test::framework::current_test_case().p_name;
    fs::path working_dir = fs::temp_directory_path() / dir;
    fs::remove(working_dir);
    fs::create_directories(working_dir);
    return working_dir;
}
