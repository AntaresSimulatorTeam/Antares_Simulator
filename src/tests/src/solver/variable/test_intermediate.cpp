// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test time series"

#define WIN32_LEAN_AND_MEAN

#include <numeric>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/variable/economy/STStorageLevelsByCluster.h"
#include "antares/solver/variable/economy/STStorageWithdrawalByCluster.h"
#include "antares/solver/variable/storage/intermediate.h"
#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"

#include "dummyVariable.h"

constexpr double TOLERANCE = 1.e-6;
using Antares::Constants::nbHoursInAWeek;

std::unique_ptr<Antares::Data::Study> studyHelper(unsigned FirstDay, unsigned LastDay)
{
    auto study = std::make_unique<Antares::Data::Study>();
    study->parameters.simulationDays.first = FirstDay;
    study->parameters.simulationDays.end = LastDay;
    study->parameters.nbYears = 5;
    study->maxNbYearsInParallel = 5;
    study->initializeRuntimeInfos();

    study->parameters.resetPlaylist(study->parameters.nbYears);
    {
        using namespace Antares::Solver::Variable;
        Antares::Data::VariablePrintInfo v(Category::FileLevel::va, Category::DataLevel::area);
        v.setMaxColumns(4); // EXP STD MIN MAX
        study->parameters.variablesPrintInfo.add("for_some_reason_this_name_has_no_consequences",
                                                 v);
    }

    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    study->parameters.variablesPrintInfo.computeMaxColumnsCountInReports({});

    return study;
}

template<unsigned FirstDay, unsigned LastDay>
struct StudyFixture
{
    StudyFixture():
        study(studyHelper(FirstDay, LastDay)),
        writer(durationCollector),
        survey(*study,
               study->parameters.variablesPrintInfo.getTotalMaxColumnsCount(),
               "out",
               writer)
    {
    }

    void run(Antares::Solver::Variable::DummyVariable& variable)
    {
        variable.initializeFromStudy(*study);

        std::map<unsigned int, unsigned int> numSpaceToYear{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};

        unsigned int nbYearsForCurrentSummary = study->parameters.nbYears;
        for (auto [numSpace, year]: numSpaceToYear)
        {
            variable.yearBegin(year, numSpace);
            variable.computeSummary(year, nbYearsForCurrentSummary);
        }

        survey.data.columnIndex = 0;
        using namespace Antares::Solver::Variable;
        variable.buildSurveyReport(survey,
                                   Category::DataLevel::area,
                                   Category::FileLevel::va,
                                   Category::hourly);
    }

    Benchmarking::DurationCollector durationCollector;

    std::unique_ptr<Antares::Data::Study> study;
    Antares::Solver::InMemoryWriter writer;
    Antares::Solver::Variable::SurveyResults survey;
};

using FullYearStudyFixture = StudyFixture<0, 365>;
using PartialYearStudyFixture = StudyFixture<0, 192>;

BOOST_AUTO_TEST_SUITE(intermediate_suite)

BOOST_FIXTURE_TEST_CASE(averageFromHourlyFullYear, FullYearStudyFixture)
{
    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);
    intermediate[0] = 10; // hour 0
    intermediate[1] = 20; // hour 1
    intermediate.computeAveragesForCurrentYearFromHourlyResults();

    constexpr int nbHoursInYear = 8736;
    BOOST_CHECK_EQUAL(study->runtime.rangeLimits.hour[Antares::Data::rangeCount], nbHoursInYear);

    BOOST_CHECK_CLOSE(intermediate.year, (10. + 20.) / nbHoursInYear, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.week[0], (10. + 20.) / nbHoursInAWeek, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.month[0], (10. + 20.) / (31 * 24), TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.day[0], (10. + 20.) / 24, TOLERANCE);
}

BOOST_FIXTURE_TEST_CASE(averageFromHourlyPartialYear, PartialYearStudyFixture)
{
    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);
    intermediate[0] = 10; // hour 0
    intermediate[1] = 20; // hour 1
    intermediate.computeAveragesForCurrentYearFromHourlyResults();

    constexpr int nbWeeks = 27; // std::floor(192 / 7);
    const int nbHoursInYear = nbWeeks * nbHoursInAWeek;
    BOOST_CHECK_EQUAL(study->runtime.rangeLimits.week[Antares::Data::rangeCount], nbWeeks);
    BOOST_CHECK_EQUAL(study->runtime.rangeLimits.hour[Antares::Data::rangeCount], nbHoursInYear);

    BOOST_CHECK_CLOSE(intermediate.year, (10. + 20.) / nbHoursInYear, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.week[0], (10. + 20.) / nbHoursInAWeek, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.month[0], (10. + 20.) / (31 * 24), TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.day[0], (10. + 20.) / 24, TOLERANCE);
}

// This test illustrates how to aggregate hourly values to daily values then format to a 2 column
// report (CSV-like file)
BOOST_FIXTURE_TEST_CASE(intermediate_buildAnnualSurveyReport_to_surveyresults, FullYearStudyFixture)
{
    using namespace Antares::Solver::Variable;

    const auto getIntermediate = [&](double value)
    {
        IntermediateValues intermediate;
        intermediate.initializeFromStudy(*study);

        // Fill first day hourly values with 1.0, others remain 0.0
        for (unsigned int h = 0; h != HOURS_PER_DAY; ++h)
        {
            intermediate[h] = value;
        }
        return intermediate;
    };

    auto var1 = getIntermediate(1.);
    auto var2 = getIntermediate(2.);

    // Compute aggregated statistics (day/week/month/year)
    var1.computeStatisticsForTheCurrentYear();
    var2.computeStatisticsForTheCurrentYear();

    // Prepare SurveyResults for manual use
    survey.data.columnIndex = 0;

    bool nonApplicable[2] = {false, false};
    bool printed[2] = {true, true};
    survey.isCurrentVarNA = nonApplicable;
    survey.isPrinted = printed;

    struct VCardStub
    {
        enum
        {
            decimal = 0,
        };
    };

    // First column: variable 1
    survey.variableCaption = "variable1";
    survey.variableUnit = "unit1";
    var1.buildAnnualSurveyReport<VCardStub>(survey,
                                            Category::FileLevel::va,
                                            Category::Precision::daily);

    // Second column: variable 2
    survey.variableCaption = "variable2";
    survey.variableUnit = "unit2";
    var2.buildAnnualSurveyReport<VCardStub>(survey,
                                            Category::FileLevel::va,
                                            Category::Precision::daily);

    const std::string filename = "daily.txt";
    survey.data.filename = filename;

    // Write report to file
    survey.saveToFile(Category::DataLevel::area,
                      Category::FileLevel::va,
                      Category::Precision::daily);

    // Check the contents of the file
    const auto& map = writer.getMap();
    BOOST_REQUIRE(map.contains(filename));
    const auto& content = map.at(filename);

    // Captions from both buildAnnualSurveyReport calls must be present
    BOOST_CHECK_NE(content.find("system	daily			variable1	variable2"),
                   std::string::npos);
    BOOST_CHECK_NE(content.find("				unit1	unit2"), std::string::npos);
    // The value for the first day must be present
    BOOST_CHECK_NE(content.find("	1	01	JAN	24	48"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(hourToWeekAggregationWithStatistics, FullYearStudyFixture)
{
    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);

    const auto& range = study->runtime.rangeLimits;
    const auto& calendar = study->calendarOutput;

    const unsigned firstDay = range.day[Antares::Data::rangeBegin];
    const unsigned lastDay = range.day[Antares::Data::rangeEnd];
    const unsigned firstWeekIndex = calendar.days[firstDay].week;

    unsigned hourIndex = range.hour[Antares::Data::rangeBegin];
    double expectedWeekSum = 0.0;

    for (unsigned day = firstDay; day <= lastDay; ++day)
    {
        const bool isFirstWeek = (calendar.days[day].week == firstWeekIndex);

        for (unsigned h = 0; h != HOURS_PER_DAY; ++h)
        {
            const double value = isFirstWeek ? 1.0 : 0.0;
            intermediate[hourIndex] = value;
            expectedWeekSum += isFirstWeek ? value : 0.0;
            ++hourIndex;
        }
    }

    intermediate.computeStatisticsForTheCurrentYear();

    const unsigned firstMonthIndex = range.month[Antares::Data::rangeBegin];

    BOOST_CHECK_CLOSE(intermediate.day[firstDay], static_cast<double>(HOURS_PER_DAY), TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.week[firstWeekIndex], expectedWeekSum, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.month[firstMonthIndex], expectedWeekSum, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.year, expectedWeekSum, TOLERANCE);
}

BOOST_FIXTURE_TEST_CASE(stsWithdrawalTraitsUseStatisticsForCurrentYear, FullYearStudyFixture)
{
    using Antares::Solver::Variable::Economy::STStorageWithdrawalByClusterTraits;

    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);
    intermediate[0] = 10.;
    intermediate[1] = 20.;

    STStorageWithdrawalByClusterTraits::computeStats(intermediate);

    BOOST_CHECK_CLOSE(intermediate.day[0], 30., TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.year, 30., TOLERANCE);
}

BOOST_FIXTURE_TEST_CASE(stsLevelsTraitsUseAveragesFromHourlyResults, FullYearStudyFixture)
{
    using Antares::Solver::Variable::Economy::STStorageLevelsByClusterTraits;

    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);
    intermediate[0] = 10.;
    intermediate[1] = 20.;

    STStorageLevelsByClusterTraits::computeStats(intermediate);

    constexpr int kNbHoursInYear = 8736;
    BOOST_CHECK_CLOSE(intermediate.day[0], (10. + 20.) / 24., TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.year, (10. + 20.) / kNbHoursInYear, TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(aggregation)

class ConstantOverScenarios: public Antares::Solver::Variable::DummyVariable
{
public:
    double hourlyValue(unsigned int /*year*/, unsigned int hour) override
    {
        return hour + 1;
    }
};

enum
{
    EXP = 0,
    STD = 1,
    MIN = 2,
    MAX = 3
};

BOOST_FIXTURE_TEST_CASE(constant_over_scenarios, FullYearStudyFixture)
{
    ConstantOverScenarios variable;
    run(variable);
    BOOST_CHECK_CLOSE(survey.values[EXP][0], 1, TOLERANCE);
    BOOST_CHECK_CLOSE(survey.values[STD][0], 0, TOLERANCE);
    BOOST_CHECK_CLOSE(survey.values[MIN][0], 1, TOLERANCE);
    BOOST_CHECK_CLOSE(survey.values[MAX][0], 1, TOLERANCE);
}

class LargeValues: public Antares::Solver::Variable::DummyVariable
{
public:
    double hourlyValue(unsigned int year, unsigned int /*hour*/) override
    {
        switch (year)
        {
        case 0:
            return 959327997.543667;
        case 1:
            return 959327998.410129;
        case 2:
            return 959327998.437623;
        case 3:
            return 959328000.311142;
        case 4:
            return 959327999.911116;
        default:
            return 0;
        }
    }
};

BOOST_FIXTURE_TEST_CASE(large_different_values, FullYearStudyFixture)
{
    LargeValues variable;
    run(variable);
    BOOST_CHECK_CLOSE(survey.values[EXP][0], 959327998.922736, TOLERANCE);
    // This value is WRONG, the std dev should be approx 1.02999981662262445
    BOOST_CHECK_CLOSE(survey.values[STD][0], 0., TOLERANCE);
    BOOST_CHECK_CLOSE(survey.values[MIN][0], 959327997.543667, TOLERANCE);
    BOOST_CHECK_CLOSE(survey.values[MAX][0], 959328000.311142, TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()
