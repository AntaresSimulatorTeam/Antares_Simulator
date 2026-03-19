// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test time series"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/variable/categories.h"
#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"

using namespace Antares::Solver::Variable;

struct StudyFixture
{
    StudyFixture():
        study(std::make_unique<Antares::Data::Study>())
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Antares::Data::Study> study;
};

BOOST_AUTO_TEST_SUITE(surveyresults)

BOOST_FIXTURE_TEST_CASE(survey_result_hourly, StudyFixture)
{
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);

    unsigned int nbVariables = 2;
    SurveyResults survey(*study, nbVariables, "out", writer);

    // Loop
    survey.data.columnIndex = nbVariables;
    {
        survey.captions[0][0] = "hello";
        survey.captions[1][0] = "unit";
        survey.captions[2][0] = "N/A";
    }
    {
        survey.captions[0][1] = "world";
        survey.captions[1][1] = "unit2";
        survey.captions[2][1] = "N/A";
    }
    for (unsigned int h = 0; h < HOURS_PER_YEAR; ++h)
    {
        survey.values[0][h] = h + 1;
        survey.values[1][h] = h * h + 3;
    }
    // Hourly
    {
        const std::string filename = "hourly.txt";
        survey.data.filename = filename;
        survey.saveToFile(Category::DataLevel::setOfAreas,
                          Category::FileLevel::va,
                          Category::Precision::hourly);

        BOOST_REQUIRE(writer.getMap().contains(filename));
        const auto& content = writer.getMap().at(filename);
        // Header
        BOOST_CHECK_NE(content.find("system	hourly				hello	world\n"),
                       std::string::npos);
        // Some values
        BOOST_CHECK_NE(content.find("	9	01	JAN	08:00	9	67\n"),
                       std::string::npos);
    }
    // Annual
    // ⚠ SurveyResults does not perform temporal aggregation;
    // it simply writes its content. Temporal aggregation must be done beforehand.
    if (false)
    {
        const std::string filename = "annual.txt";
        survey.data.filename = filename;
        survey.saveToFile(Category::DataLevel::setOfAreas,
                          Category::FileLevel::va,
                          Category::Precision::annual);
        BOOST_REQUIRE(writer.getMap().contains(filename));
        const auto& content = writer.getMap().at(filename);
        // Some values
        // NO AGGREGATION HERE
        BOOST_CHECK_NE(content.find("	9	01	JAN	08:00	9	67\n"),
                       std::string::npos);
    }
}

BOOST_AUTO_TEST_SUITE_END()
