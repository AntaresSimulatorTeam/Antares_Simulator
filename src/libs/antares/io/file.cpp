/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "file.h"
#include <yuni/io/file.h>
#include <yuni/core/system/suspend.h>
#include <yuni/datetime/timestamp.h>
#ifdef YUNI_OS_WINDOWS
#include <yuni/core/system/windows.hdr.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif
#include <errno.h>
#include "../logs.h"

using namespace Yuni;
using namespace Antares;

enum
{
    retryTimeout = 35, // seconds
};

bool IOFileSetContent(const AnyString& filename, const AnyString& content)
{
    if (System::windows)
    {
        // On Windows,  there is still the hard limit to 256 chars even if the API allows more
        if (filename.size() >= 256)
            logs.warning() << "I/O error: Maximum path length limitation (> 256 characters)";
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
                String text;
                DateTime::TimestampToString(text, "%H:%M");
                logs.info() << "Not enough disk space since " << text << ". Waiting...";
                // break;
            }
            // waiting a little...
            Suspend(retryTimeout);
        }
        ++attempt;

        IO::File::Stream out(filename, IO::OpenMode::write);
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
            return true;

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
            continue; // not enough disk space

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
