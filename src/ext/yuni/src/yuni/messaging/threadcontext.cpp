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
#include "threadcontext.h"
#include "../net/http-status-code.h"


namespace Yuni
{
namespace Messaging
{

	Context::Context(Service& service) :
		userdata(nullptr),
		httpStatus(),
		remotePort(),
		service(service),
		httpStatusCode()
	{
		HttpStatusCode& codes = const_cast<HttpStatusCode&>(httpStatusCode);

		// 2xx
		for (uint i = 200; i <= codes.max2xx; ++i)
			codes.header2xx[i - 200] << "HTTP/1.1 " << i << ' ' << Net::HttpStatusCodeToCString(i) << "\r\n";

		// 4xx
		for (uint i = 400; i <= codes.max4xx; ++i)
		{
			codes.header4xx[i - 400] << "HTTP/1.1 " << i << ' '
				<< Net::HttpStatusCodeToCString(i) << "\r\n";

			codes.response4xx[i - 400] << codes.header4xx[i - 400]
				<< "\r\nCache: no-cache\r\n\r\n"
				<< Net::HttpStatusCodeToCString(i) << "\r\n";
		}

		// 5xx
		for (uint i = 500; i <= codes.max5xx; ++i)
		{
			codes.header5xx[i - 500] << "HTTP/1.1 " << i << ' '
				<< Net::HttpStatusCodeToCString(i) << "\r\n";

			codes.response5xx[i - 500] << codes.header5xx[i - 500]
				<< "\r\nCache: no-cache\r\n\r\n"
				<< Net::HttpStatusCodeToCString(i) << "\r\n";
		}
	}





} // namespace Messaging
} // namespace Yuni

