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
#include "tribool.h"



namespace Yuni
{

	inline Tribool::Tribool()
	{
		pValue.flags[0] = -1;
		pValue.flags[1] =  0;
	}


	inline Tribool::Tribool(bool value, bool defvalue)
	{
		pValue.flags[0] = static_cast<yint8>(value);
		pValue.flags[1] = static_cast<yint8>(defvalue);
	}


	inline Tribool::Tribool(const NullPtr&, bool defvalue)
	{
		pValue.flags[0] = static_cast<yint8>(-1);
		pValue.flags[1] = static_cast<yint8>(defvalue);
	}


	inline Tribool::Tribool(const Tribool& rhs)
	{
		pValue.u16 = rhs.pValue.u16;
	}


	inline void Tribool::clear()
	{
		pValue.flags[0] = -1; // reset to indeterminate
		pValue.flags[1] =  0;
	}


	inline bool Tribool::indeterminate() const
	{
		return (pValue.flags[0] < 0);
	}


	inline bool Tribool::defaultValue() const
	{
		return (0 != pValue.flags[1]);
	}

	inline void Tribool::defaultValue(bool defvalue)
	{
		pValue.flags[1] = static_cast<yuint8>(defvalue);
	}


	inline bool Tribool::toBool() const
	{
		return (indeterminate())
			? (0 != pValue.flags[1])   // using the default value
			: (0 != pValue.flags[0]);
	}


	inline Tribool& Tribool::operator = (const NullPtr&)
	{
		pValue.flags[0] = -1;
		return *this;
	}


	inline Tribool& Tribool::operator = (bool value)
	{
		pValue.flags[0] = static_cast<yint8>(value);
		return *this;
	}


	inline Tribool& Tribool::operator = (const Tribool& rhs)
	{
		pValue.u16 = rhs.pValue.u16;
		return *this;
	}


	inline bool Tribool::operator == (bool value) const
	{
		return (operator bool ()) == value;
	}


	inline bool Tribool::operator == (const NullPtr&) const
	{
		return indeterminate();
	}


	inline bool Tribool::operator == (const Tribool& rhs) const
	{
		return pValue.u16 == rhs.pValue.u16;
	}


	inline bool Tribool::operator != (bool value) const
	{
		return (operator bool ()) != value;
	}


	inline bool Tribool::operator != (const NullPtr&) const
	{
		return not indeterminate();
	}


	inline bool Tribool::operator != (const Tribool& rhs) const
	{
		return pValue.u16 != rhs.pValue.u16;
	}


	inline Tribool::operator bool () const
	{
		return toBool();
	}


	template<class StreamT> inline void Tribool::print(StreamT& out) const
	{
		switch (pValue.flags[0])
		{
			case 0:  out.write("false", 5); break;
			case 1:  out.write("true", 4); break;
			default: out.write("indeterminate", 13); break;
		}
	}



} // namespace Yuni



//! \name Operator overload for stream printing
//@{
inline std::ostream& operator << (std::ostream& out, const Yuni::Tribool& rhs)
{
	rhs.print(out);
	return out;
}
//@}



namespace Yuni
{
namespace Extension
{
namespace CString
{

	template<class CStringT>
	class Append<CStringT, Yuni::Tribool>
	{
	public:
		static void Perform(CStringT& s, const Yuni::Tribool& rhs)
		{
			rhs.print(s);
		}
	};


	template<>
	class Into<Yuni::Tribool>
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, Yuni::Tribool& out)
		{
			AnyString s1 = s;
			s1.trim();

			switch (s1.size())
			{
				case 1:
				{
					switch (s1.first())
					{
						case '1': out = true;  return true;
						case '0': out = false; return true;
						case '?': out.clear(); return true;
					}
					break;
				}
				case 0:
				{
					break;
				}
				default:
				{
					Yuni::ShortString16 s2 = s1;
					s2.toLower();
					if ("true" == s2 or "on" == s2 or "yes" == s2)
					{
						out = true;
						return true;
					}
					if ("false" == s2 or "off" == s2 or "no" == s2)
					{
						out = false;
						return true;
					}
					if ("indeterminate" == s2 or "null" == s2 or "undefined" == s2 or "undef" == s2 or "default" == s2)
					{
						out.clear();
						return true;
					}
					break;
				}
			}

			out.clear();
			return false;
		}

		template<class StringT> static Yuni::Tribool Perform(const StringT& s)
		{
			Tribool out;
			Perform(s, out);
			return out;
		}
	};



} // namespace CString
} // namespace Extension
} // namespace Yuni
