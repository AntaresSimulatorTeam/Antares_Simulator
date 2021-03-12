#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"

#include <antares/study/parameters.h>
#include <antares/study/load-options.h>

#include <iostream>
#include <fstream>
#include <streambuf>

/* GCC */
#ifdef __GNUC__
#if __GNUC__ < 8
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
/* Other compilers */
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

/*
   Small wrapper to ensure temporary files are removed when we no longer need them

   NOTE: This is needed because class Antares::Data::Parameter cannot read
   from an arbitrary stream, only a file specified by its path. This should be
   changed.
*/

class TemporaryFile
{
private:
    const fs::path mPath;

public:
    TemporaryFile() : mPath(fs::temp_directory_path() / "generaldata.ini")
    {
    }

    void write(const std::string& content)
    {
        std::ofstream ofs(mPath);
        // dump content into temporary file
        ofs << content;
    }

    std::string read()
    {
        std::ifstream in(mPath);
        if (in)
        {
            return (
              std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
        }
        return "error";
    }

    ~TemporaryFile()
    {
        fs::remove(mPath);
    }

    std::string path() const
    {
        return mPath.string();
    }
};

namespace {
  const std::size_t& npos = std::string::npos;
}

TEST_CASE("Read v800 active years", "[v800]")
{
    using namespace Antares::Data;
    Parameters p;
    TemporaryFile ini_file;
    ini_file.write(
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
    TemporaryFile ini_file;
    ini_file.write(
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
    TemporaryFile ini_file;
    ini_file.write(R"([general]
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

TEST_CASE("Write v810 active years", "[v810]")
{
    using namespace Antares::Data;
    Parameters p;
    p.nbYears = 5;
    p.yearsFilter = {true, true, false, true, true};
    p.yearsWeight = std::vector<float>(5, 1.f);
    p.userPlaylist = true;
    p.mode = stdmEconomy;

    p.variablesPrintInfo.add(new VariablePrintInfo("name", 0, 0, 0));
    p.include.unfeasibleProblemBehavior = UnfeasibleProblemBehavior::ERROR_MPS;

    TemporaryFile ini_file;
    p.saveToFile(ini_file.path(), 810);
    const std::string content = ini_file.read();
    // I/O error
    REQUIRE(content != "error");

    REQUIRE(content.find("playlist_active_years = 0,1,3,4") != ::npos);
    // All weights are 1, they should not be written
    REQUIRE(content.find("playlist_weight_year") == ::npos);
}

TEST_CASE("Write v810 weights", "[v810]")
{
    using namespace Antares::Data;
    Parameters p;
    p.nbYears = 5;
    p.yearsFilter = std::vector<bool>(5);
    p.yearsWeight = std::vector<float>{1.f, 2.f, 3.f, 4.f, 0.f};
    p.userPlaylist = true;
    p.mode = stdmEconomy;
    p.variablesPrintInfo.add(new VariablePrintInfo("name", 0, 0, 0));
    p.include.unfeasibleProblemBehavior = UnfeasibleProblemBehavior::ERROR_MPS;

    TemporaryFile ini_file;
    p.saveToFile(ini_file.path(), 810);
    const std::string content = ini_file.read();
    // I/O error
    REQUIRE(content != "error");
    // Weight 0 is 1.f, it should not be written
    REQUIRE(content.find("playlist_weight_year_0") == ::npos);
    // Weight 1 is 2.f, it should be written
    REQUIRE(content.find("playlist_weight_year_1") != ::npos);
}
