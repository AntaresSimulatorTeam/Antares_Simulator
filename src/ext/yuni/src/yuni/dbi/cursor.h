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
#ifndef __YUNI_DBI_CURSOR_H__
# define __YUNI_DBI_CURSOR_H__

# include "../yuni.h"
# include "../core/string.h"
# include "../core/noncopyable.h"
# include "adapter/entries.h"
# include "row.h"


namespace Yuni
{
namespace DBI
{

	/*!
	** \ingroup DBI
	*/
	class Cursor final : private Yuni::NonCopyable<Cursor>
	{
	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Cursor(::yn_dbi_adapter& adapter, void* handle);
		/*!
		** \brief Move constructor
		*/
		Cursor(Cursor&& other);
		//! Destructor
		~Cursor();
		//@}


		//! \name Bindings
		//@{
		//! Bind a sepcific parameter as a string
		Cursor& bind(uint index, const AnyString& value);
		//! Bind a sepcific parameter as a bool
		Cursor& bind(uint index, bool value);
		//! Bind a sepcific parameter as a sint32
		Cursor& bind(uint index, sint32 value);
		//! Bind a sepcific parameter as a sint64
		Cursor& bind(uint index, sint64 value);
		//! Bind a sepcific parameter as a double
		Cursor& bind(uint index, double value);
		//! Bind a sepcific parameter as a null
		Cursor& bind(uint index, const NullPtr&);

		//! Convenient method for binding the first 1 parameter
		template<class A1>
		Cursor& map(const A1& a1);
		//! Convenient method for binding the first 2 parameters at once
		template<class A1, class A2>
		Cursor& map(const A1& a1, const A2& a2);
		//! Convenient method for binding the first 3 parameters at once
		template<class A1, class A2, class A3>
		Cursor& map(const A1& a1, const A2& a2, const A3& a3);
		//! Convenient method for binding the first 4 parameters at once
		template<class A1, class A2, class A3, class A4>
		Cursor& map(const A1& a1, const A2& a2, const A3& a3, const A4& a4);
		//@}


		//! \name Execution
		//@{
		/*!
		** \brief Execute the query but discard the results
		*/
		DBI::Error perform();

		/*!
		** \brief Execute the query
		*/
		DBI::Error execute();
		//@}


		//! \name Resultset
		//@{
		/*!
		** \brief Fetch the current row
		*/
		Row fetch();

		/*!
		** \brief Iterate over all rows in the resultset
		*/
		template<class CallbackT> DBI::Error each(const CallbackT& callback);

		/*!
		** \brief Go to the next row
		*/
		DBI::Error next();

		/*!
		** \brief Go to the previous row
		*/
		DBI::Error previous();

		/*!
		** \brief Move the cursor to a specific row index in the result set
		*/
		DBI::Error moveTo(uint64 rowindex);
		//@}


	private:
		//! Alias to the current channel
		// \internal This reference can be null and must never be called if pHandle is null
		::yn_dbi_adapter& pAdapter;
		//! Opaque pointer to the current query
		void* pHandle;

	}; // class Cursor





} // namespace DBI
} // namespace Yuni

# include "cursor.hxx"

#endif // __YUNI_DBI_CURSOR_H__
