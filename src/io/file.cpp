/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

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
    if (Yuni::System::windows)
    {
        // On Windows,  there is still the hard limit to 256 chars even if the API allows more
        if (filename.size() >= 256)
        {
            logs.warning() << "I/O error: Maximum path length limitation (> 256 characters)";
        }
    }

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
                Yuni::String text;
                Yuni::DateTime::TimestampToString(text, "%H:%M");
                logs.info() << "Not enough disk space since " << text << ". Waiting...";
                // break;
            }
            // waiting a little...
            Yuni::Suspend(retryTimeout);
        }
        ++attempt;

        Yuni::IO::File::Stream out(filename, Yuni::IO::OpenMode::write);
        if (not out.opened())
        {
            switch (errno)
            {
            case EACCES:
                // permission denied, useless to spend more time to try to write
                // the file, we should abort immediatly
                // aborting only if it is the first attempt, otherwise it could be a
                // side effect for some cleanup
                if (0 == attempt)
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
            if (0 != _chsize_s(fd, (__int64)content.size()))
#else
            int fd = fileno(out.nativeHandle());
            if (0 != ftruncate(fd, (off_t)content.size()))
#endif
            {
                // not enough disk space
                continue;
            }
        }
        if (content.size() != out.write(content))
        {
            continue; // not enough disk space
        }

        // OK, good
        // Notifying the user / logs that we can safely continue. It could be interresting
        // to have this log entry if the logs did not have enough disk space for itself
        if (attempt /* - 1*/ > 1)
        {
            // do not wait for the end of the loop for closing the file
            out.close();

            // For UI notification
            logs.info() << "Resuming...";
        }
        return true;
    } while (true);

    return false;
}

} // namespace Antares::IO
