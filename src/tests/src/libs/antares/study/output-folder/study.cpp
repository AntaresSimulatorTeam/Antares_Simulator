#define BOOST_TEST_MODULE output folder

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib> // setenv

#include <study.h>

// Test the generation of folder output names
// We expect something like 20221230-0914eco-test
// If the directory/archive already exists, then fall back to 20221230-0914eco-test-2, etc.

using namespace Antares::Data;
namespace fs = std::filesystem;

static void deleteDirectoryContents(const fs::path& dir_path)
{
    for (const auto& entry : fs::directory_iterator(dir_path))
        fs::remove_all(entry.path());
}

struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;
    Fixture() : tmp_path(fs::temp_directory_path())
    {
        // We need to set a 0 offset in the timezone. Otherwise, function
        // DateTime::TimestampToString (a wrapper around ::strftime) will add 1 or 2 hours. We don't
        // want this. See https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
        setenv("TZ", "GMT", 1);
    }

    ~Fixture()
    {
        // Clean up our mess to avoid side-effects
        deleteDirectoryContents(tmp_path / "output");
    }
    fs::path tmp_path;
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(economy_legacyfiles_emptylabel)
{
    StudyMode mode = stdmEconomy;
    ResultFormat fmt = legacyFilesDirectories;
    const YString label = "";
    const Yuni::sint64 startTime = 1;
    const YString expectedOutput = (tmp_path / "output" / "19700101-0000eco").string();
    const YString actualOutput
      = StudyCreateOutputName(mode, fmt, tmp_path.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);
}

BOOST_AUTO_TEST_CASE(economy_legacyfiles_label_now)
{
    StudyMode mode = stdmEconomy;
    ResultFormat fmt = legacyFilesDirectories;
    const YString label = "test";
    const Yuni::sint64 startTime = 1672391667;
    const YString expectedOutput = (tmp_path / "output" / "20221230-0914eco-test").string();
    const YString actualOutput
      = StudyCreateOutputName(mode, fmt, tmp_path.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);
}

BOOST_AUTO_TEST_CASE(adequacy_legacyfiles_label_now)
{
    StudyMode mode = stdmAdequacy;
    ResultFormat fmt = legacyFilesDirectories;
    const YString label = "test";
    const Yuni::sint64 startTime = 1672391667;
    const YString expectedOutput = (tmp_path / "output" / "20221230-0914adq-test").string();
    const YString actualOutput
      = StudyCreateOutputName(mode, fmt, tmp_path.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);

    fs::create_directory(tmp_path / "output" / "20221230-0914adq-test");
    const YString expectedOutput_suffix
      = (tmp_path / "output" / "20221230-0914adq-test-2").string();
    const YString actualOutput_suffix
      = StudyCreateOutputName(mode, fmt, tmp_path.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput_suffix, expectedOutput_suffix);
}

BOOST_AUTO_TEST_CASE(adequacy_zip_label_now)
{
    StudyMode mode = stdmAdequacy;
    ResultFormat fmt = zipArchive;
    const YString label = "test";
    const Yuni::sint64 startTime = 1672391667;
    const YString expectedOutput = (tmp_path / "output" / "20221230-0914adq-test").string();
    const YString actualOutput
      = StudyCreateOutputName(mode, fmt, tmp_path.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);

    std::ofstream zip_file(tmp_path / "output" / "20221230-0914adq-test.zip");
    zip_file << "I am a zip file. Well, not really.";

    const YString expectedOutput_suffix
      = (tmp_path / "output" / "20221230-0914adq-test-2").string();
    const YString actualOutput_suffix
      = StudyCreateOutputName(mode, fmt, tmp_path.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput_suffix, expectedOutput_suffix);
}

BOOST_AUTO_TEST_SUITE_END()
