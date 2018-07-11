/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once



namespace Yuni
{
namespace IO
{

	/*!
	** \brief Error list
	*/
	enum Error
	{
		//! No error
		errNone = 0,
		//! Generic error
		errUnknown,
		//! Bad filename
		errBadFilename,
		//! The file could not be loaded - not found or permission error
		errNotFound,
		//! A hard memory limit has been reached
		errMemoryLimit,
		//! It is impossible to overwrite an existing file
		errOverwriteNotAllowed,
		//! Impossible to write
		errWriteFailed,
		//! Impossible to read
		errReadFailed,
	};




} // namespace IO
} // namespace Yuni
