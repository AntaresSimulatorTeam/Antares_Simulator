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

#include "result.h"
#include <antares/memory/memory.h>
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




CellColumnData::CellColumnData() :
	rows(nullptr),
	height(0)
{}


CellColumnData::CellColumnData(const CellColumnData&)
{}


CellColumnData::~CellColumnData()
{}


ResultMatrix::ResultMatrix() :
	columns(nullptr),
	width(0),
	heightAfterAggregation(0)
{}


ResultMatrix::ResultMatrix(const ResultMatrix&) :
	columns(NULL),
	width(0),
	heightAfterAggregation(0)
{}


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
		return false;

	Progress::Total = heightAfterAggregation;
	String buffer;
	buffer.reserve(8 * 1024 * 1024);

	// Writing pseudo header like any other CSV in antares
	{
		// buffer
		//	<< "Alpes\tarea\tva\thourly\n"
		//	<< "\tVARIABLES\tBEGIN\tEND\n"
		//	<< "\t78\t1\t8736\n";
		buffer
			<< "mc-var\taggregate\n"
			<< "\t\tBEGIN\tEND\n"
			<< "\t\t1\t" << heightAfterAggregation << '\n';
		buffer << '\n';
		{
			buffer << '\t';
			for (uint i = 0; i < width; ++i)
				buffer << '\t' << "year";
			buffer << '\n';

			buffer << '\t';
			for (uint i = 0; i < width; ++i)
				buffer << '\t' << (i + 1);
			buffer << '\n';
		}
		for (uint r = 0; r != 1; ++r)
		{
			buffer << '\t';
			for (uint i = 0; i < width; ++i)
				buffer << '\t';
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
			dataBuffer[d] = new CellData[width];

		uint flush = 1000;
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
							memcpy(dataBuffer[offset][x], columns[x].rows[offset + y], maxSizePerCell);
						}
					}
					else
					{
						for (uint offset = 0; offset != dataBufferHeight; ++offset)
						{
							assert(offset + y < columns[x].height);
							memcpy(dataBuffer[offset][x], columns[x].rows[offset + y], maxSizePerCell);
						}
					}

					if (!--flush)
					{
						flush = 1000;
						memory.flushAll();
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
				dataBufferOffset = 0;
		}

		for (uint d = 0; d != dataBufferHeight; ++d)
			delete[] dataBuffer[d];
	}
	else
	{
		for (uint y = 0; y != heightAfterAggregation; ++y)
		{
			buffer << '\t' << (1 + y) << '\t';
			//if (columns[0].rows.valid())
				AppendToBuffer(buffer, columns[0].rows[y]);
			for (uint x = 1; x < width; ++x)
			{
				buffer << '\t';
				//if (columns[x].rows.valid())
					AppendToBuffer(buffer, columns[x].rows[y]);

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
		file << buffer;
	return true;
}


