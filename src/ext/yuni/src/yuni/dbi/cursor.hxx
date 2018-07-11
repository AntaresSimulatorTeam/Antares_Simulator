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
#ifndef __YUNI_DBI_CURSOR_HXX__
# define __YUNI_DBI_CURSOR_HXX__


namespace Yuni
{
namespace DBI
{

	inline Cursor::Cursor(::yn_dbi_adapter& adapter, void* handle) :
		pAdapter(adapter),
		pHandle(handle)
	{}


	inline Cursor::Cursor(Cursor&& other) :
		pAdapter(other.pAdapter),
		pHandle(other.pHandle)
	{
		other.pHandle = nullptr;
	}


	inline Cursor::~Cursor()
	{
		if (pHandle)
			pAdapter.query_ref_release(pHandle);
	}


	inline Cursor& Cursor::bind(uint index, const AnyString& value)
	{
		if (pHandle)
			pAdapter.bind_str(pHandle, index, value.c_str(), value.size());
		return *this;
	}


	inline Cursor& Cursor::bind(uint index, bool value)
	{
		if (pHandle)
			pAdapter.bind_bool(pHandle, index, (int) value);
		return *this;
	}


	inline Cursor& Cursor::bind(uint index, sint32 value)
	{
		if (pHandle)
			pAdapter.bind_int32(pHandle, index, value);
		return *this;
	}


	inline Cursor& Cursor::bind(uint index, sint64 value)
	{
		if (pHandle)
			pAdapter.bind_int64(pHandle, index, value);
		return *this;
	}


	inline Cursor& Cursor::bind(uint index, double value)
	{
		if (pHandle)
			pAdapter.bind_double(pHandle, index, value);
		return *this;
	}


	inline Cursor& Cursor::bind(uint index, const NullPtr&)
	{
		if (pHandle)
			pAdapter.bind_null(pHandle, index);
		return *this;
	}


	template<class A1>
	inline Cursor& Cursor::map(const A1& a1)
	{
		(void) bind(0, a1);
		return *this;
	}


	template<class A1, class A2>
	inline Cursor& Cursor::map(const A1& a1, const A2& a2)
	{
		(void) bind(0, a1);
		(void) bind(1, a2);
		return *this;
	}


	template<class A1, class A2, class A3>
	inline Cursor& Cursor::map(const A1& a1, const A2& a2, const A3& a3)
	{
		(void) bind(0, a1);
		(void) bind(1, a2);
		(void) bind(2, a3);
		return *this;
	}


	template<class A1, class A2, class A3, class A4>
	inline Cursor& Cursor::map(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
	{
		(void) bind(0, a1);
		(void) bind(1, a2);
		(void) bind(2, a3);
		(void) bind(3, a4);
		return *this;
	}


	inline DBI::Error Cursor::next()
	{
		assert(pAdapter.cursor_go_to_next != NULL);
		return (pHandle)
			? (DBI::Error) pAdapter.cursor_go_to_next(pHandle)
			: DBI::errNoRow;
	}


	inline DBI::Error Cursor::previous()
	{
		assert(pAdapter.cursor_go_to_previous != NULL);
		return (pHandle)
			? (DBI::Error) pAdapter.cursor_go_to_previous(pHandle)
			: DBI::errNoRow;
	}


	inline Row Cursor::fetch()
	{
		return Row(pAdapter, pHandle);
	}


	inline DBI::Error Cursor::moveTo(uint64 rowindex)
	{
		assert(pAdapter.cursor_go_to != NULL);
		return (pHandle)
			? (DBI::Error) pAdapter.cursor_go_to(pHandle, rowindex)
			: DBI::errNoRow;
	}


	template<class CallbackT>
	inline DBI::Error Cursor::each(const CallbackT& callback)
	{
		do
		{
			// go to the next statement
			auto error = next();

			switch (error)
			{
				case errNone:
					{
						// Got another row !
						Row row(pAdapter, pHandle);
						if (not callback(row))
							return errNone; // asked to stop
						break;
					}
				case errNoRow:
					{
						// no more row
						return errNone;
					}
				default:
					{
						// another error ! Strange however...
						return error;
					}
			}
		}
		while (true);

		// generic error, which should never happen here
		return errFailed;
	}




} // namespace DBI
} // namespace Yuni

#endif // __YUNI_DBI_CURSOR_HXX__
