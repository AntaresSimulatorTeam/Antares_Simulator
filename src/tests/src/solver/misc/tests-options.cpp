#define BOOST_TEST_MODULE test misc

#include <filesystem>
#include <fstream>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "antares/solver/misc/options.h"

namespace fs = std::filesystem;

BOOST_AUTO_TEST_SUITE(check_study_folder)

BOOST_AUTO_TEST_CASE(study_folder_is_valid___no_exception_raised)
{
    fs::path studyFolder = fs::temp_directory_path() / "some-study-name";
    BOOST_CHECK_NO_THROW(checkStudyFolder(studyFolder.string()));
}

BOOST_AUTO_TEST_CASE(study_folder_is_empty___exception_raised)
{
    fs::path studyFolder; // Empty path
    std::string err_msg = "A study folder is required. Use '--help' for more information";
    BOOST_CHECK_EXCEPTION(checkStudyFolder(studyFolder.string()),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(study_folder_contains_non_ascii_chars___exception_raised)
{
    fs::path studyFolder = fs::temp_directory_path() / "I contain an accènt";
    std::string err_msg = "Study folder contains non ASCII chars : ";
    BOOST_CHECK_EXCEPTION(checkStudyFolder(studyFolder.string()),
                          std::runtime_error,
                          containsMessage(err_msg));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fix_study_folder)

BOOST_AUTO_TEST_CASE(sudy_folder_relative_path_is_converted_into_an_absolute_path)
{
    fs::path studyFolder(".");
    fs::path expectedPath = fs::absolute(fs::current_path());
    BOOST_CHECK_EQUAL(fixStudyFolder(studyFolder.string()), expectedPath.string());
}

BOOST_AUTO_TEST_CASE(study_folder_does_not_exist___exception_raised)
{
    fs::path studyFolder = fs::current_path() / "blah-blah"; // Does not exist
    std::string err_msg = "does not exist";
    BOOST_CHECK_EXCEPTION(fixStudyFolder(studyFolder.string()),
                          std::runtime_error,
                          containsMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(print_pid_to_disk)

std::string getPIDfromFile(fs::path& filePath)
{
    std::string pid_to_return;
    std::ifstream pidFile(filePath.string());
    if (pidFile.is_open())
    {
        while (pidFile.good())
        {
            getline(pidFile, pid_to_return);
        }
    }
    pidFile.close();
    return pid_to_return;
}

BOOST_AUTO_TEST_CASE(ask_for_pid_printed_to_disk___right_pid_is_printed)
{
    fs::path pidFilePath = fs::temp_directory_path() / "pid.txt";
    Settings settings;
    settings.PID = pidFilePath.string();

    printPIDtoDisk(settings);

    std::string pid = getPIDfromFile(pidFilePath);
    BOOST_CHECK_EQUAL(std::stoi(pid), getpid());

    fs::remove(pidFilePath); // Tear-down
}

BOOST_AUTO_TEST_CASE(ask_for_pid_printed_to_an_non_existing_file___exception_raised)
{
    fs::path nonExistingFile = fs::current_path() / "blah-blah" / "pid.txt";
    Settings settings;
    settings.PID = nonExistingFile.string();

    std::string err_msg = "Impossible to write pid file";
    BOOST_CHECK_EXCEPTION(printPIDtoDisk(settings), std::runtime_error, containsMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()
