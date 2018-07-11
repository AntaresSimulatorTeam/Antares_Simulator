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
#ifndef __YUNI_DBI_ROW_H__
# define __YUNI_DBI_ROW_H__

# include "column.h"


namespace Yuni
{
namespace DBI
{

	/*!
	** \ingroup DBI
	*/
	class Row final
	{
	public:
		//! \name Constructors
		//@{
		//! Default constructor
		explicit Row(::yn_dbi_adapter& adapter, void* handle);
		//! Copy constructor
		Row(const Row& rhs);
		//@}


		//! \name Columns
		//@{
		//! Get the Nth column
		Column column(uint nth);
		//@}


		//! \name Operators
		//@{
		//! Get the Nth column
		Column operator [] (uint nth);
		//@}


	private:
		//! Alias to the current channel
		// \internal This reference can be null and must never be called if pHandle is null
		::yn_dbi_adapter& pAdapter;
		//! Opaque pointer to the current query
		void* pHandle;

	}; // class Row





} // namespace DBI
} // namespace Yuni

# include "row.hxx"

#endif // __YUNI_DBI_ROW_H__
