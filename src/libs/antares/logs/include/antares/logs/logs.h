// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_LOGS_LOGS_H__
#define __ANTARES_LIBS_LOGS_LOGS_H__

#include <concepts>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

/*!
** \defgroup logs Logs
** \ingroup toolbox
*/

namespace Antares::Logs
{

namespace Verbosity
{
struct Fatal final
{
    static constexpr int level = 1000;
    static constexpr bool enabled = true;
};

struct Error final
{
    static constexpr int level = 2000;
    static constexpr bool enabled = true;
};

struct Warning final
{
    static constexpr int level = 3000;
    static constexpr bool enabled = true;
};

struct Checkpoint final
{
    static constexpr int level = 4000;
    static constexpr bool enabled = true;
};

struct Notice final
{
    static constexpr int level = 5000;
    static constexpr bool enabled = true;
};

struct Info final
{
    static constexpr int level = 7000;
    static constexpr bool enabled = true;
};

struct Debug final
{
    static constexpr int level = 10000;
#ifdef NDEBUG
    static constexpr bool enabled = false;
#else
    static constexpr bool enabled = true;
#endif
};
} // namespace Verbosity

//! Per-thread number appended to the application name in the log prefix (e.g. "[app-3]")
std::optional<int>& threadNumber();

class Logger;
struct LevelInfo;

// Matches Yuni::String/CString/ShortStringNN without depending on their headers.
template<class S>
concept CStrSizeStringLike = requires(const S& s) {
    { s.c_str() } -> std::convertible_to<const char*>;
    { s.size() } -> std::convertible_to<std::size_t>;
};

/*!
** \brief A single log entry being built; flushes to the logger on destruction.
*/
class LogBuffer final
{
public:
    LogBuffer(Logger& logger, const LevelInfo& level);

    template<class U>
    LogBuffer(Logger& logger, const LevelInfo& level, const U& u):
        LogBuffer(logger, level)
    {
        *this << u;
    }

    ~LogBuffer();

    LogBuffer(const LogBuffer&) = delete;
    LogBuffer& operator=(const LogBuffer&) = delete;
    LogBuffer(LogBuffer&&) = delete;
    LogBuffer& operator=(LogBuffer&&) = delete;

    LogBuffer& operator<<(const char* s);
    LogBuffer& operator<<(const std::string& s);
    LogBuffer& operator<<(std::string_view s);
    LogBuffer& operator<<(const std::filesystem::path& p);
    LogBuffer& operator<<(char c);
    LogBuffer& operator<<(unsigned char c);
    LogBuffer& operator<<(bool b);
    LogBuffer& operator<<(float f);
    LogBuffer& operator<<(double d);
    LogBuffer& operator<<(const void* p);

    template<class T>
    LogBuffer& operator<<(T* p)
    {
        return *this << static_cast<const void*>(p);
    }

    template<CStrSizeStringLike S>
    LogBuffer& operator<<(const S& s)
    {
        pBuffer.append(s.c_str(), s.size());
        return *this;
    }

    template<std::integral T>
    requires(!std::same_as<T, char> && !std::same_as<T, unsigned char>
             && !std::same_as<T, signed char> && !std::same_as<T, bool>)
    LogBuffer& operator<<(T v)
    {
        appendInteger(static_cast<long long>(v), std::is_signed_v<T>);
        return *this;
    }

    void appendFormat(const char format[], ...);
    void vappendFormat(const char format[], va_list args);

private:
    void appendInteger(long long v, bool isSigned);

    Logger* pLogger;
    const LevelInfo* pLevel;
    std::string pBuffer;
};

//! Compile-time no-op buffer used for debug() under NDEBUG
class NoopLogBuffer final
{
public:
    template<class U>
    const NoopLogBuffer& operator<<(const U&) const
    {
        return *this;
    }

    void appendFormat(const char[], ...) const
    {
        //noop
    }

    void vappendFormat(const char[], va_list) const
    {
        //noop
    }
};

/*!
** \brief Notification hook invoked with the raw (undecorated) message of every
** dispatched log entry, except empty messages and Debug-level ones.
*/
class LogCallback final
{
public:
    LogCallback() = default;
    ~LogCallback() = default;
    LogCallback(const LogCallback&) = delete;
    LogCallback& operator=(const LogCallback&) = delete;
    LogCallback(LogCallback&&) = delete;
    LogCallback& operator=(LogCallback&&) = delete;

    template<class T>
    void connect(T* object, void (T::*method)(int, const std::string&))
    {
        connect([object, method](int level, const std::string& message)
                { (object->*method)(level, message); });
    }

    void connect(std::function<void(int, const std::string&)> fn);
    void clear();
    bool empty() const;
    void operator()(int level, const std::string& message) const;

private:
    mutable std::mutex pMutex;
    std::vector<std::function<void(int, const std::string&)>> pCallbacks;
};

class Logger final
{
public:
    Logger();
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    LogBuffer fatal();
    template<class U>
    LogBuffer fatal(const U& u);

    LogBuffer error();
    template<class U>
    LogBuffer error(const U& u);

    LogBuffer warning();
    template<class U>
    LogBuffer warning(const U& u);

    LogBuffer checkpoint();
    template<class U>
    LogBuffer checkpoint(const U& u);

    LogBuffer notice();
    template<class U>
    LogBuffer notice(const U& u);

    LogBuffer info();
    template<class U>
    LogBuffer info(const U& u);

#ifdef NDEBUG
    NoopLogBuffer debug()
    {
        return {};
    }

    template<class U>
    NoopLogBuffer debug(const U&)
    {
        return {};
    }
#else
    LogBuffer debug();
    template<class U>
    LogBuffer debug(const U& u);
#endif

    //! Opens (append mode) the log file at the given path; empty path closes it. Returns success.
    bool logfile(const std::string& filename);
    std::string logfile() const;
    bool logfileIsOpened() const;
    void closeLogfile();

    void applicationName(std::string name);

    //! Minimum level required for a message to be dispatched (see Verbosity::*::level)
    int verbosityLevel;
    //! Connect to be notified of every dispatched message (see LogCallback)
    LogCallback callback;

private:
    friend class LogBuffer;
    void dispatch(const LevelInfo& level, const std::string& message);

    mutable std::mutex pMutex;
    std::string pApplicationName = "noname";
    std::string pOutputFilename;
    std::ofstream pFile;
};

// Declared here, at namespace scope, so every translation unit that includes this header sees a
// real top-level `extern` declaration before any use. A declaration nested inside these inline
// accessor functions is not enough to reliably establish external linkage for the `const`
// definitions in logs.cpp under MSVC.
extern const LevelInfo levelInfoFatal;
extern const LevelInfo levelInfoError;
extern const LevelInfo levelInfoWarning;
extern const LevelInfo levelInfoCheckpoint;
extern const LevelInfo levelInfoNotice;
extern const LevelInfo levelInfoInfo;
#ifndef NDEBUG
extern const LevelInfo levelInfoDebug;
#endif

#define ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(NAME, TAG) \
    inline LogBuffer Logger::NAME()                 \
    {                                               \
        return LogBuffer(*this, levelInfo##TAG);    \
    }                                               \
    template<class U>                               \
    inline LogBuffer Logger::NAME(const U& u)       \
    {                                               \
        return LogBuffer(*this, levelInfo##TAG, u); \
    }

ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(fatal, Fatal)
ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(error, Error)
ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(warning, Warning)
ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(checkpoint, Checkpoint)
ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(notice, Notice)
ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(info, Info)
#ifndef NDEBUG
ANTARES_LOGS_LEVEL_ACCESSOR_IMPL(debug, Debug)
#endif

#undef ANTARES_LOGS_LEVEL_ACCESSOR_IMPL

} // namespace Antares::Logs

namespace Antares
{
extern Logs::Logger logs;
}

/*!
** \brief Display informations about encountered errors
*/
void LogDisplayErrorInfos(unsigned int errors,
                          unsigned int warnings,
                          const char* message,
                          bool printAsError = true);

#endif /* __ANTARES_LIBS_LOGS_LOGS_H__ */
