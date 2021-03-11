#define BOOST_TEST_MODULE playlist tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <antares/study/parameters.h>
#include <antares/study/load-options.h>

#include <iostream>
#include <fstream>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace bf = boost::filesystem;

/*
   Small wrapper to ensure temporary files are removed when we no longer need them

   NOTE: This is needed because class Antares::Data::Parameter cannot read
   from an arbitrary stream, only a file specified by its path. This should be
   changed.
*/

class TemporaryFile
{
private:
    bf::path mPath;

public:
    std::string path() const
    {
        return std::string(mPath.native());
    }

    TemporaryFile(std::string content)
    {
        mPath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        std::ofstream ofs(path());
        // dump content into temporary file
        ofs << content;
    }

    ~TemporaryFile()
    {
        bf::remove(mPath);
    }
};

BOOST_AUTO_TEST_SUITE(playlist_update)

BOOST_AUTO_TEST_CASE(read_two_active_years_v800)
{
    using namespace Antares::Data;
    Parameters p;
    TemporaryFile ini_file(
      R"([general]
mode = Adequacy
horizon = 2000
nbyears = 5
simulation.start = 1
simulation.end = 365
january.1st = Monday
first-month-in-year = january
first.weekday = Monday
leapyear = false
year-by-year = false
derated = false
custom-scenario = false
user-playlist = true

[playlist]
playlist_reset = false
playlist_year += 0
playlist_year += 1)");

    StudyLoadOptions loadOpt;
    loadOpt.nbYears = 5;
    const bool loadSuccessful = p.loadFromFile(ini_file.path(), 800, loadOpt);
    BOOST_TEST(loadSuccessful);

    BOOST_TEST(p.userPlaylist);
    BOOST_TEST(p.nbYears == 5);

    BOOST_TEST(p.mode == StudyMode::stdmAdequacy);

    BOOST_TEST(p.yearsFilter.size() == 5);
    size_t idx = 0;
    for (bool v : {true, true, false, false, false})
        BOOST_TEST(p.yearsFilter[idx++] == v);

    p.prepareForSimulation(loadOpt);
    BOOST_TEST(p.effectiveNbYears == 2);
}

BOOST_AUTO_TEST_CASE(read_two_active_years_v810)
{
    using namespace Antares::Data;
    Parameters p;
    TemporaryFile ini_file(
      R"([general]
mode = Adequacy
horizon = 2000
nbyears = 5
simulation.start = 1
simulation.end = 365
january.1st = Monday
first-month-in-year = january
first.weekday = Monday
leapyear = false
year-by-year = false
derated = false
custom-scenario = false
user-playlist = true

[playlist]
playlist_active_years=0,1)");

    StudyLoadOptions loadOpt;
    loadOpt.nbYears = 5;
    const bool loadSuccessful = p.loadFromFile(ini_file.path(), 810, loadOpt);
    BOOST_TEST(loadSuccessful);

    BOOST_TEST(p.userPlaylist);
    BOOST_TEST(p.nbYears == 5);

    BOOST_TEST(p.mode == StudyMode::stdmAdequacy);

    BOOST_TEST(p.yearsFilter.size() == 5);
    size_t idx = 0;
    for (bool v : {true, true, false, false, false})
        BOOST_TEST(p.yearsFilter[idx++] == v);

    p.prepareForSimulation(loadOpt);
    BOOST_TEST(p.effectiveNbYears == 2);
}

BOOST_AUTO_TEST_CASE(read_weights_v810)
{
    using namespace Antares::Data;
    Parameters p;
    TemporaryFile ini_file(
      R"([general]
mode = Adequacy
horizon = 2000
nbyears = 5
simulation.start = 1
simulation.end = 365
january.1st = Monday
first-month-in-year = january
first.weekday = Monday
leapyear = false
year-by-year = false
derated = false
custom-scenario = false
user-playlist = true

[playlist]
playlist_active_years=0,1
playlist_weight_year_4=2
playlist_weight_year_0=1.1
playlist_weight_year_1=3.3
)");

    StudyLoadOptions loadOpt;
    loadOpt.nbYears = 5;
    const bool loadSuccessful = p.loadFromFile(ini_file.path(), 810, loadOpt);
    BOOST_TEST(loadSuccessful);

    BOOST_TEST(p.userPlaylist);
    BOOST_TEST(p.nbYears == 5);

    BOOST_TEST(p.mode == StudyMode::stdmAdequacy);
    BOOST_TEST(p.yearsWeight.size() == 5);

    size_t idx = 0;
    for (float w : {1.1f, 3.3f, 1.f, 1.f, 2.f})
        BOOST_TEST(p.yearsWeight[idx++] == w, tt::tolerance(1e-6));
}

BOOST_AUTO_TEST_SUITE_END()
