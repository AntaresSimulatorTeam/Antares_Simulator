// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_IO_STATISTICS_H__
#define __ANTARES_LIBS_IO_STATISTICS_H__

#include <yuni/yuni.h>

namespace Antares::Statistics
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

} // namespace Antares::Statistics

#endif // __ANTARES_LIBS_IO_STATISTICS_H__
