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
#ifndef __YUNI_NET_HTTP_STATUS_CODE_H__
# define __YUNI_NET_HTTP_STATUS_CODE_H__

# include "../yuni.h"


namespace Yuni
{
namespace Net
{

	/*!
	** \brief Convert an http status code into a human-readable diagnostic information
	**
	** \code
	** std::cout << Net::HttpStatusCodeToCString(500) << std::endl; // Internal error
	** \endcode
	*/
	const char* HttpStatusCodeToCString(uint code);




} // namespace Net
} // namespace Yuni

#endif // __YUNI_NET_HTTP_STATUS_CODE_H__
