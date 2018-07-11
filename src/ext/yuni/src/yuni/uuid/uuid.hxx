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
#include "uuid.h"




namespace Yuni
{

	template<class StringT>
	inline UUID::UUID(const StringT& string)
	{
		if (not assign(string))
			clear();
	}


	inline void UUID::clear()
	{
		pValue.n64[0] = 0;
		pValue.n64[1] = 0;
	}


	inline UUID::UUID()
	{
		pValue.n64[0] = 0;
		pValue.n64[1] = 0;
	}


	inline UUID::UUID(const UUID& rhs)
	{
		pValue.n64[0] = rhs.pValue.n64[0];
		pValue.n64[1] = rhs.pValue.n64[1];
	}


	inline UUID::UUID(Flag flag)
	{
		switch (flag)
		{
			case fGenerate: generate(); break;
			case fNull: clear(); break;
		}
	}


	inline bool UUID::operator ! () const
	{
		return null();
	}


	template<class StringT>
	inline UUID& UUID::operator = (const StringT& string)
	{
		if (not assign(string))
			clear();
		return *this;
	}


	inline bool UUID::null() const
	{
		return  (0 == pValue.n64[0]) and (0 == pValue.n64[1]);
	}


	inline UUID& UUID::operator = (const UUID& rhs)
	{
		pValue.n64[0] = rhs.pValue.n64[0];
		pValue.n64[1] = rhs.pValue.n64[1];
		return *this;
	}


	inline bool UUID::operator == (const UUID& rhs) const
	{
		return  (pValue.n64[0] == rhs.pValue.n64[0])
			and (pValue.n64[1] == rhs.pValue.n64[1]);
	}


	inline bool UUID::operator != (const UUID& rhs) const
	{
		return (not operator == (rhs));
	}


	inline bool UUID::operator < (const UUID& rhs) const
	{
		return (pValue.n64[0] == rhs.pValue.n64[0])
			? (pValue.n64[1] < rhs.pValue.n64[1])
			: (pValue.n64[0] < rhs.pValue.n64[0]);
	}


	inline bool UUID::operator > (const UUID& rhs) const
	{
		return (pValue.n64[0] == rhs.pValue.n64[0])
			? (pValue.n64[1] > rhs.pValue.n64[1])
			: (pValue.n64[0] > rhs.pValue.n64[0]);
	}


	inline bool UUID::operator <= (const UUID& rhs) const
	{
		return ((*this < rhs) or (*this == rhs));
	}


	inline bool UUID::operator >= (const UUID& rhs) const
	{
		return ((*this > rhs) or (*this == rhs));
	}





} // namespace Yuni







namespace Yuni
{
namespace Private
{
namespace UUID
{

	class Helper final
	{
	public:
		static void WriteToCString(char* cstr, const Yuni::UUID& uuid)
		{
			uuid.writeToCString(cstr);
		}
	};


} // namespace UUID
} // namespace Private
} // namespace Yuni







namespace Yuni
{
namespace Extension
{
namespace CString
{

	template<class CStringT>
	class Append<CStringT, Yuni::UUID> final
	{
	public:
		static void Perform(CStringT& s, const Yuni::UUID& rhs)
		{
			const uint currentLength = s.size();
			// writeToCString is guarantee to have 42 chars
			s.reserve(currentLength + 42); // at least 36 + 1 zero-terminated
			Yuni::Private::UUID::Helper::WriteToCString((char*)s.data() + currentLength, rhs);
			s.resize(currentLength + 36); // guid is 36-bytes length
			//s.removeLast();
		}

	}; // class Append


	template<>
	class Into<Yuni::UUID> final
	{
	public:
		typedef Yuni::UUID TargetType;
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, TargetType& out)
		{
			return out.assign(s);
		}

		template<class StringT> static TargetType Perform(const StringT& s)
		{
			return TargetType(s);
		}

	}; // class Into



} // namespace CString
} // namespace Extension
} // namespace Yuni


// ostream
std::ostream& operator << (std::ostream& out, const Yuni::UUID& rhs);

