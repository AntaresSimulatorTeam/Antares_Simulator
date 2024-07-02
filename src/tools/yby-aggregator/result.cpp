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

#include "result.h"

#include "progress.h"

using namespace Yuni;
using namespace Antares;

namespace // anonymous
{
template<class StringT>
void AppendToBuffer(StringT& out, const char* buffer)
{
    uint length = 0;
    for (; buffer[length] != '\0'; ++length)
        ;
    out.append(buffer, length);
}

} // anonymous namespace

CellColumnData::CellColumnData():
    rows(nullptr),
    height(0)
{
}

CellColumnData::CellColumnData(const CellColumnData&)
{
}

CellColumnData::~CellColumnData()
{
}

ResultMatrix::ResultMatrix():
    columns(nullptr),
    width(0),
    heightAfterAggregation(0)
{
}

ResultMatrix::ResultMatrix(const ResultMatrix&):
    columns(nullptr),
    width(0),
    heightAfterAggregation(0)
{
}

ResultMatrix::~ResultMatrix()
{
    delete[] columns;
}

void ResultMatrix::resize(uint i)
{
    delete[] columns;
    columns = new CellColumnData[i];
    width = i;
}

bool ResultMatrix::saveToCSVFile(const String& filename) const
{
    IO::File::Stream file;
    if (!file.openRW(filename))
    {
        return false;
    }

    Progress::Total = heightAfterAggregation;
    String buffer;
    buffer.reserve(8 * 1024 * 1024);

    // Writing pseudo header like any other CSV in antares
    {
        // buffer
        //	<< "Alpes\tarea\tva\thourly\n"
        //	<< "\tVARIABLES\tBEGIN\tEND\n"
        //	<< "\t78\t1\t8736\n";
        buffer << "mc-var\taggregate\n"
               << "\t\tBEGIN\tEND\n"
               << "\t\t1\t" << heightAfterAggregation << '\n';
        buffer << '\n';
        {
            buffer << '\t';
            for (uint i = 0; i < width; ++i)
            {
                buffer << '\t' << "year";
            }
            buffer << '\n';

            buffer << '\t';
            for (uint i = 0; i < width; ++i)
            {
                buffer << '\t' << (i + 1);
            }
            buffer << '\n';
        }
        for (uint r = 0; r != 1; ++r)
        {
            buffer << '\t';
            for (uint i = 0; i < width; ++i)
            {
                buffer << '\t';
            }
            buffer << '\n';
        }
    }

    if (width > 1000)
    {
        enum
        {
            dataBufferHeight = 100
        };

        CellData* dataBuffer[dataBufferHeight];
        for (uint d = 0; d != dataBufferHeight; ++d)
        {
            dataBuffer[d] = new CellData[width];
        }

        uint dataBufferOffset = 0;
        for (uint y = 0; y != heightAfterAggregation; ++y)
        {
            if (!dataBufferOffset)
            {
                for (uint x = 0; x < width; ++x)
                {
                    if (dataBufferHeight + y >= heightAfterAggregation)
                    {
                        uint maxH = heightAfterAggregation - y;
                        for (uint offset = 0; offset != maxH; ++offset)
                        {
                            assert(offset + y < columns[x].height);
                            memcpy(dataBuffer[offset][x],
                                   columns[x].rows[offset + y],
                                   maxSizePerCell);
                        }
                    }
                    else
                    {
                        for (uint offset = 0; offset != dataBufferHeight; ++offset)
                        {
                            assert(offset + y < columns[x].height);
                            memcpy(dataBuffer[offset][x],
                                   columns[x].rows[offset + y],
                                   maxSizePerCell);
                        }
                    }
                }
            }

            buffer << '\t' << (1 + y) << '\t';
            AppendToBuffer(buffer, columns[0].rows[y]);
            for (uint x = 1; x < width; ++x)
            {
                buffer << '\t';
                AppendToBuffer(buffer, dataBuffer[dataBufferOffset][x]);

                if (buffer.size() > 1024 * 1024 * 16)
                {
                    file << buffer;
                    buffer.clear();
                }
            }
            buffer << '\n';
            ++Progress::Current;
            if (++dataBufferOffset == dataBufferHeight)
            {
                dataBufferOffset = 0;
            }
        }

        for (uint d = 0; d != dataBufferHeight; ++d)
        {
            delete[] dataBuffer[d];
        }
    }
    else
    {
        for (uint y = 0; y != heightAfterAggregation; ++y)
        {
            buffer << '\t' << (1 + y) << '\t';
            if (columns[0].rows)
            {
                AppendToBuffer(buffer, columns[0].rows[y]);
            }
            for (uint x = 1; x < width; ++x)
            {
                buffer << '\t';
                if (columns[x].rows)
                {
                    AppendToBuffer(buffer, columns[x].rows[y]);
                }

                if (buffer.size() > 1024 * 1024 * 8)
                {
                    file << buffer;
                    buffer.clear();
                }
            }
            buffer << '\n';
            ++Progress::Current;
        }
    }

    if (not buffer.empty())
    {
        file << buffer;
    }
    return true;
}
