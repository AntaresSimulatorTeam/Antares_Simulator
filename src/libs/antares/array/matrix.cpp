/*
** Copyright 2007-2023 RTE
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

#include "antares/array/matrix.h"
#include "antares/jit/jit.h"

using namespace Yuni;

namespace Antares
{
enum
{
    sizeofMatrix = sizeof(uint) * 2    // width, height
                   + sizeof(void*) * 2 // entry, jit
};

int MatrixTestForPositiveValues(const char* msg, const Matrix<>* m)
{
    uint x = 0;
    uint y;

    if (m->width and m->height)
    {
        for (; x < m->width; ++x)
        {
            const Matrix<>::ColumnType& col = m->entry[x];
            for (y = 0; y < m->height; ++y)
            {
                if (col[y] < 0.)
                {
                    logs.error() << msg << ": Negative value detected (at the position " << x << ','
                                 << y << ')';
                    return 0;
                }
            }
        }
    }
    return 1;
}

int MatrixTestIfValuesAreHigherThan(const char* msg, const Matrix<>* m, const double value)
{
    uint x = 0;
    uint y;

    logs.debug() << "Checking : " << msg;
    if (m->width and m->height)
    {
        for (; x < m->width; ++x)
        {
            const Matrix<>::ColumnType& col = m->entry[x];
            for (y = 0; y < m->height; ++y)
            {
                if (col[y] < value)
                {
                    logs.error() << msg << ": invalid value detected (at column " << x
                                 << ", row: " << y << ')';
                    return 0;
                }
            }
        }
    }
    return 1;
}

int MatrixTestIfValuesAreLowerThan(const char* msg, const Matrix<>* m, const double value)
{
    uint x = 0;
    uint y;

    logs.debug() << "Checking : " << msg;
    if (m->width and m->height)
    {
        for (; x < m->width; ++x)
        {
            const Matrix<>::ColumnType& col = m->entry[x];
            for (y = 0; y < m->height; ++y)
            {
                if (col[y] > value)
                {
                    logs.error() << msg << ": invalid value detected (at column " << x
                                 << ", row: " << y << ')';
                    return 0;
                }
            }
        }
    }
    return 1;
}

int MatrixTestForPositiveValues_LimitWidth(const char* msg, const Matrix<>* m, uint maxWidth)
{
    logs.debug() << "Checking : " << msg;
    if (m->width and m->height and maxWidth)
    {
        for (uint x = 0; x < maxWidth; ++x)
        {
            auto& column = m->entry[x];
            for (uint y = 0; y < m->height; ++y)
            {
                if (column[y] < 0.)
                {
                    logs.error() << msg << ": negative value detected (at column " << x
                                 << ", row: " << y << ')';
                    return 0;
                }
            }
        }
    }
    return 1;
}

} // namespace Antares
