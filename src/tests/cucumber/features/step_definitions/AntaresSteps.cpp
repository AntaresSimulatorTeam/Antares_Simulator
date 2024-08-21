#include <cucumber-cpp/autodetect.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

#include "antares/api/SimulationResults.h"
#include "antares/api/solver.h"

using cucumber::ScenarioScope;
using namespace std;

struct AntaresCtx
{
    string studyPath;
    Antares::API::SimulationResults simulationResults;
};

GIVEN("^the study path is \"(.+)\"$")
{
    REGEX_PARAM(string, studyPath);
    ScenarioScope<AntaresCtx> context;
    context->studyPath = std::filesystem::current_path().string()
                         + "/src/tests/resources/Antares_Simulator_Tests/" + studyPath;
}

WHEN("^I run antares simulator")
{
    ScenarioScope<AntaresCtx> context;
    context->simulationResults = Antares::API::PerformSimulation(context->studyPath);
}

THEN("^the simulation succeeds")
{
    ScenarioScope<AntaresCtx> context;
    ASSERT_FALSE(context->simulationResults.error.has_value());
}

THEN("^the simulation fails")
{
    ScenarioScope<AntaresCtx> context;
    ASSERT_TRUE(context->simulationResults.error.has_value());
}

double read(string fileName, string tag)
{
    ScenarioScope<AntaresCtx> context;
    std::ifstream file(context->simulationResults.simulationPath.string() + "/" + fileName);
    std::string str;
    while (std::getline(file, str))
    {
        if (str.starts_with(tag))
        {
            return stod(str.substr(tag.size() + 3));
        }
    }
    // TODO : would like to use :
    // FAIL() << "Could not find tag " << tag << " in file " << fileName;
    // but FAIL() can only be used in functions that return void, as documented in
    // https://google.github.io/googletest/reference/assertions.html
    return 0;
}

THEN("^the expected value of the annual system cost is (.*)")
{
    REGEX_PARAM(double, expected);
    double actual = read("annualSystemCost.txt", "EXP");
    ASSERT_NEAR(actual, expected, expected * 0.001);
}

THEN("^the minimum annual system cost is (.*)")
{
    REGEX_PARAM(double, expected);
    double actual = read("annualSystemCost.txt", "MIN");
    ASSERT_NEAR(actual, expected, expected * 0.001);
}

THEN("^the maximum annual system cost is (.*)")
{
    REGEX_PARAM(double, expected);
    double actual = read("annualSystemCost.txt", "MAX");
    ASSERT_NEAR(actual, expected, expected * 0.001);
}
