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
#ifndef __YUNI_DBI_COLUMN_H__
# define __YUNI_DBI_COLUMN_H__



namespace Yuni
{
namespace DBI
{

	/*!
	** \ingroup DBI
	*/
	class Column final
	{
	public:
		//! \name Constructors
		//@{
		//! Default constructor
		explicit Column(::yn_dbi_adapter& adapter, void* handle, uint colindx);
		//! Copy constructor
		Column(const Column& rhs);
		//@}


		//! Get if the value is null
		bool null() const;

		//! Convert the value of the column as bool
		bool asBool() const;

		//! Convert the value of the column as int32
		sint32 asInt() const;

		//! Convert the value of the column as int64
		sint64 asInt64() const;

		//! Convert the value of the column as a double
		double asDouble() const;

		//! Convert the value of the column as a string
		String asString() const;

		/*!
		** \brief Convert the value of the column into a specific type
		**
		** If the convertion is not a standard one, the conversion from
		** a string to this type will be used.
		*/
		template<class T> T as() const;

		/*!
		** \brief Append the value of the column to an existing string
		*/
		template<class StringT> void appendToString(StringT& string);


	private:
		//! Alias to the current channel
		// \internal This reference can be null and must never be called if pHandle is null
		::yn_dbi_adapter& pAdapter;
		//! Opaque pointer to the current query
		void* pHandle;
		//! Column index
		uint pColumnIndex;

	}; // class Column





} // namespace DBI
} // namespace Yuni

# include "column.hxx"

#endif // __YUNI_DBI_COLUMN_H__
