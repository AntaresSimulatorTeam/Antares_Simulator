// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/logs.h"

#include <array>
#include <charconv>
#include <cstdio>
#include <ctime>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Antares::Logs
{

std::optional<int>& threadNumber()
{
    static thread_local std::optional<int> number;
    return number;
}

struct LevelInfo
{
    int level;
    const char* tag;
    bool toStderr;
    bool notifyCallback;
    // ANSI escape codes (unused on Windows console output, which uses SetConsoleTextAttribute)
    const char* tagColorAnsi;
    const char* messageColorAnsi;
#ifdef _WIN32
    WORD tagColorWin;
    WORD messageColorWin;
#endif
};

#ifdef _WIN32
#define ANTARES_LOGS_WIN_COLOR(attr) attr,
#else
#define ANTARES_LOGS_WIN_COLOR(attr)
#endif

#ifdef _WIN32
namespace
{
constexpr WORD winDefault = 7; // FOREGROUND_RED|GREEN|BLUE, no intensity
constexpr WORD winRed = FOREGROUND_RED | FOREGROUND_INTENSITY;
constexpr WORD winYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD winGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD winWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
                          | FOREGROUND_INTENSITY;
} // namespace
#else
namespace
{
[[maybe_unused]] constexpr int winDefault = 0;
[[maybe_unused]] constexpr int winRed = 0;
[[maybe_unused]] constexpr int winYellow = 0;
[[maybe_unused]] constexpr int winGreen = 0;
[[maybe_unused]] constexpr int winWhite = 0;
} // namespace
#endif

const LevelInfo levelInfoFatal = {Verbosity::Fatal::level,
                                  "fatal",
                                  true,
                                  true,
                                  "\x1b[0;31m",
                                  nullptr,
                                  ANTARES_LOGS_WIN_COLOR(winRed)
                                    ANTARES_LOGS_WIN_COLOR(winDefault)};
const LevelInfo levelInfoError = {Verbosity::Error::level,
                                  "error",
                                  true,
                                  true,
                                  "\x1b[0;31m",
                                  nullptr,
                                  ANTARES_LOGS_WIN_COLOR(winRed)
                                    ANTARES_LOGS_WIN_COLOR(winDefault)};
const LevelInfo levelInfoWarning = {Verbosity::Warning::level,
                                    "warns",
                                    true,
                                    true,
                                    "\x1b[0;33m",
                                    nullptr,
                                    ANTARES_LOGS_WIN_COLOR(winYellow)
                                      ANTARES_LOGS_WIN_COLOR(winDefault)};
const LevelInfo levelInfoCheckpoint = {Verbosity::Checkpoint::level,
                                       "check",
                                       false,
                                       true,
                                       "\x1b[1;37m\x1b[1m",
                                       "\x1b[1;37m\x1b[1m",
                                       ANTARES_LOGS_WIN_COLOR(winWhite)
                                         ANTARES_LOGS_WIN_COLOR(winWhite)};
const LevelInfo levelInfoNotice = {Verbosity::Notice::level,
                                   "notic",
                                   false,
                                   true,
                                   "\x1b[0;32m",
                                   nullptr,
                                   ANTARES_LOGS_WIN_COLOR(winGreen)
                                     ANTARES_LOGS_WIN_COLOR(winDefault)};

const LevelInfo levelInfoInfo = {Verbosity::Info::level,
                                 "infos",
                                 false,
                                 true,
                                 nullptr,
                                 nullptr,
                                 ANTARES_LOGS_WIN_COLOR(winDefault)
                                   ANTARES_LOGS_WIN_COLOR(winDefault)};

const LevelInfo levelInfoDebug = {Verbosity::Debug::level,
                                  "debug",
                                  false,
                                  false,
                                  nullptr,
                                  nullptr,
                                  ANTARES_LOGS_WIN_COLOR(winDefault)
                                    ANTARES_LOGS_WIN_COLOR(winDefault)};

#undef ANTARES_LOGS_WIN_COLOR

namespace
{

#ifdef _WIN32
void setConsoleColor(std::ostream& out, WORD color)
{
    HANDLE handle = GetStdHandle((&out == &std::cerr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, color);
}
#endif

} // namespace

Logger::Logger()
{
#ifdef NDEBUG
    verbosityLevel = Verbosity::Info::level;
#else
    verbosityLevel = Verbosity::Debug::level;
#endif
}

std::string timestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm tmBuffer{};
#ifdef _WIN32
    localtime_s(&tmBuffer, &now);
#else
    localtime_r(&now, &tmBuffer);
#endif
    char timestamp[20]; // "YYYY-MM-DD HH:MM:SS" + '\0'
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tmBuffer);

    return '[' + std::string(timestamp) + ']';
}

std::string application(const std::string& appliName)
{
    std::string result = '[' + appliName;
    if (const auto& tnum = threadNumber(); tnum.has_value())
    {
        result += '-' + std::to_string(*tnum);
    }
    result += ']';
    return result;
}

std::string tag(const LevelInfo& level)
{
    return '[' + std::string(level.tag) + ']';
}

std::string setTagColor(std::ostream& out, const LevelInfo& level)
{
    if (level.tagColorAnsi)
    {
#ifdef _WIN32
        setConsoleColor(out, level.tagColorWin);
#else
        return level.tagColorAnsi;
#endif
    }
    return {};
}

std::string setMsgColor(std::ostream& out, const LevelInfo& level)
{
    if (level.messageColorAnsi)
    {
#ifdef _WIN32
        setConsoleColor(out, level.messageColorWin);
#else
        return level.messageColorAnsi;
#endif
    }
    return {};
}

std::string removeColor(std::ostream& out)
{
#ifdef _WIN32
    setConsoleColor(out, winDefault);
#else
    out << "\x1b[0m";
#endif
    return {};
}

void writeToConsole(const LevelInfo& level, const std::string& appliName, const std::string& msg)
{
    std::ostream& console = level.toStderr ? std::cerr : std::cout;
    console << timestamp();
    console << application(appliName);
    console << setTagColor(console, level) << tag(level) << removeColor(console);
    console << ' ';
    console << setMsgColor(console, level) << msg << removeColor(console);
    console << '\n';
    console.flush();
}

void writeToFile(std::ofstream& file,
                 const LevelInfo& level,
                 const std::string& appliName,
                 const std::string& msg)
{
    file << timestamp() << application(appliName) << tag(level) << ' ' << msg;
#ifdef _WIN32
    file << "\r\n";
#else
    pFile << '\n';
#endif
    file.flush();
}

void Logger::dispatch(const LevelInfo& level, const std::string& message)
{
    std::lock_guard lock(pMutex);
    if (level.level > verbosityLevel)
    {
        return;
    }

    writeToConsole(level, pApplicationName, message);
    writeToFile(pFile, level, pApplicationName, message);

    if (level.notifyCallback && !callback.empty() && !message.empty())
    {
        callback(level.level, message);
    }
}

bool Logger::logfile(const std::string& filename)
{
    std::lock_guard lock(pMutex);
    if (pFile.is_open())
    {
        pFile.close();
    }
    pOutputFilename = filename;
    if (filename.empty())
    {
        return true;
    }
    pFile.open(filename, std::ios::binary | std::ios::app);
    return pFile.is_open();
}

std::string Logger::logfile() const
{
    std::lock_guard lock(pMutex);
    return pOutputFilename;
}

bool Logger::logfileIsOpened() const
{
    std::lock_guard lock(pMutex);
    return pFile.is_open();
}

void Logger::closeLogfile()
{
    std::lock_guard lock(pMutex);
    if (pFile.is_open())
    {
        pFile.close();
    }
}

void Logger::applicationName(std::string name)
{
    std::lock_guard lock(pMutex);
    pApplicationName = std::move(name);
}

void LogCallback::connect(std::function<void(int, const std::string&)> fn)
{
    std::lock_guard lock(pMutex);
    pCallbacks.push_back(std::move(fn));
}

void LogCallback::clear()
{
    std::lock_guard lock(pMutex);
    pCallbacks.clear();
}

bool LogCallback::empty() const
{
    std::lock_guard lock(pMutex);
    return pCallbacks.empty();
}

void LogCallback::operator()(int level, const std::string& message) const
{
    std::lock_guard lock(pMutex);
    for (const auto& fn: pCallbacks)
    {
        fn(level, message);
    }
}

LogBuffer::LogBuffer(Logger& logger, const LevelInfo& level):
    pLogger(&logger),
    pLevel(&level)
{
    pBuffer.reserve(512);
}

LogBuffer::~LogBuffer()
{
    pLogger->dispatch(*pLevel, pBuffer);
}

LogBuffer& LogBuffer::operator<<(const char* s)
{
    if (s)
    {
        pBuffer.append(s);
    }
    return *this;
}

LogBuffer& LogBuffer::operator<<(const std::string& s)
{
    pBuffer.append(s);
    return *this;
}

LogBuffer& LogBuffer::operator<<(std::string_view s)
{
    pBuffer.append(s);
    return *this;
}

LogBuffer& LogBuffer::operator<<(const std::filesystem::path& p)
{
    pBuffer.append(p.string());
    return *this;
}

LogBuffer& LogBuffer::operator<<(char c)
{
    pBuffer.push_back(c);
    return *this;
}

LogBuffer& LogBuffer::operator<<(unsigned char c)
{
    pBuffer.push_back(static_cast<char>(c));
    return *this;
}

LogBuffer& LogBuffer::operator<<(bool b)
{
    pBuffer.append(b ? "true" : "false");
    return *this;
}

LogBuffer& LogBuffer::operator<<(float f)
{
    return *this << static_cast<double>(f);
}

LogBuffer& LogBuffer::operator<<(double d)
{
    char buffer[64];
    int written = std::snprintf(buffer, sizeof(buffer), "%f", d);
    if (written > 0)
    {
        pBuffer.append(buffer, static_cast<std::size_t>(written));
    }
    return *this;
}

LogBuffer& LogBuffer::operator<<(const void* p)
{
    char buffer[32];
#ifdef _MSC_VER
    // With MSVC, %p does not provide the "0x" prefix
    buffer[0] = '0';
    buffer[1] = 'x';
    int written = std::snprintf(buffer + 2, sizeof(buffer) - 2, "%p", p);
    if (written > 0)
    {
        pBuffer.append(buffer, static_cast<std::size_t>(written) + 2);
    }
#else
    int written = std::snprintf(buffer, sizeof(buffer), "%p", p);
    if (written > 0)
    {
        pBuffer.append(buffer, static_cast<std::size_t>(written));
    }
#endif
    return *this;
}

void LogBuffer::appendInteger(long long v, bool isSigned)
{
    std::array<char, 24> buffer;
    std::to_chars_result result;
    if (isSigned)
    {
        result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), v);
    }
    else
    {
        result = std::to_chars(buffer.data(),
                               buffer.data() + buffer.size(),
                               static_cast<unsigned long long>(v));
    }
    pBuffer.append(buffer.data(), result.ptr);
}

void LogBuffer::appendFormat(const char format[], ...)
{
    va_list args;
    va_start(args, format);
    vappendFormat(format, args);
    va_end(args);
}

void LogBuffer::vappendFormat(const char format[], va_list args)
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    int needed = std::vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (needed <= 0)
    {
        return;
    }
    std::size_t oldSize = pBuffer.size();
    pBuffer.resize(oldSize + static_cast<std::size_t>(needed));
    std::vsnprintf(pBuffer.data() + oldSize, static_cast<std::size_t>(needed) + 1, format, args);
}

} // namespace Antares::Logs

void LogDisplayErrorInfos(unsigned int errors,
                          unsigned int warnings,
                          const char* message,
                          bool printAsError)
{
    using namespace Antares;

    std::string error;
    std::string warning;

    switch (errors)
    {
    case 0:
        break;
    case 1:
        error = "1 error";
        break;
    default:
        error = std::to_string(errors) + " errors";
        break;
    }
    switch (warnings)
    {
    case 0:
        break;
    case 1:
        warning = "1 warning";
        break;
    default:
        warning = std::to_string(warnings) + " warnings";
        break;
    }

    auto logLambda = [&](auto&& stream)
    {
        if (errors && warnings)
        {
            stream << "Found " << error << " and " << warning << ": " << message;
        }
        else
        {
            if (errors)
            {
                stream << "Found " << error << ": " << message;
            }
            if (warnings)
            {
                stream << "Found " << warning << ": " << message;
            }
        }
    };

    if (printAsError)
    {
        logLambda(logs.error());
    }
    else
    {
        logLambda(logs.info());
    }
}
