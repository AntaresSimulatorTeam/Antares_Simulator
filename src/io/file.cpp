// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/file.h"

#include <yuni/core/system/suspend.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/io/file.h>

#ifdef YUNI_OS_WINDOWS
#include <io.h>

#include <yuni/core/system/windows.hdr.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <cstring>
#include <errno.h>
#include <fstream>

#include <antares/logs/logs.h>

namespace fs = std::filesystem;

constexpr int retryTimeout = 35; // seconds
// Bound the wait so that a failure which is not a lack of disk space can never
// turn into an endless retry loop. 100 attempts ~= 1 hour.
constexpr uint maxAttempts = 100;

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

namespace
{
//! Is it worth waiting and trying again ? Only a genuine lack of disk space is:
//! any other error will fail again identically on the next attempt.
bool isRetryable(int err)
{
    switch (err)
    {
    case ENOSPC:
#ifdef EDQUOT
    case EDQUOT:
#endif
        return true;
    default:
        return false;
    }
}
} // anonymous namespace

bool fileSetContent(const std::string& filename, const std::string& content)
{
    if (Yuni::System::windows)
    {
        // On Windows,  there is still the hard limit to 256 chars even if the API allows more
        if (filename.size() >= 256)
        {
            logs.warning() << "I/O error: Maximum path length limitation (> 256 characters)";
        }
    }

    for (uint attempt = 1; attempt <= maxAttempts; ++attempt)
    {
        if (attempt > 1)
        {
            if (attempt == 2)
            {
                // only one log entry, we already have not enough disk space :)
                logs.warning() << "impossible to write " << filename
                               << " (probably not enough disk space).";

                // Notification via the UI interface
                Yuni::String text;
                Yuni::DateTime::TimestampToString(text, "%H:%M");
                logs.info() << "Not enough disk space since " << text << ". Waiting...";
            }
            // waiting a little...
            Yuni::Suspend(retryTimeout);
        }

        errno = 0;
        Yuni::IO::File::Stream out(filename, Yuni::IO::OpenMode::write);
        if (not out.opened())
        {
            const int err = errno;
            if (not isRetryable(err))
            {
                // permission denied and friends: useless to spend more time to try
                // to write the file, we should abort immediatly
                logs.error() << "I/O error: cannot open " << filename << ": "
                             << std::strerror(err);
                return false;
            }
            continue;
        }

        if (content.empty()) // ok, good, it's over
        {
            return true;
        }

        // little io trick : when the size if greater than a page size, it is possible
        // to use ftruncate to reduce block disk allocation
        // In the optimal conditions, we may earn in some cases up to 15% of the
        // overall elapsed time for writing
        if (content.size() > 1024 * 1024)
        {
#ifdef YUNI_OS_WINDOWS
            int fd = _fileno(out.nativeHandle());
            // _chsize_s returns the error code itself, it does not set errno
            const int resizeError = _chsize_s(fd, (__int64)content.size());
#else
            int fd = fileno(out.nativeHandle());
            errno = 0;
            const int resizeError = (0 == ftruncate(fd, (off_t)content.size())) ? 0 : errno;
#endif
            if (resizeError != 0)
            {
                if (not isRetryable(resizeError))
                {
                    logs.error() << "I/O error: cannot resize " << filename << " to "
                                 << content.size() << " bytes: " << std::strerror(resizeError);
                    return false;
                }
                continue; // not enough disk space
            }
        }

        // Write the raw buffer. Handing the std::string over as-is would pick the
        // generic Yuni::IO::File::Stream::write() overload, which copies it into a
        // Yuni::String whose size type is only 32 bits: any content of 4 GB or more
        // would be silently truncated (and a 4 GB one truncated down to nothing).
        errno = 0;
        const uint64_t written = out.write(content.data(),
                                           static_cast<uint64_t>(content.size()));
        if (written != content.size())
        {
            const int err = errno;
            if (not isRetryable(err))
            {
                logs.error() << "I/O error: incomplete write of " << filename
                             << " (written = " << written << ", size = " << content.size()
                             << "): " << std::strerror(err);
                return false;
            }
            continue; // not enough disk space
        }

        // OK, good
        // Notifying the user / logs that we can safely continue. It could be interresting
        // to have this log entry if the logs did not have enough disk space for itself
        if (attempt > 1)
        {
            // do not wait for the end of the loop for closing the file
            out.close();

            // For UI notification
            logs.info() << "Resuming...";
        }
        return true;
    }

    logs.error() << "I/O error: giving up on writing " << filename << " after " << maxAttempts
                 << " attempts (not enough disk space)";
    return false;
}

} // namespace Antares::IO
