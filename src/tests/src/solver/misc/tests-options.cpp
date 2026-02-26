#define BOOST_TEST_MODULE test misc

#include <filesystem>
#include <fstream>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "antares/solver/misc/options.h"

using namespace Antares::Data;
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

#if defined(_WIN32)
BOOST_AUTO_TEST_CASE(on_Win__study_folder_contains_non_ascii_chars___exception_raised)
{
    fs::path studyFolder = fs::temp_directory_path() / "I contain an accént";
    std::string err_msg = "Study folder contains non ASCII chars : ";
    BOOST_CHECK_EXCEPTION(checkStudyFolder(studyFolder.string()),
                          std::runtime_error,
                          containsMessage(err_msg));
}
#endif
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fix_study_folder)

std::string removeTrailingSeparator(const std::string& path)
{
    std::string to_return = path;
    if (to_return.back() == '/' || to_return.back() == '\\')
    {
        to_return.pop_back();
    }
    return to_return;
}

BOOST_AUTO_TEST_CASE(sudy_folder_relative_path_is_converted_into_an_absolute_path)
{
    fs::path studyFolder(".");
    std::string expectedPath = fs::absolute(fs::current_path()).string();
    std::string fixedStudyFolder = fixStudyFolder(studyFolder.string());
    BOOST_CHECK_EQUAL(removeTrailingSeparator(fixedStudyFolder), expectedPath);
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
            std::getline(pidFile, pid_to_return);
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

struct CreateOptionsAndSettings
{
    Settings settings;
    StudyLoadOptions options;
};

BOOST_FIXTURE_TEST_SUITE(check_and_fix_settings_and_options, CreateOptionsAndSettings)

BOOST_AUTO_TEST_CASE(options_give_to_settings_the_simulation_name)
{
    BOOST_CHECK(options.simulationName.empty()); // Value by default

    options.simulationName = "my-simulation";
    checkAndCorrectSettingsAndOptions(settings, options);
    BOOST_CHECK(settings.simulationName == options.simulationName);
}

BOOST_AUTO_TEST_CASE(options_nb_of_threads_forces_parallel_mode)
{
    BOOST_CHECK(!options.forceParallel); // Value by default

    options.maxNbYearsInParallel = 3;
    checkAndCorrectSettingsAndOptions(settings, options);
    BOOST_CHECK(options.forceParallel);
}

BOOST_AUTO_TEST_CASE(parallel_optione_incompatible)
{
    options.enableParallel = true;
    options.forceParallel = true;
    std::string err_msg = "Options --parallel and --force-parallel are incompatible";
    BOOST_CHECK_EXCEPTION(checkAndCorrectSettingsAndOptions(settings, options),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(optimization_range_set_to_unknown___exception_raised)
{
    settings.simplexOptimRange = "unknown_opt_range";
    std::string err_msg = "Invalid command line value for --optimization-range ('day' or 'week' "
                          "expected)";
    BOOST_CHECK_EXCEPTION(checkAndCorrectSettingsAndOptions(settings, options),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(optimization_range_set_to_day)
{
    settings.simplexOptimRange = "DAy"; // Whatever the case, will be downcased
    checkAndCorrectSettingsAndOptions(settings, options);
    BOOST_CHECK_EQUAL(options.simplexOptimizationRange, sorDay);
}

BOOST_AUTO_TEST_CASE(optimization_range_set_to_week)
{
    settings.simplexOptimRange = "wEeK"; // Whatever the case, will be downcased
    checkAndCorrectSettingsAndOptions(settings, options);
    BOOST_CHECK_EQUAL(options.simplexOptimizationRange, sorWeek);
}

// settings.noOutput && settings.forceZipOutput
BOOST_AUTO_TEST_CASE(output_settings_are_incompatible___exception_raised)
{
    settings.noOutput = true;
    settings.forceZipOutput = true;
    std::string err_msg = "no-output and zip-output options are incompatible";
    BOOST_CHECK_EXCEPTION(checkAndCorrectSettingsAndOptions(settings, options),
                          std::runtime_error,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(settings_reset)

BOOST_AUTO_TEST_CASE(reset_restores_default_values)
{
    Settings settings;

    // Set non-default values
    settings.studyFolder = "some-folder";
    settings.simulationName = "some-simulation";
    settings.commentFile = "some-comment-file";
    settings.simplexOptimRange = "week";
    settings.PID = "some-pid-path";

    settings.ignoreLoadingErrors = true;
    settings.ignoreConstraints = true;
    settings.tsGeneratorsOnly = true;
    settings.noOutput = true;
    settings.forceZipOutput = true;

    settings.solverOptions.linearSolver = "custom-linear";
    settings.solverOptions.linearSolverParameters = "custom-params";
    settings.solverOptions.lpSolverParamOptim1 = "optim1";
    settings.solverOptions.lpSolverParamOptim2 = "optim2";
    settings.solverOptions.useOptim1BasisInNextWeek = false;
    settings.solverOptions.useOptim1BasisInOptim2 = false;
    settings.solverOptions.quadraticSolver = "custom-quadratic";
    settings.solverOptions.quadraticSolverParameters = "quadratic-params";
    settings.solverOptions.solverLogs = true;

    settings.reset();

    BOOST_CHECK(settings.studyFolder.empty());
    BOOST_CHECK(settings.simulationName.empty());
    BOOST_CHECK(settings.commentFile.empty());
    BOOST_CHECK(settings.simplexOptimRange.empty());
    BOOST_CHECK(settings.PID.empty());

    BOOST_CHECK(!settings.ignoreLoadingErrors);
    BOOST_CHECK(!settings.ignoreConstraints);
    BOOST_CHECK(!settings.tsGeneratorsOnly);
    BOOST_CHECK(!settings.noOutput);
    BOOST_CHECK(!settings.forceZipOutput);

    BOOST_CHECK_EQUAL(settings.solverOptions.linearSolver, std::string("sirius"));
    BOOST_CHECK(settings.solverOptions.linearSolverParameters.empty());
    BOOST_CHECK(settings.solverOptions.lpSolverParamOptim1.empty());
    BOOST_CHECK(settings.solverOptions.lpSolverParamOptim2.empty());
    BOOST_CHECK(settings.solverOptions.useOptim1BasisInNextWeek);
    BOOST_CHECK(settings.solverOptions.useOptim1BasisInOptim2);
    BOOST_CHECK_EQUAL(settings.solverOptions.quadraticSolver, std::string("sirius"));
    BOOST_CHECK(settings.solverOptions.quadraticSolverParameters.empty());
    BOOST_CHECK(!settings.solverOptions.solverLogs);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(create_parser)

BOOST_AUTO_TEST_CASE(default_parser_resets_settings_and_keeps_defaults)
{
    Settings settings;
    settings.studyFolder = "folder";
    settings.simulationName = "simulation";
    settings.commentFile = "comment";
    settings.simplexOptimRange = "day";
    settings.noOutput = true;
    settings.forceZipOutput = true;

    StudyLoadOptions options;

    auto parser = CreateParser(settings, options);
    BOOST_CHECK(parser);

    // CreateParser must reset settings
    BOOST_CHECK(settings.studyFolder.empty());
    BOOST_CHECK(settings.simulationName.empty());
    BOOST_CHECK(settings.commentFile.empty());
    BOOST_CHECK(settings.simplexOptimRange.empty());
    BOOST_CHECK(settings.PID.empty());

    BOOST_CHECK(!settings.ignoreLoadingErrors);
    BOOST_CHECK(!settings.ignoreConstraints);
    BOOST_CHECK(!settings.tsGeneratorsOnly);
    BOOST_CHECK(!settings.noOutput);
    BOOST_CHECK(!settings.forceZipOutput);

    // Solver options should be at their defaults
    BOOST_CHECK_EQUAL(settings.solverOptions.linearSolver, std::string("sirius"));
    BOOST_CHECK(settings.solverOptions.linearSolverParameters.empty());
    BOOST_CHECK(settings.solverOptions.lpSolverParamOptim1.empty());
    BOOST_CHECK(settings.solverOptions.lpSolverParamOptim2.empty());
    BOOST_CHECK(settings.solverOptions.useOptim1BasisInNextWeek);
    BOOST_CHECK(settings.solverOptions.useOptim1BasisInOptim2);
    BOOST_CHECK_EQUAL(settings.solverOptions.quadraticSolver, std::string("sirius"));
    BOOST_CHECK(settings.solverOptions.quadraticSolverParameters.empty());
    BOOST_CHECK(!settings.solverOptions.solverLogs);
}

BOOST_AUTO_TEST_CASE(parser_can_be_created)
{
    Settings settings;
    StudyLoadOptions options;

    auto parser = CreateParser(settings, options);
    BOOST_CHECK(parser);
}

BOOST_AUTO_TEST_SUITE_END()
