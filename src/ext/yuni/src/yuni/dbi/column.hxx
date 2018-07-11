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
#ifndef __YUNI_DBI_COLUMN_HXX__
# define __YUNI_DBI_COLUMN_HXX__



namespace Yuni
{
namespace DBI
{

	inline Column::Column(::yn_dbi_adapter& adapter, void* handle, uint colindx) :
		pAdapter(adapter),
		pHandle(handle),
		pColumnIndex(colindx)
	{}


	inline Column::Column(const Column& rhs) :
		pAdapter(rhs.pAdapter),
		pHandle(rhs.pHandle),
		pColumnIndex(rhs.pColumnIndex)
	{}


	inline bool Column::null() const
	{
		return (0 != pAdapter.column_is_null(pHandle, pColumnIndex));
	}


	inline String Column::asString() const
	{
		uint length;
		const char* text = pAdapter.column_to_cstring(pHandle, pColumnIndex, &length);
		return String(text, length);
	}


	template<class StringT>
	inline void Column::appendToString(StringT& string)
	{
		uint length;
		const char* text = pAdapter.column_to_cstring(pHandle, pColumnIndex, &length);
		string.append(text, length);
	}


	inline bool Column::asBool() const
	{
		return (0 != pAdapter.column_to_int32(pHandle, pColumnIndex));
	}


	inline double Column::asDouble() const
	{
		return pAdapter.column_to_double(pHandle, pColumnIndex);
	}


	inline sint32 Column::asInt() const
	{
		return pAdapter.column_to_int32(pHandle, pColumnIndex);
	}


	inline sint64 Column::asInt64() const
	{
		return pAdapter.column_to_int64(pHandle, pColumnIndex);
	}




	template<class T>
	inline T Column::as() const
	{
		const String& text = asString();
		return text.to<T>();
	}


	template<> inline String Column::as<String>() const
	{
		return asString();
	}

	template<> inline bool Column::as<bool>() const
	{
		return asBool();
	}

	template<> inline double Column::as<double>() const
	{
		return asDouble();
	}

	template<> inline float Column::as<float>() const
	{
		return (float) asDouble();
	}

	template<> inline sint32 Column::as<sint32>() const
	{
		return asInt();
	}

	template<> inline sint64 Column::as<sint64>() const
	{
		return asInt64();
	}




} // namespace DBI
} // namespace Yuni

#endif // __YUNI_DBI_COLUMN_HXX__
