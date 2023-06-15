#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include "container.h"

#define SEP Yuni::IO::Separator

using namespace std;
using namespace Antares::Data;

std::string getFolder()
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    return tmpDir.string();
}


// =================
// The fixture
// =================
struct Fixture
{
    Fixture(const Fixture & f) = delete;
    Fixture(const Fixture && f) = delete;
    Fixture & operator= (const Fixture & f) = delete;
    Fixture& operator= (const Fixture && f) = delete;
    Fixture() = default;
    ~Fixture()
    {
    }

    std::string folder = getFolder();

};


// ==================
// Tests section
// ==================

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_SUITE_END()
