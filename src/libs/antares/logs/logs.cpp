// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/logs.h"

#include <array>
#include <charconv>
#include <cstdio>
#include <iostream>

#include "antares/logs/logger-utils-linux.h"
#include "antares/logs/logger-utils-win.h"
#include "antares/logs/logger-utils.h"

namespace Antares::Logs
{

std::optional<int>& threadNumber()
{
    static thread_local std::optional<int> number;
    return number;
}

const LevelInfo levelInfoFatal = {Verbosity::Fatal::level,
                                  "fatal",
                                  true,
                                  true,
                                  Color::red,
                                  Color::none};
const LevelInfo levelInfoError = {Verbosity::Error::level,
                                  "error",
                                  true,
                                  true,
                                  Color::red,
                                  Color::none};
const LevelInfo levelInfoWarning = {Verbosity::Warning::level,
                                    "warns",
                                    true,
                                    true,
                                    Color::yellow,
                                    Color::none};
const LevelInfo levelInfoCheckpoint = {Verbosity::Checkpoint::level,
                                       "check",
                                       false,
                                       true,
                                       Color::white,
                                       Color::white};
const LevelInfo levelInfoNotice = {Verbosity::Notice::level,
                                   "notic",
                                   false,
                                   true,
                                   Color::green,
                                   Color::none};

const LevelInfo levelInfoInfo = {Verbosity::Info::level,
                                 "infos",
                                 false,
                                 true,
                                 Color::none,
                                 Color::none};

const LevelInfo levelInfoDebug = {Verbosity::Debug::level,
                                  "debug",
                                  false,
                                  false,
                                  Color::none,
                                  Color::none};

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
    setLocalTime(&tmBuffer, &now);

    char timestamp[20];
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

void writeToConsole(const LevelInfo& level, const std::string& appliName, const std::string& msg)
{
    std::ostream& console = level.toStderr ? std::cerr : std::cout;
    ColorEnabler colorEnabler(console);
    console << timestamp();
    console << application(appliName);
    console << colorEnabler.tagColor(level) << tag(level) << colorEnabler.removeColor();
    console << ' ';
    console << colorEnabler.msgColor(level) << msg << colorEnabler.removeColor();
    console << '\n';
    console.flush();
}

void writeToFile(std::ofstream& file,
                 const LevelInfo& level,
                 const std::string& appliName,
                 const std::string& msg)
{
    file << timestamp() << application(appliName) << tag(level) << ' ' << msg;
    file << eol();
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
