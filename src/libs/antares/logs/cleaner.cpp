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

#include "cleaner.h"
#include <yuni/io/directory/info.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/io/file.h>

using namespace Yuni;

namespace Antares
{
// days per month, immutable values for version prior to 3.9 for sure
// these values are not related in any cases to an Antares study
static const uint daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static inline bool IsLeapYear(uint year)
{
    if (0 == year % 4)
    {
        if (0 == year % 100)
            return (0 == (year % 400));
        return true;
    }
    return false;
}

static bool SuitableForDeletion(const AnyString& name, DateTime::Timestamp now)
{
    auto offset = name.find('-');
    if (offset < name.size())
    {
        ++offset;
        auto afterDate = name.find('-', offset);
        if (afterDate < name.size())
        {
            auto afterTime = name.find('.', afterDate);
            if (afterTime < name.size())
            {
                if (afterTime <= afterDate or afterDate <= offset)
                    return false;

                AnyString date(name.c_str() + offset, afterDate - offset);
                AnyString time(name.c_str() + afterDate + 1, afterTime - afterDate - 1);
                if (date.size() != 8 or time.size() != 6)
                    return false;

                AnyString year(date, 0, 4);
                AnyString month(date, 4, 2);
                AnyString day(date, 6, 2);
                AnyString hours(time, 0, 2);
                AnyString minutes(time, 2, 2);
                AnyString seconds(time, 4, 2);
                seconds.trimLeft("0");
                minutes.trimLeft("0");
                hours.trimLeft("0");
                day.trimLeft("0");
                month.trimLeft("0");

                DateTime::Timestamp timestamp = seconds.to<uint>() + minutes.to<uint>() * 60
                                                + hours.to<uint>() * 3600
                                                + (day.to<uint>() - 1) * 24 * 3600;

                uint y = year.to<uint>();
                if (!(y < 9999)) // 4 digits
                {
                    y = 1970;
                }
                else
                {
                    if (y < 1970)
                        y = 1970;
                }

                for (uint i = 1970; i < y; ++i)
                {
                    if (not IsLeapYear(i))
                        timestamp += 365 * 24 * 3600;
                    else
                        timestamp += 366 * 24 * 3600;
                }

                uint m = month.to<uint>();
                if (m < 12 and m)
                {
                    for (uint i = 0; i != m - 1; ++i)
                    {
                        if (i == 1 and IsLeapYear(y))
                            timestamp += 24 * 3600 * (daysPerMonth[i] + 1);
                        else
                            timestamp += 24 * 3600 * daysPerMonth[i];
                    }
                }

                if (timestamp < now)
                    return true;
            }
        }
    }
    return false;
}

void PurgeLogFiles(const AnyString& path, uint retention)
{
    if (path.empty())
        return;

    auto now = DateTime::Now();
    now -= retention;

    IO::Directory::Info info(path);

    auto end = info.file_end();
    for (auto i = info.file_begin(); i != end; ++i)
    {
        auto& name = *i;
        if (not name.startsWith("uisimulator-") or not name.endsWith(".log"))
            continue;

        if (SuitableForDeletion(name, now))
            IO::File::Delete(i.filename());
    }
}

} // namespace Antares
