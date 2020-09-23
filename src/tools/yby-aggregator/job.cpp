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

#include "job.h"
#include <antares/logs.h>
#include "progress.h"

using namespace Yuni;
using namespace Antares;


/*extern*/ Job::QueueService  queueService;
static Atomic::Int<> gNbJobs = 0;

#define SEP  IO::Separator


Yuni::Mutex gResultsMutex;


bool JobFileReader::RemainJobsToExecute()
{
	return 0 != gNbJobs;
}


JobFileReader::JobFileReader() :
	pVariablesOn(nullptr),
	pDataOffset((uint) -1),
	pTmpResults(nullptr),
	pLineCount(0u)
{
	++gNbJobs;
}


JobFileReader::~JobFileReader()
{
	// deallocation
	if (pTmpResults)
	{
		assert(!(!output));
		const uint nbVars = (uint) output->columns.size();
		for (uint i = 0; i != nbVars; ++i)
			delete[] pTmpResults[i];
		delete[] pTmpResults;
	}
	delete[] pVariablesOn;

	++Progress::Current;
	--gNbJobs;
}


void JobFileReader::onExecute()
{
	if (!datafile || !output || !path || !output->canContinue())
		return;
	if (!openCSVFile())
		return;
	if (!prepareJumpTable())
		return;
	if (pDataOffset == (uint) -1)
	{
		logs.error() << "invalid data offset";
		return;
	}
	if (!readRawData())
		return;
	if (!storeResults())
		return;
}


bool JobFileReader::openCSVFile()
{
	pFilename.clear();
	pFilename << path << SEP;
	studydata->append(pFilename);
	pFilename << SEP;
	datafile->append(pFilename);

	if (!pFile.open(pFilename))
	{
		// The error message will be disabled to allow invalid command line
		// parameters.

		// logs.error() << "I/O error: impossible to open " << pFilename;
		// high value to avoid the message 'too many errors'
		// Like this, the output will be completely disabled
		// output->errors = 99999;
		return false;
	}
	return true;
}



bool JobFileReader::readRawData()
{
	// The number of variables to fetch
	const uint nbVars = (uint) output->columns.size();

	// Allocating data for a temporary column
	// Before we can not properly assume the hieght of the column, we will
	// take the worst case scenario
	// There is no need to properly initilize this array, since the height
	// will be kept
	assert(!pTmpResults);
	pTmpResults = new TemporaryColumnData[nbVars];
	for (uint i = 0; i != nbVars; ++i)
		pTmpResults[i] = new CellData[maxRows];

	// A buffer when dealing with rows on several file buffers
	CString<1024> line;
	// The offset on the current file buffer
	uint offset = pDataOffset;
	// An adapter, for avoiding useless string copies
	AnyString adapter;
	// The total number of lines which have been found in the CSV file
	uint nbLines = 0;

	do
	{
		// Looking for the next end-of-line
		String::Size pos = buffer.find('\n', offset);
		if (pos < buffer.size())
		{
			adapter.adapt(buffer.c_str() + offset, pos - offset);
			if (!adapter.empty())
				readLine(adapter, nbLines);
			else
				logs.warning() << "Got an empty line at " << (pLineCount + 8) << ": " << pFilename;

			// Another line has been found
			++nbLines;
			offset = pos + 1;
		}
		else
		{
			// The end-of-line has not been found
			// If the offset is invalid, we can simply read the next file buffer,
			// and try again
			if (offset >= buffer.size())
			{
				if (!pFile.read(buffer, buffer.chunkSize))
					break;
				offset = 0;
			}
			else
			{
				// Otherwise, we have a line between two file chunks
				// We will keep the piece of line we have already read into
				// #line, another file chunk will be read, and we will try again
				line.clear();
				do
				{
					line.append(buffer.c_str() + offset, buffer.size() - offset);
					if (!pFile.read(buffer, buffer.chunkSize))
						break;
					pos = buffer.find('\n');
					offset = 0;
				}
				while (pos == String::npos);

				// Adding the final chunk to the line currently read...
				if (!buffer.empty())
					line.append(buffer.c_str(), pos);
				offset = pos + 1;

				// ... and analyzing it
				if (!line.empty())
					readLine(line, nbLines);
				else
					logs.warning() << "Got an empty line at " << (pLineCount + 8) << "*: " << pFilename;

				// Another line has been found
				++nbLines;

				// end-of-file
				if (!buffer)
					break;
			}
		}
	}
	while (true);

	pLineCount = nbLines;

	return true;
}




void JobFileReader::readLine(const AnyString& line, uint y)
{
	assert(not line.empty());
	assert(y < maxRows);

	if (y >= maxRows)
	{
		logs.error() << "Too many rows have been found (more than " << (uint)maxRows << ')';
		output->incrementError();
		return;
	}

	// The number of columns referenced in the jump table
	uint jumpTableSize = (uint) pJumpTable.size();
	// The current offset within the current line
	uint offset = 0;
	// The current column
	uint column = 0;
	// End of line
	bool eol = false;
	// The total number of variables
	const uint nbVars = (uint) output->columns.size();
	//
	AnyString adapter;

	do
	{
		// Dynamic Bound checking
		if (column >= jumpTableSize)
		{
			logs.warning() << "Too many columns line " << y << " in " << pFilename;
			break;
		}

		// Let's find the next separator
		String::Size pos = line.find('\t', offset);
		eol = (pos >= line.size());
		if (eol)
			pos = line.size();

		const uint mapping = pJumpTable[column];
		if (mapping != (uint) -1)
		{
			// The current column is related to data that we have to retrieve
			if (mapping < nbVars)
			{
				assert(offset < line.size());
				assert(pos <= line.size());
				assert(offset <= pos);

				adapter.adapt(line.c_str() + offset, pos - offset);
				if (!adapter)
					pTmpResults[mapping][y][0] = '\0';
				else
				{
					if (adapter.size() > maxSizePerCell - 1)
					{
						logs.warning() << "Content too long at line " << y << " column " << column << ": " << pFilename;
						pTmpResults[mapping][y][0] = '\0';
					}
					else
					{
						for (uint x = 0; x != adapter.size(); ++x)
							pTmpResults[mapping][y][x] = adapter.at(x);
						pTmpResults[mapping][y][adapter.size()] = '\0';
					}
				}
			}
			else
				logs.error() << "invalid column mapping";
		}

		++column;
		offset = pos + 1;
	}
	while (!eol);
}


bool JobFileReader::storeResults()
{
	if (!pLineCount)
		return false;

	Yuni::MutexLocker locker(gResultsMutex);

	// The total number of variables
	const uint nbVars = (uint) output->columns.size();

	ResultsForAllStudyItems& results = output->results;
	ResultsForAllDataLevels& alldatalevels = results[studydata->name];
	DataFile::Ptr& data = datafile;
	if (!data)
	{
		logs.error() << "invalid data";
		output->incrementError();
		return false;
	}
	ResultsForAllTimeLevels& alltimelevels = alldatalevels[data->dataLevel];
	ResultsAllVars& allvars = alltimelevels[data->timeLevel];

	for (uint v = 0; v != nbVars; ++v)
	{
		// This variable may not have been found in the CSV file
		if (!pVariablesOn[v])
			continue;

		ResultMatrix& var = allvars[v];
		if (year >= var.width)
		{
			logs.error() << "invalid year (got " << year << ", max: " << var.width << ")";
			output->incrementError();
			return false;
		}
		CellColumnData& store = var.columns[year];
		const TemporaryColumnData& ref = pTmpResults[v];

		// Allocate the memory for the result data
		if (Memory::Null(store.rows))
			Memory::Allocate(store.rows, pLineCount);
		else
			logs.error() << "internal error";

		// Copy
		store.height = pLineCount;
		Memory::Acquire(store.rows);
		assert(!Memory::StrictNull(store.rows));
		for (uint y = 0; y != pLineCount; ++y)
		{
			assert(!Memory::StrictNull(store.rows));
			memcpy(store.rows[y], ref[y], maxSizePerCell);
		}
		// Flush
		Memory::Flush(store.rows);
	}

	return true;
}



bool JobFileReader::prepareJumpTable()
{
	// Looking for the 5th line
	if (!pFile.read(buffer, buffer.chunkSize))
		return false;

	String::Size offset = 0;
	for (uint i = 0; i != 4; ++i)
	{
		const String::Size pos = buffer.find('\n', offset);
		if (pos == String::npos)
		{
			logs.error() << "invalid header in " << pFilename;
			output->incrementError();
			return false;
		}
		offset = pos + 1;
	}
	// Looking for the \n
	String::Size pos = buffer.find('\n', offset);
	if (pos == String::npos)
	{
		logs.error() << "invalid header in " << pFilename;
		output->incrementError();
		return false;
	}
	AnyString adapter(buffer.c_str() + offset, pos - offset);
	String::Vector list;
	adapter.split(list, "\t", true, false);
	if (list.size() < 3)
	{
		logs.error() << "invalid header in " << pFilename << " (not enough fields)";
		output->incrementError();
		return false;
	}

	pDataOffset = pos + 1;

	uint startIndex = 0;
	const DataFile::ShortString& timeLevel = datafile->timeLevel;
	for (uint i = 0; i != list.size(); ++i)
	{
		if (timeLevel == list[i])
		{
			startIndex = i + 1;
			break;
		}
	}
	if (startIndex >= list.size())
	{
		logs.error() << "invalid header in " << pFilename << " (invalid time level)";
		output->incrementError();
		return false;
	}

	pVariablesOn = new bool[output->columns.size()];
	for (uint i = 0; i != output->columns.size(); ++i)
		pVariablesOn[i] = false;

	// Mapping
	resizeJumpTable((uint) list.size());
	uint jumpFound = 0;
	for (uint i = startIndex; i != list.size(); ++i)
	{
		String& entry = list[i];
		if (!entry)
			continue;
		entry.trim(" \r");
		if (!entry)
			continue;
		entry.toLower();

		uint columnCount = (uint) output->columns.size();
		for (uint j = 0; j != columnCount; ++j)
		{
			if (output->columns[j] == entry)
			{
				if (!jumpFound)
				{
					pJumpTable[i] = j;
					pVariablesOn[j] = true;
					++jumpFound;
				}
				break;
			}
		}
	}

	if (!jumpFound)
		return false;

	// Skip the next line
	++pos;
	for (uint s = 0; s != 2; ++s)
	{
		pos = buffer.find('\n', pos);
		if (pos == String::npos)
			return false;
		++pos;
	}

	pDataOffset = pos;
	return true;
}


