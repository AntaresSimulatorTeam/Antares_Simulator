/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_VACUUM_IO_H__
#define __ANTARES_VACUUM_IO_H__

#include <yuni/io/file.h>
#ifndef YUNI_OS_WINDOWS
#include <unistd.h>
#else
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/string/wstring.h>
#endif
#include <unordered_set>

#include <atomic>

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
