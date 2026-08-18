// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE logs_format
#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/logs/logs.h>

#include "files-system.h"

using namespace Antares;

namespace
{

std::vector<std::string> readLines(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

// "[YYYY-MM-DD HH:MM:SS]" -> 21 bytes
const std::string timestampPattern = R"(\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\])";

void checkLine(const std::string& line, const std::string& expectedAfterTimestamp)
{
    const std::regex fullPattern(timestampPattern + expectedAfterTimestamp);
    BOOST_TEST_INFO("line: '" + line + "' expected suffix: '" + expectedAfterTimestamp + "'");
    BOOST_CHECK(std::regex_match(line, fullPattern));
}

std::string escapeForRegex(const std::string& s)
{
    static const std::regex specialChars(R"([-[\]{}()*+?.,\^$|#])");
    return std::regex_replace(s, specialChars, R"(\$&)");
}

// Redirects a std::ostream to an internal buffer for the lifetime of the object
class StreamCapture
{
public:
    explicit StreamCapture(std::ostream& stream):
        stream_(stream),
        previousBuffer_(stream.rdbuf(captured_.rdbuf()))
    {
    }

    ~StreamCapture()
    {
        stream_.rdbuf(previousBuffer_);
    }

    StreamCapture(const StreamCapture&) = delete;
    StreamCapture& operator=(const StreamCapture&) = delete;
    StreamCapture(StreamCapture&&) = delete;
    StreamCapture& operator=(StreamCapture&&) = delete;

    std::string str() const
    {
        return captured_.str();
    }

private:
    std::ostringstream captured_;
    std::ostream& stream_;
    std::streambuf* previousBuffer_;
};

// Saves/restores global logger state, opens a temporary log file
class LogsUnderTest
{
public:
    LogsUnderTest():
        workingDir_(
          createTempDirectory(boost::unit_test::framework::current_test_case().p_name))
    {
        logFilePath_ = workingDir_ / "format-test.log";
        logs.applicationName("test");
        logs.verbosityLevel = 100000; // let everything through
        BOOST_REQUIRE(logs.logfile(logFilePath_.string()));
        BOOST_REQUIRE(logs.logfileIsOpened());
    }

    ~LogsUnderTest()
    {
        logs.closeLogfile();
        logs.callback.clear();
        logs.applicationName("noname");
#ifdef NDEBUG
        logs.verbosityLevel = 8000; // Compatibility::level
#else
        logs.verbosityLevel = 10000; // Debug::level
#endif
        std::error_code ec;
        std::filesystem::remove_all(workingDir_, ec);
    }

    LogsUnderTest(const LogsUnderTest&) = delete;
    LogsUnderTest& operator=(const LogsUnderTest&) = delete;
    LogsUnderTest(LogsUnderTest&&) = delete;
    LogsUnderTest& operator=(LogsUnderTest&&) = delete;

    std::vector<std::string> readLogFile() const
    {
        return readLines(logFilePath_);
    }

    const std::filesystem::path& logFilePath() const
    {
        return logFilePath_;
    }

private:
    std::filesystem::path workingDir_;
    std::filesystem::path logFilePath_;
};

} // namespace

BOOST_AUTO_TEST_SUITE(file_line_format)

BOOST_FIXTURE_TEST_CASE(each_level_has_exact_tag_bytes, LogsUnderTest)
{
    logs.fatal() << "message";
    logs.error() << "message";
    logs.warning() << "message";
    logs.checkpoint() << "message";
    logs.notice() << "message";
    logs.info() << "message";

    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 8);
    checkLine(lines[0], R"(\[test\]\[fatal\] message)");
    checkLine(lines[1], R"(\[test\]\[error\] message)");
    checkLine(lines[2], R"(\[test\]\[warns\] message)");
    checkLine(lines[3], R"(\[test\]\[check\] message)");
    checkLine(lines[4], R"(\[test\]\[notic\] message)");
    checkLine(lines[6], R"(\[test\]\[infos\] message)");
}

BOOST_FIXTURE_TEST_CASE(no_ansi_escape_bytes_in_file, LogsUnderTest)
{
    logs.error() << "colored on console, plain in file";
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    BOOST_CHECK(lines[0].find('\x1b') == std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(thread_number_appends_dash_suffix_to_application_name, LogsUnderTest)
{
    Logs::threadNumber() = 42;
    logs.info() << "with thread";
    Logs::threadNumber().reset();
    logs.info() << "without thread";

    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 2);
    checkLine(lines[0], R"(\[test-42\]\[infos\] with thread)");
    checkLine(lines[1], R"(\[test\]\[infos\] without thread)");
}

BOOST_FIXTURE_TEST_CASE(empty_flush_writes_prefix_and_trailing_space, LogsUnderTest)
{
    logs.info();
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    checkLine(lines[0], R"(\[test\]\[infos\] )");
    BOOST_CHECK_EQUAL(lines[0].back(), ' ');
}

BOOST_FIXTURE_TEST_CASE(message_passed_as_argument_is_logged, LogsUnderTest)
{
    logs.info("direct argument");
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    checkLine(lines[0], R"(\[test\]\[infos\] direct argument)");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(operand_formatting)

BOOST_FIXTURE_TEST_CASE(floating_point_uses_printf_f_format, LogsUnderTest)
{
    logs.info() << 3.5;
    logs.info() << 3.5f;
    logs.info() << -0.25;
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 3);
    checkLine(lines[0], R"(\[test\]\[infos\] 3\.500000)");
    checkLine(lines[1], R"(\[test\]\[infos\] 3\.500000)");
    checkLine(lines[2], R"(\[test\]\[infos\] -0\.250000)");
}

BOOST_FIXTURE_TEST_CASE(integers_are_decimal, LogsUnderTest)
{
    logs.info() << 42 << ' ' << -7 << ' ' << static_cast<uint64_t>(18446744073709551615ull) << ' '
                << static_cast<short>(-3) << ' ' << 0u;
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    checkLine(lines[0], R"(\[test\]\[infos\] 42 -7 18446744073709551615 -3 0)");
}

BOOST_FIXTURE_TEST_CASE(bool_is_spelled_out, LogsUnderTest)
{
    logs.info() << true << ' ' << false;
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    checkLine(lines[0], R"(\[test\]\[infos\] true false)");
}

BOOST_FIXTURE_TEST_CASE(char_types_append_as_characters_not_numbers, LogsUnderTest)
{
    logs.info() << 'x' << static_cast<unsigned char>(65);
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    checkLine(lines[0], R"(\[test\]\[infos\] xA)");
}

BOOST_FIXTURE_TEST_CASE(filesystem_path_is_streamed_raw_without_quotes, LogsUnderTest)
{
    const std::filesystem::path path("some dir/sub");
    logs.info() << "Output folder : " << path;
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    const std::string expected = std::string("Output folder : ") + escapeForRegex(path.string());
    checkLine(lines[0], R"(\[test\]\[infos\] )" + expected);
    BOOST_CHECK(lines[0].find('"') == std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(std_string_and_literals_append_verbatim, LogsUnderTest)
{
    const std::string part = "std::string part";
    logs.info() << "literal " << part;
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 1);
    checkLine(lines[0], R"(\[test\]\[infos\] literal std::string part)");
}

BOOST_FIXTURE_TEST_CASE(append_format_uses_printf_semantics, LogsUnderTest)
{
    logs.info().appendFormat("%s: %02luh%02lum%02lus", "Total", 1ul, 2ul, 3ul);
    logs.info().appendFormat("value = %e", 1234.5);
    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 2);
    checkLine(lines[0], R"(\[test\]\[infos\] Total: 01h02m03s)");
    checkLine(lines[1], R"(\[test\]\[infos\] value = 1\.234500e\+03)");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(callback_contract)

namespace
{
struct CallbackRecorder final
{
    std::vector<std::pair<int, std::string>> received;

    void onLogMessage(int level, const std::string& message)
    {
        received.emplace_back(level, message);
    }
};
} // namespace

BOOST_FIXTURE_TEST_CASE(callback_receives_raw_message_and_level, LogsUnderTest)
{
    CallbackRecorder recorder;
    logs.callback.connect(&recorder, &CallbackRecorder::onLogMessage);

    logs.fatal() << "f";
    logs.error() << "e";
    logs.warning() << "w";
    logs.info() << "i";

    logs.callback.clear();

    BOOST_REQUIRE_EQUAL(recorder.received.size(), 4);
    BOOST_CHECK_EQUAL(recorder.received[0].first, 1000);
    BOOST_CHECK_EQUAL(recorder.received[0].second, "f");
    BOOST_CHECK_EQUAL(recorder.received[1].first, 2000);
    BOOST_CHECK_EQUAL(recorder.received[1].second, "e");
    BOOST_CHECK_EQUAL(recorder.received[2].first, 3000);
    BOOST_CHECK_EQUAL(recorder.received[2].second, "w");
    BOOST_CHECK_EQUAL(recorder.received[3].first, 7000);
    BOOST_CHECK_EQUAL(recorder.received[3].second, "i");
}

BOOST_FIXTURE_TEST_CASE(callback_skips_empty_messages, LogsUnderTest)
{
    CallbackRecorder recorder;
    logs.callback.connect(&recorder, &CallbackRecorder::onLogMessage);
    logs.info();
    logs.info() << "not empty";
    logs.callback.clear();

    BOOST_REQUIRE_EQUAL(recorder.received.size(), 1);
    BOOST_CHECK_EQUAL(recorder.received[0].second, "not empty");
}

#ifndef NDEBUG
BOOST_FIXTURE_TEST_CASE(callback_skips_debug_messages, LogsUnderTest)
{
    CallbackRecorder recorder;
    logs.callback.connect(&recorder, &CallbackRecorder::onLogMessage);
    logs.debug() << "debug line";
    logs.info() << "info line";
    logs.callback.clear();

    BOOST_REQUIRE_EQUAL(recorder.received.size(), 1);
    BOOST_CHECK_EQUAL(recorder.received[0].second, "info line");
}
#endif

BOOST_FIXTURE_TEST_CASE(callback_not_invoked_when_filtered_by_verbosity, LogsUnderTest)
{
    CallbackRecorder recorder;
    logs.callback.connect(&recorder, &CallbackRecorder::onLogMessage);
    logs.verbosityLevel = 2000; // Error::level
    logs.info() << "filtered out";
    logs.error() << "kept";
    logs.callback.clear();

    BOOST_REQUIRE_EQUAL(recorder.received.size(), 1);
    BOOST_CHECK_EQUAL(recorder.received[0].second, "kept");
}

BOOST_FIXTURE_TEST_CASE(clear_stops_delivery, LogsUnderTest)
{
    CallbackRecorder recorder;
    logs.callback.connect(&recorder, &CallbackRecorder::onLogMessage);
    logs.info() << "before clear";
    logs.callback.clear();
    logs.info() << "after clear";

    BOOST_REQUIRE_EQUAL(recorder.received.size(), 1);
    BOOST_CHECK_EQUAL(recorder.received[0].second, "before clear");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(verbosity_gating)

BOOST_FIXTURE_TEST_CASE(messages_above_verbosity_level_are_dropped, LogsUnderTest)
{
    logs.verbosityLevel = 2000; // Error::level
    logs.info() << "dropped";
    logs.warning() << "dropped too";
    logs.error() << "written";
    logs.fatal() << "also written";

    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 2);
    checkLine(lines[0], R"(\[test\]\[error\] written)");
    checkLine(lines[1], R"(\[test\]\[fatal\] also written)");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(console_routing)

BOOST_FIXTURE_TEST_CASE(errors_go_to_stderr_others_to_stdout, LogsUnderTest)
{
    std::string outBytes;
    std::string errBytes;
    {
        StreamCapture out(std::cout);
        StreamCapture err(std::cerr);
        logs.fatal() << "to err";
        logs.error() << "to err";
        logs.warning() << "to err";
        logs.info() << "to out";
        logs.notice() << "to out";
        logs.checkpoint() << "to out";
        outBytes = out.str();
        errBytes = err.str();
    }

    BOOST_CHECK(errBytes.find("[fatal]") != std::string::npos);
    BOOST_CHECK(errBytes.find("[error]") != std::string::npos);
    BOOST_CHECK(errBytes.find("[warns]") != std::string::npos);
    BOOST_CHECK(errBytes.find("[infos]") == std::string::npos);

    BOOST_CHECK(outBytes.find("[infos]") != std::string::npos);
    BOOST_CHECK(outBytes.find("[notic]") != std::string::npos);
    BOOST_CHECK(outBytes.find("[check]") != std::string::npos);
    BOOST_CHECK(outBytes.find("[error]") == std::string::npos);
}

#ifndef _WIN32
BOOST_FIXTURE_TEST_CASE(console_colors_exact_bytes_on_unix, LogsUnderTest)
{
    std::string outBytes;
    std::string errBytes;
    {
        StreamCapture out(std::cout);
        StreamCapture err(std::cerr);
        logs.error() << "msg";
        logs.info() << "msg";
        outBytes = out.str();
        errBytes = err.str();
    }

    // error: red tag, no message color
    BOOST_CHECK(errBytes.find("\x1b[0;31m[error]\x1b[0m msg") != std::string::npos);
    // info: no escape sequences at all
    BOOST_CHECK(outBytes.find('\x1b') == std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(checkpoint_colors_tag_and_message_on_unix, LogsUnderTest)
{
    std::string outBytes;
    {
        StreamCapture out(std::cout);
        logs.checkpoint() << "msg";
        outBytes = out.str();
    }
    BOOST_CHECK(outBytes.find("[check]") != std::string::npos);
    // white tag color present, message wrapped in color/reset
    BOOST_CHECK(outBytes.find("\x1b[1;37m") != std::string::npos);
    BOOST_CHECK(outBytes.find("\x1b[0m") != std::string::npos);
}
#endif

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(logfile_api)

BOOST_FIXTURE_TEST_CASE(logfile_getter_returns_path_and_reopening_appends, LogsUnderTest)
{
    BOOST_CHECK_EQUAL(std::string(logs.logfile().c_str()), logFilePath().string());

    logs.info() << "first";
    logs.closeLogfile();
    BOOST_CHECK(!logs.logfileIsOpened());

    // reopen same path: must append, not truncate (importlogs.cpp relies on this)
    BOOST_REQUIRE(logs.logfile(logFilePath().string()));
    logs.info() << "second";

    const auto lines = readLogFile();
    BOOST_REQUIRE_EQUAL(lines.size(), 2);
    checkLine(lines[0], R"(\[test\]\[infos\] first)");
    checkLine(lines[1], R"(\[test\]\[infos\] second)");
}

BOOST_AUTO_TEST_SUITE_END()
