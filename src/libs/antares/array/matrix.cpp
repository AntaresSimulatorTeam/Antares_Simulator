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

#include "antares/array/matrix.h"

#include <cstdlib>

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
