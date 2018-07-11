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
#include "../../yuni.h"



namespace Yuni
{
namespace System
{

	/*!
	** \brief Retrieves the calling user's name
	**
	** On Unixes, the value will be read from the environment variables
	** `LOGNAME`. On Windows, the method GetUserName will be used.
	**
	** Windows (from MSDN):
	** If the current thread is impersonating another client, the GetUserName
	** function returns the user name of the client that the thread is impersonating.
	** \see http://msdn.microsoft.com/en-us/library/ms724432%28v=vs.85%29.aspx
	**
	** \param out Variable of type 'string' where the value will be appened
	** \param emptyBefore True to empty the parameter `out` before
	** \return True if the operation succeeded (a valid username has been found), false otherwise
	*/
	template<class StringT> YUNI_DECL bool Username(StringT& out, bool emptyBefore = true);



} // namespace System
} // namespace Yuni

# include "username.hxx"

