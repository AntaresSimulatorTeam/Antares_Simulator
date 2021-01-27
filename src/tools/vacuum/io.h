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
#ifndef __ANTARES_VACUUM_IO_H__
#define __ANTARES_VACUUM_IO_H__

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#ifndef YUNI_OS_WINDOWS
#include <unistd.h>
#else
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/string/wstring.h>
#endif
#include <unordered_set>

//! Flag to determine whether we are in dry mode or not
extern bool dry;

//! Mutex for getting/setting statistics

extern Yuni::Atomic::Int<> IOBytesDeleted;
extern Yuni::Atomic::Int<> IOFilesDeleted;
extern Yuni::Atomic::Int<> IOFoldersDeleted;

//! All inputs folders, which can not be removed
extern std::unordered_set<YString> inputFolders;

/*!
** \brief Remove a file
**
** No I/O operations will be performed if in dry mode
*/
bool RemoveFile(const YString& filename, yuint64 size);

/*!
** \brief Try to remove a folder if empty
**
** No I/O operations will be performed if in dry mode
*/
bool RemoveDirectoryIfEmpty(const YString& folder);

#endif // __ANTARES_VACUUM_IO_H__
