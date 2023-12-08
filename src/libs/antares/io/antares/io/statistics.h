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
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_LIBS_IO_STATISTICS_H__
#define __ANTARES_LIBS_IO_STATISTICS_H__

#include <yuni/yuni.h>

namespace Antares
{
namespace Statistics
{
/*!
** \brief Get the amount of data (in bytes) which have been read from the disk
*/
uint64_t ReadFromDisk();

/*!
** \brief Get the amount of data (in bytes) which have been written to the disk
*/
uint64_t WrittenToDisk();

/*!
** \brief Get the amount of data (in bytes) which have been read from the disk since the startup
*/
uint64_t ReadFromDiskSinceStartup();

/*!
** \brief Get the amount of data (in bytes) which have been written to the disk since the startup
*/
uint64_t WrittenToDiskSinceStartup();

/*!
** \brief Get the amount of data (in bytes) which have been read from the network
*/
uint64_t ReadFromNetwork();

/*!
** \brief Get the amount of data (in bytes) which have been written to the network
*/
uint64_t WrittenToNetwork();

/*!
** \brief Get the amount of data (in bytes) which have been read from the network since the startup
*/
uint64_t ReadFromNetworkSinceStartup();

/*!
** \brief Get the amount of data (in bytes) which have been written to the network since the startup
*/
uint64_t WrittenToNetworkSinceStartup();

/*!
** \brief Notify taht a certain amount of data (in bytes) has been read from disk
*/
void HasReadFromDisk(uint64_t size);

/*!
** \brief Notify taht a certain amount of data (in bytes) has been written to disk
*/
void HasWrittenToDisk(uint64_t size);

/*!
** \brief Notify taht a certain amount of data (in bytes) has been read from the network
*/
void HasReadFromNetwork(uint64_t size);

/*!
** \brief Notify taht a certain amount of data (in bytes) has been sent to the network
*/
void HasWrittenToNetwork(uint64_t size);

/*!
** \brief Reset the internal counters
*/
void Reset();

/*!
** \brief Dump the statistic to the logs
*/
void DumpToLogs();

class Updater
{
public:
    Updater()
    {
        Reset();
    }

    ~Updater()
    {
        DumpToLogs();
    }
};

class LogsDumper
{
public:
    LogsDumper()
    {
    }

    ~LogsDumper()
    {
        DumpToLogs();
    }
};

} // namespace Statistics
} // namespace Antares

#endif // __ANTARES_LIBS_IO_STATISTICS_H__
