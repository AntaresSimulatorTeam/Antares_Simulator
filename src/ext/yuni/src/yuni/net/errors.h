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
#ifndef __YUNI_NET_ERRORS_H__
# define __YUNI_NET_ERRORS_H__

# include "../yuni.h"
# include "../core/string.h"


namespace Yuni
{
namespace Net
{

	/*!
	** \brief Errors that can be returned by routines in the \p 'Yuni::Net' module
	*/
	enum Error
	{
		//! No error
		errNone = 0,
		//! Unknown error
		errUnknown,
		//! The port is invalid (range [1..65535])
		errInvalidPort,
		//! The address is invalid
		errInvalidHostAddress,
		//! The given address already exists in the list
		errDupplicatedAddress,
		//! No transport layer
		errNoTransport,
		//! Invalid transport layer
		errInvalidTransport,
		//! Impossible to start web server
		errStartFailed,
		//! The maximum number of errors
		errMax
	};



	/*!
	** \brief Convert an error to a human readable string
	**
	** For standard uses, you would prefer using Yuni::String
	*/
	const char* ErrorToCString(Error error);


} // namespace Net
} // namespace Yuni




namespace Yuni
{
namespace Extension
{
namespace CString
{

	template<class CStringT>
	class Append<CStringT, Yuni::Net::Error>
	{
	public:
		static inline void Perform(CStringT& string, Yuni::Net::Error error)
		{
			string += Yuni::Net::ErrorToCString(error);
		}
	};



} // namespace CString
} // namespace Extension
} // namespace Yuni


#endif // __YUNI_NET_ERRORS_H__
