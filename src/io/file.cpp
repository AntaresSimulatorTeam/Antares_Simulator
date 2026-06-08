// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/file.h"

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <thread>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <fstream>

#include <antares/logs/logs.h>

namespace fs = std::filesystem;

constexpr int retryTimeout = 35; // seconds

namespace Antares::IO
{

static void logErrorAndThrow [[noreturn]] (const std::string& errorMessage)
{
    Antares::logs.error() << errorMessage;
    throw std::runtime_error(errorMessage);
}

std::string readFile(const fs::path& filePath)
{
    std::ifstream file(filePath, std::ios_base::binary | std::ios_base::in);
    if (!file.is_open())
    {
        logErrorAndThrow(filePath.string() + ": file does not exist");
    }

    using Iterator = std::istreambuf_iterator<char>;
    std::string content(Iterator{file}, Iterator{});
    if (!file)
    {
        logErrorAndThrow("Read failed '" + filePath.string() + "'");
    }
    return content;
}

bool fileSetContent(const std::string& filename, const std::string& content)
{
#ifdef _WIN32
    // On Windows,  there is still the hard limit to 256 chars even if the API allows more
    if (filename.size() >= 256)
    {
        logs.warning() << "I/O error: Maximum path length limitation (> 256 characters)";
    }
#endif

    uint attempt = 0;
    do
    {
        if (attempt > 0)
        {
            if (attempt == 1)
            {
                // only one log entry, we already have not enough disk space :)
                logs.warning() << "impossible to write " << filename
                               << " (probably not enough disk space).";

                // Notification via the UI interface
                char timeBuffer[16];
                std::time_t now = std::time(nullptr);
                std::tm timeInfo{};
#ifdef _WIN32
                localtime_s(&timeInfo, &now);
#else
                localtime_r(&now, &timeInfo);
#endif
                if (std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", &timeInfo))
                {
                    logs.info() << "Not enough disk space since " << timeBuffer << ". Waiting...";
                }
                else
                {
                    logs.info() << "Not enough disk space. Waiting...";
                }
            }
            // waiting a little...
            std::this_thread::sleep_for(std::chrono::seconds(retryTimeout));
        }
        ++attempt;

        std::FILE* fh = std::fopen(filename.c_str(), "wb");
        if (!fh)
        {
            switch (errno)
            {
            case EACCES:
                // permission denied, useless to spend more time to try to write
                // the file, we should abort immediately
                // aborting only if it is the first attempt, otherwise it could be a
                // side effect for some cleanup
                if (1 == attempt)
                {
                    logs.error() << "I/O error: permission denied: " << filename;
                    return false;
                }
                break;
            }
            continue;
        }

        if (content.empty()) // ok, good, it's over
        {
            std::fclose(fh);
            return true;
        }

        // little io trick : when the size if greater than a page size, it is possible
        // to use ftruncate to reduce block disk allocation
        // In the optimal conditions, we may earn in some cases up to 15% of the
        // overall elapsed time for writing
        if (content.size() > 1024 * 1024)
        {
#ifdef _WIN32
            int fd = _fileno(fh);
            if (0 != _chsize_s(fd, (__int64)content.size()))
#else
            int fd = fileno(fh);
            if (0 != ftruncate(fd, (off_t)content.size()))
#endif
            {
                // not enough disk space
                std::fclose(fh);
                continue;
            }
        }
        std::size_t written = std::fwrite(content.data(), 1, content.size(), fh);
        if (written != content.size())
        {
            std::fclose(fh);
            continue; // not enough disk space
        }

        // OK, good
        // Notifying the user / logs that we can safely continue. It could be interresting
        // to have this log entry if the logs did not have enough disk space for itself
        if (attempt > 1)
        {
            // For UI notification
            std::fclose(fh);
            logs.info() << "Resuming...";
        }
        else
        {
            std::fclose(fh);
        }
        return true;
    } while (true);

    return false;
}

} // namespace Antares::IO
