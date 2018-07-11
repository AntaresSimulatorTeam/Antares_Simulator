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
#include "cursor.h"
#include <cassert>


namespace Yuni
{
namespace DBI
{


	DBI::Error Cursor::execute()
	{
		if (pHandle)
		{
			assert(pAdapter.query_execute and "invalid query_execute");
			return (DBI::Error) pAdapter.query_execute(pHandle);
		}

		return DBI::errNoTransaction;
	}


	DBI::Error Cursor::perform()
	{
		if (pHandle)
		{
			assert(pAdapter.query_perform_and_release and "invalid query_perform_and_release");

			// execute the query
			DBI::Error error = (DBI::Error) pAdapter.query_perform_and_release(pHandle);
			// we must consider that the query has been released
			pHandle = nullptr;
			return error;
		}
		return errNoQuery;
	}





} // namespace DBI
} // namespace Yuni
