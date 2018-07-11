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
#include "errors.h"


namespace Yuni
{
namespace Net
{

	const char* ErrorToCString(Error error)
	{
		switch (error)
		{
			case errNone:
				return "no error";
			case errInvalidPort:
				return "invalid port";
			case errInvalidHostAddress:
				return "invalid host address";
			case errDupplicatedAddress:
				return "address dupplicated";
			case errNoTransport:
				return "no transport available";
			case errInvalidTransport:
				return "invalid transport";
			case errStartFailed:
				return "impossible to start the server";
			case errUnknown:
			case errMax:
				break;
		}
		return "<unknown>";
	}



} // namespace Net
} // namespace Yuni

