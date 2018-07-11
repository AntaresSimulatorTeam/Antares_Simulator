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
namespace OpenMode
{


	enum Type
	{
		none = 0,
		//! Open the file for reading
		read = 1,
		//! Open the file for writing. It will be created if it does not exist.
		write = 2,
		//! (write mode) The stream is positioned at the end of the file
		append = 4,
		//! (write mode) Truncte the file if it already exists
		truncate = 8,
	};



	/*!
	** \brief Convert an open mode into a C-String for 'fopen'
	*/
	const char* ToCString(int mode);

	/*!
	** \brief Convert an open mode into a C-String for 'fwopen'
	*/
	const wchar_t*  ToWCString(int mode);





} // namespace OpenMode
} // namespace IO
} // namespace Yuni
