#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"

#include <antares/study/parameters.h>
#include <antares/study/load-options.h>

#include <iostream>
#include <fstream>
#include <filesystem>

/*
   Small wrapper to ensure temporary files are removed when we no longer need them

   NOTE: This is needed because class Antares::Data::Parameter cannot read
   from an arbitrary stream, only a file specified by its path. This should be
   changed.
*/

class TemporaryFile
{
private:
    std::filesystem::path mPath;

public:
    TemporaryFile(std::string content)
    {
        mPath = std::filesystem::temp_directory_path() / "generaldata.ini";
        std::ofstream ofs(mPath);
        // dump content into temporary file
        ofs << content;
    }

    ~TemporaryFile()
    {
        std::filesystem::remove(mPath);
    }

    std::string path() const
    {
        return mPath.string();
    }
};

TEST_CASE("Read v800 active years", "[v800]")
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
    REQUIRE(loadSuccessful);

    REQUIRE(p.userPlaylist);
    REQUIRE(p.nbYears == 5);

    REQUIRE(p.mode == StudyMode::stdmAdequacy);

    REQUIRE(p.yearsFilter.size() == 5);
    size_t idx = 0;
    for (bool v : {true, true, false, false, false})
        REQUIRE(p.yearsFilter[idx++] == v);

    p.prepareForSimulation(loadOpt);
    REQUIRE(p.effectiveNbYears == 2);
}

TEST_CASE("Read v810 active years", "[v810]")
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
    REQUIRE(loadSuccessful);

    REQUIRE(p.userPlaylist);
    REQUIRE(p.nbYears == 5);

    REQUIRE(p.mode == StudyMode::stdmAdequacy);

    REQUIRE(p.yearsFilter.size() == 5);
    size_t idx = 0;
    for (bool v : {true, true, false, false, false})
    {
        REQUIRE(p.yearsFilter[idx++] == v);
    }

    p.prepareForSimulation(loadOpt);
    REQUIRE(p.effectiveNbYears == 2);
}

TEST_CASE("Read v810 weights", "[v810]")
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
    REQUIRE(loadSuccessful);

    REQUIRE(p.userPlaylist);
    REQUIRE(p.nbYears == 5);

    REQUIRE(p.mode == StudyMode::stdmAdequacy);
    REQUIRE(p.yearsWeight.size() == 5);

    size_t idx = 0;
    for (float w : {1.1f, 3.3f, 1.f, 1.f, 2.f})
        CHECK(p.yearsWeight[idx++] == Approx(w).margin(1e-6));
}
