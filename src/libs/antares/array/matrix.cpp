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
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/math.h>
#include <yuni/io/file.h>

#include "matrix.h"
#include <logs.h>
#include <math.h>
#include "../benchmarking.h"
#include "../study/memory-usage.h"

using namespace Yuni;

namespace Antares
{
enum
{
    sizeofMatrix = sizeof(uint) * 2    // width, height
                   + sizeof(void*) * 2 // entry, jit
};

void MatrixEstimateMemoryUsageFromJIT(size_t sizeofT,
                                      Antares::Data::StudyMemoryUsage& u,
                                      JIT::Informations* jit)
{
    if (jit)
    {
        // But we may already have the estimate value in cache
        if (jit->estimatedSize[0] and jit->estimatedSize[1])
        {
            MatrixEstimateMemoryUsage(sizeofT, u, jit->estimatedSize[0], jit->estimatedSize[1]);
            return;
        }

        if (not jit->sourceFilename.empty())
        {
            uint64 size;
            if (IO::File::Size(jit->sourceFilename, size))
            {
                if (size and size < Matrix<>::filesizeHardLimit)
                {
                    IO::File::Stream file(jit->sourceFilename);
                    if (file.opened())
                    {
                        // Load the data
                        // Copying the whole buffer
                        if (!u.buffer)
                            u.buffer = new Matrix<>::BufferType();
                        u.buffer->resize((String::Size)size);
                        // Read the entire file
                        if (size == file.read(u.buffer->data(), size))
                        {
                            // Adding a final \n to make sure we have a line return at the end of
                            // the file
                            *(u.buffer) += '\n';

                            String::Size offset = u.buffer->find('\n');
                            if (offset != String::npos)
                            {
                                String::Size n = 0;
                                uint columns = 1;
                                while (
                                  (n = u.buffer->find_first_of(ANTARES_MATRIX_CSV_SEPARATORS, n))
                                    != String::npos
                                  and n < offset)
                                {
                                    ++columns;
                                    ++n;
                                }
                                jit->estimatedSize[0] = columns;
                                jit->estimatedSize[1] = (uint)u.buffer->countChar('\n');

                                MatrixEstimateMemoryUsage(
                                  sizeofT, u, jit->estimatedSize[0], jit->estimatedSize[1]);
                                return;
                            }
                        }
                    }
                }
            }
        }
    } // if jit

    // We really don't know. The worst case is not 0.
    if (u.gatheringInformationsForInput)
        u.requiredMemoryForInput += sizeofMatrix;
    else
        u.requiredMemoryForOutput += sizeofMatrix;
}

void MatrixEstimateMemoryUsage(size_t sizeofT, Data::StudyMemoryUsage& u, uint w, uint h)
{
    for (uint i = 0; i != w; ++i)
        Antares::Memory::EstimateMemoryUsage(sizeofT, h, u, false);

    if (u.gatheringInformationsForInput)
        u.requiredMemoryForInput += sizeofMatrix + sizeof(void*) * w;
    else
        u.requiredMemoryForOutput += sizeofMatrix + sizeof(void*) * w;
}

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

int MatrixTestForNegativeValues(const char* msg, const Matrix<>* m)
{
    logs.debug() << "Checking : " << msg;
    if (m->width and m->height)
    {
        for (uint x = 0; x < m->width; ++x)
        {
            auto& column = m->entry[x];
            for (uint y = 0; y < m->height; ++y)
            {
                if (column[y] > 0.)
                {
                    logs.error() << msg << ": positive value detected (at column " << x
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
