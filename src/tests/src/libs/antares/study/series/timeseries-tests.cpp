#define BOOST_TEST_MODULE "test time series"
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <fstream>

#include <include/antares/series/series.h>

using namespace Antares::Data;
using std::filesystem::temp_directory_path;

// =================
// The fixture
// =================
struct FixtureFull
{
    FixtureFull(const FixtureFull& f) = delete;
    FixtureFull(const FixtureFull&& f) = delete;
    FixtureFull& operator=(const FixtureFull& f) = delete;
    FixtureFull& operator=(const FixtureFull&& f) = delete;
    FixtureFull()
    {
    }

    std::string folder;

};

// ==================
// Tests section
// ==================

// Here, we need the "lightweight fixture"
BOOST_AUTO_TEST_SUITE(timeseries_tests)

BOOST_AUTO_TEST_SUITE_END()
