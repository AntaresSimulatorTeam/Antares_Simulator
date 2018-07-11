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
#include "string.h"
#include <iostream>


namespace Yuni
{

	const String emptyString;



	std::ostream& operator << (std::ostream& out, const AnyString& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::ShortString16& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::ShortString32& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::ShortString64& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::ShortString128& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::ShortString256& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::ShortString512& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::Clob& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::String& rhs)
	{
		if (not rhs.empty())
			out.write(rhs.data(), static_cast<std::streamsize>(rhs.size()));
		return out;
	}


	std::ostream& operator << (std::ostream& out, const Yuni::UTF8::Char& rhs)
	{
		rhs.write(out);
		return out;
	}



} // namespace Yuni
