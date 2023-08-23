#define BOOST_TEST_MODULE output folder
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib> // setenv

#include <study.h>

// Test the generation of folder output names
// We expect something like outputRoot/20221230-0914eco-test
// If the directory/archive already exists, then fall back to outputRoot/20221230-0914eco-test-2,
// etc.

using namespace Antares::Data;
namespace fs = std::filesystem;

struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;
    Fixture() : outputRoot(fs::temp_directory_path() / "output")
    {
        // We need to set a 0 offset in the timezone. Otherwise, function
        // DateTime::TimestampToString (a wrapper around ::strftime) will add 1 or 2 hours. We don't
        // want this. See https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
#ifdef YUNI_OS_MSVC
        _putenv_s("TZ", "GMT");
#else
        setenv("TZ", "GMT", 1);
#endif
        // Create outputRoot / output
        fs::create_directory(outputRoot);
    }

    ~Fixture()
    {
        // Clean up our mess to avoid side-effects
        fs::remove_all(outputRoot);
    }
    fs::path outputRoot;
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(economy_legacyfiles_emptylabel, Fixture)
{
    StudyMode mode = stdmEconomy;
    ResultFormat fmt = legacyFilesDirectories;
    const YString label = "";
    const Yuni::sint64 startTime = 1;
    const YString expectedOutput = (outputRoot / "19700101-0000eco").string();
    const YString actualOutput
      = StudyCreateOutputPath(mode, fmt, outputRoot.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);
}

BOOST_FIXTURE_TEST_CASE(economy_legacyfiles_label_now, Fixture)
{
    StudyMode mode = stdmEconomy;
    ResultFormat fmt = legacyFilesDirectories;
    const YString label = "test";
    const Yuni::sint64 startTime = 1672391667;
    const YString expectedOutput = (outputRoot / "20221230-0914eco-test").string();
    const YString actualOutput
      = StudyCreateOutputPath(mode, fmt, outputRoot.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);
}

BOOST_FIXTURE_TEST_CASE(adequacy_legacyfiles_label_now, Fixture)
{
    StudyMode mode = stdmAdequacy;
    ResultFormat fmt = legacyFilesDirectories;
    const YString label = "test";
    const Yuni::sint64 startTime = 1672391667;
    const YString expectedOutput = (outputRoot / "20221230-0914adq-test").string();
    const YString actualOutput
      = StudyCreateOutputPath(mode, fmt, outputRoot.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);

    fs::create_directory(outputRoot / "20221230-0914adq-test");
    const YString expectedOutput_suffix = (outputRoot / "20221230-0914adq-test-2").string();
    const YString actualOutput_suffix
      = StudyCreateOutputPath(mode, fmt, outputRoot.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput_suffix, expectedOutput_suffix);
}

BOOST_FIXTURE_TEST_CASE(adequacy_zip_label_now, Fixture)
{
    StudyMode mode = stdmAdequacy;
    ResultFormat fmt = zipArchive;
    const YString label = "test";
    const Yuni::sint64 startTime = 1672391667;
    const YString expectedOutput = (outputRoot / "20221230-0914adq-test").string();
    const YString actualOutput
      = StudyCreateOutputPath(mode, fmt, outputRoot.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput, expectedOutput);

    std::ofstream zip_file(outputRoot / "20221230-0914adq-test.zip");
    zip_file << "I am a zip file. Well, not really.";

    const YString expectedOutput_suffix = (outputRoot / "20221230-0914adq-test-2").string();
    const YString actualOutput_suffix
      = StudyCreateOutputPath(mode, fmt, outputRoot.string(), label, startTime);
    BOOST_CHECK_EQUAL(actualOutput_suffix, expectedOutput_suffix);
}

BOOST_AUTO_TEST_SUITE_END()
