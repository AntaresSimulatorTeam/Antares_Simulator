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
#include "file.h"
#include "../../core/traits/cstring.h"
#include "../../core/traits/length.h"
#include "../../core/static/remove.h"




namespace Yuni
{
namespace IO
{
namespace File
{


	inline uint64 Size(const AnyString& filename)
	{
		uint64 size;
		return (Size(filename, size)) ? size : 0;
	}


	inline bool Exists(const AnyString& filename)
	{
		Yuni::IO::NodeType type = Yuni::IO::TypeOf(filename);
		return (Yuni::IO::typeFolder != type and Yuni::IO::typeUnknown != type);
	}


	template<class U>
	bool SetContent(const AnyString& filename, const U& content)
	{
		IO::File::Stream file(filename, OpenMode::write | OpenMode::truncate);
		if (file.opened())
		{
			file += content;
			return true;
		}
		return false;
	}


	template<class U>
	bool AppendContent(const AnyString& filename, const U& content)
	{
		IO::File::Stream file(filename, OpenMode::write | OpenMode::append);
		if (file.opened())
		{
			file += content;
			return true;
		}
		return false;
	}



	template<class U>
	bool SaveToFile(const AnyString& filename, const U& content)
	{
		IO::File::Stream file;
		if (file.openRW(filename))
		{
			file += content;
			return true;
		}
		return false;
	}



	template<class PredicateT>
	bool
	ReadLineByLine(const AnyString& filename, const PredicateT& predicate)
	{
		IO::File::Stream file;
		if (file.open(filename))
		{
			String line;
			while (file.readline<4096u, String>(line))
				predicate(line);

			return true;
		}
		return false;
	}






} // namespace File
} // namespace IO
} // namespace Yuni
