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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <yuni/io/file.h>
#include <yuni/core/string.h>
#include <limits.h>
#include "array1d.h"
#include "../logs.h"

#define ARRAY_BUFFER_SIZE 1024 * 256 /* 256Ko */

#ifdef YUNI_OS_MSVC
#define ANTARES_MATRIX_SNPRINTF sprintf_s
#else
#define ANTARES_MATRIX_SNPRINTF snprintf
#endif

using namespace Antares;
using namespace Yuni;

template<class U>
static int Array1DLoadFromBuffer(const char* filename, U& data, double values[], const int height)
{
    logs.debug() << "  :: loading `" << filename << "'";
    // Position
    Yuni::String::Size offset = 0;
    int y = 0;
    uint pos;
    int errorCount = 6;
    // char* pend;

    while (y < height and offset < data.size())
    {
        // Starting the reading of the begining of the line
        pos = offset;
        if ((offset = data.find('\n', offset)) != Yuni::String::npos)
        {
            data[offset] = '\0';
            AnyString cell(data.c_str() + pos, offset - pos);
            if (!cell)
            {
                logs.warning() << filename << ": empty data at line " << (y + 1) << ", assuming 0";
                values[y] = 0.;
            }
            else
            {
                double v;
                if (!cell.to(v))
                {
                    if (errorCount)
                    {
                        logs.warning() << filename << ": invalid decimal value at line " << (y + 1)
                                       << ", assuming 0";
                        if (!(--errorCount))
                            logs.warning() << " ... (skipped)";
                    }
                    values[y] = 0.;
                }
                else
                    values[y] = v;
            }
            /*
            // Adding the value
            if (data[pos] == '0' and data[pos + 1] == '\0')
            {
                    values[y] = 0.;
            }
            else
            {
                    // Convert string into double
                    values[y] = strtod(data.c_str() + pos, &pend);
                    if (!(pend and '\0' == *pend))
                    {
                            values[y] = 0.;
                            if (errorCount)
                            {
                                    logs.warning() << '`' << filename
                                            << "`: Invalid format: Decimal value expected (y:" << y
            << ", offset:" << (uint)pos << "byte)"; if (!(--errorCount)) logs.warning() << " ...
            (skipped)";
                            }
                    }
            }
            */

            pos = ++offset;
        }

        // Go to the next line
        ++y;

    } // while (y ...)

    // Not enough lines to describe our matrix
    if (y < height)
    {
        logs.warning() << filename << ": not enough rows (expected " << height << ", got " << y
                       << ')';

        // Initialize missing values
        while (y < height)
        {
            values[y] = 0.;
            ++y;
        }
        return 0;
    }
    return 1;
}

int Array1DLoadFromFile(const char* filename, double* data, const int height)
{
    assert(NULL != data);
    if (NULL != filename and '\0' != *filename)
    {
        // Get a file descriptor
        const uint64 size = IO::File::Size(filename);

        if (size)
        {
            IO::File::Stream file(filename);
            if (file.opened())
            {
                Clob buffer;
                buffer.resize((String::Size)size);

                // Read the entire file
                file >> buffer;
                // Adding a final \n to make sure we have a line return at the end of the file
                buffer += '\n';

                // Load the data
                Array1DLoadFromBuffer(filename, buffer, data, height);
                return 1;
            }
            logs.error() << "I/O error: " << filename << ": Impossible to load the file";
            memset(data, 0, sizeof(double) * height);
        }
        else
        {
            logs.warning() << filename << ": The file should not be empty";
            memset(data, 0, sizeof(double) * height);
        }
    }
    return 0;
}

int Array1DSaveToFile(const char filename[], const double data[], const int count, uint precision)
{
    assert(NULL != filename);
    logs.debug() << "  :: writing `" << filename << "` (h:" << count << ')';

    // Open the file for reading
    IO::File::Stream file;
    if (file.openRW(filename))
    {
        if (count)
        {
            const char* format;
            switch (precision)
            {
            // The 0 value in first
            case 0:
                format = "%.0f\n";
                break;
                // Then 6
            case 6:
                format = "%.6f\n";
                break;
                // All other entry
            case 1:
                format = "%.1f\n";
                break;
            case 2:
                format = "%.2f\n";
                break;
            case 3:
                format = "%.3f\n";
                break;
            case 4:
                format = "%.4f\n";
                break;
            case 5:
                format = "%.5f\n";
                break;
            default:
                format = "%.6f\n";
                break;
            }

            char buffer[128];
            for (int i = 0; i < count; ++i)
            {
                if (Math::Zero(data[i]))
                {
                    file += "0\n";
                }
                else
                {
                    if (!precision or fabs(data[i] - floor(data[i])) < YUNI_EPSILON)
                        ANTARES_MATRIX_SNPRINTF(buffer, sizeof(buffer), "%.0f\n", data[i]);
                    else
                        ANTARES_MATRIX_SNPRINTF(buffer, sizeof(buffer), format, data[i]);
                    file += (const char*)buffer;
                }
            }
        }
        return 1;
    }
    logs.error() << "I/O error: " << filename << ": Impossible to write the file";
    return 0;
}

int Array1DCheckStrictlyPositiveValues(const char msg[], const double data[], const int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (data[i] <= 0.)
        {
            logs.error() << msg << ": At least one value is not strictly positive (at " << i << ')';
            return 0;
        }
    }
    return 1;
}

int Array1DCheckPositiveValues(const char msg[], const double data[], const int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (data[i] < 0.)
        {
            logs.error() << msg << ": At least one value is not positive (at " << i << ')';
            return 0;
        }
    }
    return 1;
}

int Array1DCheckNegativeValues(const char msg[], const double data[], const int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (data[i] > 0.)
        {
            logs.error() << msg << ": At least one value is not negative (at " << i << ')';
            return 0;
        }
    }
    return 1;
}

void Array1DSetValues(double data[], const int count, const double value)
{
    for (int i = 0; i < count; ++i)
        data[i] = value;
}

int Array1DCheckForValuesInRange(const char msg[],
                                 const double data[],
                                 const int count,
                                 const double minV,
                                 const double maxV)
{
    for (int i = 0; i < count; ++i)
    {
        if (data[i] < minV or data[i] > maxV)
        {
            logs.error() << msg << ": All values must be within the range [" << minV << ',' << maxV
                         << ']';
            return 0;
        }
    }
    return 1;
}

int Array1DCheckGreaterOrEqual(const char msg[],
                               const double data[],
                               const int count,
                               const double v)
{
    int i = 0;
    for (; i < count; ++i)
    {
        if (data[i] < v)
        {
            logs.error() << msg << ": All values must be equal to or greater than " << v;
            return 0;
        }
    }
    return 1;
}
