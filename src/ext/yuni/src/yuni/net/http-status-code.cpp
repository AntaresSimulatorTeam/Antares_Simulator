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
#include "http-status-code.h"


namespace Yuni
{
namespace Net
{

	const char* HttpStatusCodeToCString(uint code)
	{
		switch (code)
		{
			// 2xx
			case 200: return "OK";
			case 201: return "CREATED";
			case 202: return "Accepted";
			case 203: return "Partial information";
			case 204: return "No response";
			// 4xx
			case 400: return "Bad request";
			case 401: return "Unauthorized";
			case 402: return "PaymentRequired";
			case 403: return "Forbidden";
			case 404: return "Not found";
			// 5xx
			case 500: return "Internal Error";
		}
		return nullptr;
	}





} // namespace Net
} // namespace Yuni

