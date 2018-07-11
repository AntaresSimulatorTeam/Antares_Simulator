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
#include "../yuni.h"
#include "../core/string.h"
#include "error.h"
#include "constants.h"



namespace Yuni
{
/*
** \brief Low-level routines for file and directory support
*/
namespace IO
{


	/*!
	** \brief Test if a node exists (whatever its type, a folder or a file)
	**
	** \param filename The file/directory to test (must be zero-terminated)
	** \return True if it exists, false otherwise
	*/
	bool Exists(const AnyString& filename);

	/*!
	** \brief Get the type of a node
	**
	** \param filename The file/directory to test (must be zero-terminated)
	** \param followSymLink True to get information about the real file
	** \return True if it exists, false otherwise
	*/
	NodeType TypeOf(const AnyString& filename, bool followSymLink = true);


	/*!
	** \brief Get the type of a node and directly retrieve its size
	**
	** \see TypeOf()
	** \param filename The file/directory to test (must be zero-terminated)
	** \param[out] size The size of the node (reset to zero if not found)
	** \param followSymLink True to get information about the real file
	** \return True if it exists, false otherwise
	*/
	NodeType FetchFileStatus(const AnyString& filename, yuint64& size, yint64& lastModified, bool followSymLink = true);







} // namespace IO
} // namespace Yuni

#include "filename-manipulation.h"
#include "directory.h"
#include "io.hxx"

