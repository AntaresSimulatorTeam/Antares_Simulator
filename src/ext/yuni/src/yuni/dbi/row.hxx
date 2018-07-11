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
#ifndef __YUNI_DBI_ROW_HXX__
# define __YUNI_DBI_ROW_HXX__



namespace Yuni
{
namespace DBI
{

	inline Row::Row(::yn_dbi_adapter& adapter, void* handle) :
		pAdapter(adapter),
		pHandle(handle)
	{}

	inline Row::Row(const Row& rhs) :
		pAdapter(rhs.pAdapter),
		pHandle(rhs.pHandle)
	{}


	inline Column Row::column(uint nth)
	{
		return Column(pAdapter, pHandle, nth);
	}


	inline Column Row::operator [] (uint nth)
	{
		return Column(pAdapter, pHandle, nth);
	}





} // namespace DBI
} // namespace Yuni

#endif // __YUNI_DBI_ROW_HXX__
