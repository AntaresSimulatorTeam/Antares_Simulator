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
#include "version.h"
#include <iostream>



namespace Yuni
{

	void Version::InternalLib(Version& v)
	{
		v.hi = YUNI_VERSION_HI;
		v.lo = YUNI_VERSION_LO;
		v.revision = YUNI_VERSION_REV;
	}


	bool Version::isLessThan(const Version& rhs) const
	{
		if (hi < rhs.hi)
			return true;
		if (hi == rhs.hi)
		{
			if (lo < rhs.lo)
				return true;
			if (lo == rhs.lo)
				return revision < rhs.revision;
		}
		return false;
	}


	bool Version::isGreaterThan(const Version& rhs) const
	{
		if (hi > rhs.hi)
			return true;
		if (hi == rhs.hi)
		{
			if (lo > rhs.lo)
				return true;
			if (lo == rhs.lo)
				return revision > rhs.revision;
		}
		return false;
	}


	String Version::toString() const
	{
		return String() << hi << '.' << lo << '.' << revision;
	}

	std::ostream& operator << (std::ostream& out, const Yuni::Version& rhs)
	{
		rhs.print(out);
		return out;
	}



} // namespace Yuni
