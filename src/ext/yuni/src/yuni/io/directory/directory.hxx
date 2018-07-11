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
#include "directory.h"
#include "../../core/traits/cstring.h"
#include "../../core/traits/length.h"
#include "../../core/static/remove.h"
#include "../constants.h"
#ifdef YUNI_HAS_STDLIB_H
#	include <stdlib.h>
#endif




namespace Yuni
{
namespace Private
{
namespace IO
{
namespace Directory
{

	inline bool DummyCopyUpdateEvent(Yuni::IO::Directory::CopyState, const String&, const String&, uint64, uint64)
	{
		return true;
	}



} // namespace Directory
} // namespace IO
} // namespace Private
} // namespace Yuni



namespace Yuni
{
namespace IO
{
namespace Directory
{


	inline bool Exists(const AnyString& path)
	{
		return (IO::typeFolder == IO::TypeOf(path));
	}


	inline bool Copy(const AnyString& source, const AnyString& destination, bool recursive, bool overwrite)
	{
		CopyOnUpdateBind e;
		e.bind(&Private::IO::Directory::DummyCopyUpdateEvent);
		return Copy(source, destination, recursive, overwrite, e);
	}


	inline bool Copy(const AnyString& source, const AnyString& destination, const CopyOnUpdateBind& onUpdate)
	{
		return Copy(source, destination, true, true, onUpdate);
	}




} // namespace Directory
} // namespace IO
} // namespace Yuni
