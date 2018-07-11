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

	template<class CStringT, class C> class Append;
	template<class T> class Into;


	// Atomic<>
	template<class CStringT, int SizeT, template<class> class TP>
	class Append<CStringT, Yuni::Atomic::Int<SizeT,TP> >
	{
	public:
		typedef typename CStringT::Type TypeC;
		typedef typename Static::Remove::Const<TypeC>::Type C;
		static void Perform(CStringT& s, const Yuni::Atomic::Int<SizeT, TP>& rhs)
		{
			s.append(static_cast<typename Yuni::Atomic::Int<SizeT, TP>::ScalarType>(rhs));
		}
	};


	template<int SizeT, template<class> class TP>
	class Into<Yuni::Atomic::Int<SizeT,TP> >
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, Yuni::Atomic::Int<SizeT, TP>& out)
		{
			typedef typename Yuni::Atomic::Int<SizeT, TP>::ScalarType Scalar;
			Scalar tmp;
			if (s.template to<Scalar>(tmp))
			{
				out = tmp;
				return true;
			}
			return false;
		}

		template<class StringT>
		static typename Yuni::Atomic::Int<SizeT, TP>::ScalarType Perform(const StringT& s)
		{
			return s.template to<typename Yuni::Atomic::Int<SizeT, TP>::ScalarType>();
		}
	};




} // namespace CString
} // namespace Extension
} // namespace Yuni
