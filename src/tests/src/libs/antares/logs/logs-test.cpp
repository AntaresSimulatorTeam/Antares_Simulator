// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE LogsTest

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/logs/logs.h>

using Antares::UnitTests::CaptureAntaresLogs;

BOOST_AUTO_TEST_SUITE(LogDisplayErrorInfosSuite)

BOOST_FIXTURE_TEST_CASE(no_errors_no_warnings, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(0, 0, "test message", false);
    BOOST_CHECK(getErrors().empty());
    BOOST_CHECK(getInfos().empty());
}

BOOST_FIXTURE_TEST_CASE(one_error_no_warnings_info, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(1, 0, "test message", false);
    BOOST_CHECK(getErrors().empty());
    BOOST_CHECK(getInfos().contains("Found 1 error: test message"));
}

BOOST_FIXTURE_TEST_CASE(one_error_no_warnings_error, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(1, 0, "test message", true);
    BOOST_CHECK(getInfos().empty());
    BOOST_CHECK(getErrors().contains("Found 1 error: test message"));
}

BOOST_FIXTURE_TEST_CASE(multiple_errors_no_warnings_info, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(5, 0, "test message", false);
    BOOST_CHECK(getErrors().empty());
    BOOST_CHECK(getInfos().contains("Found 5 errors: test message"));
}

BOOST_FIXTURE_TEST_CASE(multiple_errors_no_warnings_error, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(5, 0, "test message", true);
    BOOST_CHECK(getInfos().empty());
    BOOST_CHECK(getErrors().contains("Found 5 errors: test message"));
}

BOOST_FIXTURE_TEST_CASE(no_errors_one_warning_info, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(0, 1, "test message", false);
    BOOST_CHECK(getErrors().empty());
    BOOST_CHECK(getInfos().contains("Found 1 warning: test message"));
}

BOOST_FIXTURE_TEST_CASE(no_errors_one_warning_error, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(0, 1, "test message", true);
    BOOST_CHECK(getInfos().empty());
    BOOST_CHECK(getErrors().contains("Found 1 warning: test message"));
}

BOOST_FIXTURE_TEST_CASE(no_errors_multiple_warnings_info, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(0, 3, "test message", false);
    BOOST_CHECK(getErrors().empty());
    BOOST_CHECK(getInfos().contains("Found 3 warnings: test message"));
}

BOOST_FIXTURE_TEST_CASE(no_errors_multiple_warnings_error, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(0, 3, "test message", true);
    BOOST_CHECK(getInfos().empty());
    BOOST_CHECK(getErrors().contains("Found 3 warnings: test message"));
}

BOOST_FIXTURE_TEST_CASE(errors_and_warnings_info, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(2, 4, "test message", false);
    BOOST_CHECK(getErrors().empty());
    BOOST_CHECK(getInfos().contains("Found 2 errors and 4 warnings: test message"));
}

BOOST_FIXTURE_TEST_CASE(errors_and_warnings_error, CaptureAntaresLogs)
{
    LogDisplayErrorInfos(2, 4, "test message", true);
    BOOST_CHECK(getInfos().empty());
    BOOST_CHECK(getErrors().contains("Found 2 errors and 4 warnings: test message"));
}

BOOST_AUTO_TEST_SUITE_END()
