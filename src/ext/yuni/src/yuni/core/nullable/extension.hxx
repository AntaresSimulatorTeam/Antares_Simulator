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



namespace Yuni
{
namespace Extension
{
namespace CString
{

	template<class CStringT, class T, class Alloc>
	struct Append<CStringT, Yuni::Nullable<T, Alloc> > final
	{
		static void Perform(CStringT& s, const Yuni::Nullable<T,Alloc>& rhs)
		{
			if (!rhs.null())
				s << rhs.value();
		}
	};



	template<class T, class Alloc>
	class Into<Yuni::Nullable<T,Alloc> > final
	{
	public:
		typedef Yuni::Nullable<T,Alloc> TargetType;
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, TargetType& out)
		{
			T tmp;
			if (s.to(tmp))
				out = tmp;
			else
				out = nullptr;
			return true;
		}

		template<class StringT> static TargetType Perform(const StringT& s)
		{
			return s.template to<T>();
		}
	};






} // namespace CString
} // namespace Extension
} // namespace Yuni



template<typename T, class Alloc>
inline std::ostream& operator << (std::ostream& out, const Yuni::Nullable<T,Alloc>& rhs)
{
	rhs.print(out, "(null)");
	return out;
}


