// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_VACUUM_IO_H__
#define __ANTARES_VACUUM_IO_H__

#include <yuni/io/file.h>
#ifndef YUNI_OS_WINDOWS
#include <unistd.h>
#else
#include <yuni/core/string/wstring.h>
#include <yuni/core/system/windows.hdr.h>
#endif
#include <atomic>
#include <unordered_set>

//! Flag to determine whether we are in dry mode or not
extern bool dry;

//! Mutex for getting/setting statistics

extern std::atomic<int> IOBytesDeleted;
extern std::atomic<int> IOFilesDeleted;
extern std::atomic<int> IOFoldersDeleted;

//! All inputs folders, which can not be removed
extern std::unordered_set<YString> inputFolders;

/*!
** \brief Remove a file
**
** No I/O operations will be performed if in dry mode
*/
bool RemoveFile(const YString& filename, uint64_t size);

/*!
** \brief Try to remove a folder if empty
**
** No I/O operations will be performed if in dry mode
*/
bool RemoveDirectoryIfEmpty(const YString& folder);

#endif // __ANTARES_VACUUM_IO_H__
