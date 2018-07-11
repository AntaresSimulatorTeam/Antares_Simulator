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
#include "item.h"



namespace Yuni
{
namespace Event
{

	template<class C, class Mode, class R, class Dummy>
	inline
	ObserverItemA0<C,Mode, R, Dummy>::ObserverItemA0(C* o, R (C::*method)())
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, class Dummy>
	inline IObserver*
	ObserverItemA0<C,Mode, R, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, class Dummy>
	inline bool
	ObserverItemA0<C,Mode, R, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, class Dummy>
	inline R
	ObserverItemA0<C,Mode, R, Dummy>::fireEvent() const
	{
		(pObserver->*pMethod)();
	}

	template<class C, class Mode, class R, typename A0, class Dummy>
	inline
	ObserverItemA1<C,Mode, R, A0, Dummy>::ObserverItemA1(C* o, R (C::*method)(A0))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, class Dummy>
	inline IObserver*
	ObserverItemA1<C,Mode, R, A0, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, class Dummy>
	inline bool
	ObserverItemA1<C,Mode, R, A0, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, class Dummy>
	inline R
	ObserverItemA1<C,Mode, R, A0, Dummy>::fireEvent(A0 a0) const
	{
		(pObserver->*pMethod)(a0);
	}

	template<class C, class Mode, class R, typename A0, typename A1, class Dummy>
	inline
	ObserverItemA2<C,Mode, R, A0, A1, Dummy>::ObserverItemA2(C* o, R (C::*method)(A0, A1))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, class Dummy>
	inline IObserver*
	ObserverItemA2<C,Mode, R, A0, A1, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, class Dummy>
	inline bool
	ObserverItemA2<C,Mode, R, A0, A1, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, class Dummy>
	inline R
	ObserverItemA2<C,Mode, R, A0, A1, Dummy>::fireEvent(A0 a0, A1 a1) const
	{
		(pObserver->*pMethod)(a0, a1);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, class Dummy>
	inline
	ObserverItemA3<C,Mode, R, A0, A1, A2, Dummy>::ObserverItemA3(C* o, R (C::*method)(A0, A1, A2))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, class Dummy>
	inline IObserver*
	ObserverItemA3<C,Mode, R, A0, A1, A2, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, class Dummy>
	inline bool
	ObserverItemA3<C,Mode, R, A0, A1, A2, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, class Dummy>
	inline R
	ObserverItemA3<C,Mode, R, A0, A1, A2, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2) const
	{
		(pObserver->*pMethod)(a0, a1, a2);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, class Dummy>
	inline
	ObserverItemA4<C,Mode, R, A0, A1, A2, A3, Dummy>::ObserverItemA4(C* o, R (C::*method)(A0, A1, A2, A3))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, class Dummy>
	inline IObserver*
	ObserverItemA4<C,Mode, R, A0, A1, A2, A3, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, class Dummy>
	inline bool
	ObserverItemA4<C,Mode, R, A0, A1, A2, A3, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, class Dummy>
	inline R
	ObserverItemA4<C,Mode, R, A0, A1, A2, A3, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, class Dummy>
	inline
	ObserverItemA5<C,Mode, R, A0, A1, A2, A3, A4, Dummy>::ObserverItemA5(C* o, R (C::*method)(A0, A1, A2, A3, A4))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, class Dummy>
	inline IObserver*
	ObserverItemA5<C,Mode, R, A0, A1, A2, A3, A4, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, class Dummy>
	inline bool
	ObserverItemA5<C,Mode, R, A0, A1, A2, A3, A4, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, class Dummy>
	inline R
	ObserverItemA5<C,Mode, R, A0, A1, A2, A3, A4, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, class Dummy>
	inline
	ObserverItemA6<C,Mode, R, A0, A1, A2, A3, A4, A5, Dummy>::ObserverItemA6(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, class Dummy>
	inline IObserver*
	ObserverItemA6<C,Mode, R, A0, A1, A2, A3, A4, A5, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, class Dummy>
	inline bool
	ObserverItemA6<C,Mode, R, A0, A1, A2, A3, A4, A5, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, class Dummy>
	inline R
	ObserverItemA6<C,Mode, R, A0, A1, A2, A3, A4, A5, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, class Dummy>
	inline
	ObserverItemA7<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, Dummy>::ObserverItemA7(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, class Dummy>
	inline IObserver*
	ObserverItemA7<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, class Dummy>
	inline bool
	ObserverItemA7<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, class Dummy>
	inline R
	ObserverItemA7<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, class Dummy>
	inline
	ObserverItemA8<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, Dummy>::ObserverItemA8(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, class Dummy>
	inline IObserver*
	ObserverItemA8<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, class Dummy>
	inline bool
	ObserverItemA8<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, class Dummy>
	inline R
	ObserverItemA8<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, class Dummy>
	inline
	ObserverItemA9<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, Dummy>::ObserverItemA9(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, class Dummy>
	inline IObserver*
	ObserverItemA9<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, class Dummy>
	inline bool
	ObserverItemA9<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, class Dummy>
	inline R
	ObserverItemA9<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, class Dummy>
	inline
	ObserverItemA10<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, Dummy>::ObserverItemA10(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, class Dummy>
	inline IObserver*
	ObserverItemA10<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, class Dummy>
	inline bool
	ObserverItemA10<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, class Dummy>
	inline R
	ObserverItemA10<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, class Dummy>
	inline
	ObserverItemA11<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, Dummy>::ObserverItemA11(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, class Dummy>
	inline IObserver*
	ObserverItemA11<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, class Dummy>
	inline bool
	ObserverItemA11<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, class Dummy>
	inline R
	ObserverItemA11<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, class Dummy>
	inline
	ObserverItemA12<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, Dummy>::ObserverItemA12(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, class Dummy>
	inline IObserver*
	ObserverItemA12<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, class Dummy>
	inline bool
	ObserverItemA12<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, class Dummy>
	inline R
	ObserverItemA12<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, class Dummy>
	inline
	ObserverItemA13<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, Dummy>::ObserverItemA13(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, class Dummy>
	inline IObserver*
	ObserverItemA13<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, class Dummy>
	inline bool
	ObserverItemA13<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, class Dummy>
	inline R
	ObserverItemA13<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, class Dummy>
	inline
	ObserverItemA14<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, Dummy>::ObserverItemA14(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, class Dummy>
	inline IObserver*
	ObserverItemA14<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, class Dummy>
	inline bool
	ObserverItemA14<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, class Dummy>
	inline R
	ObserverItemA14<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, class Dummy>
	inline
	ObserverItemA15<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, Dummy>::ObserverItemA15(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, class Dummy>
	inline IObserver*
	ObserverItemA15<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, class Dummy>
	inline bool
	ObserverItemA15<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, class Dummy>
	inline R
	ObserverItemA15<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15, class Dummy>
	inline
	ObserverItemA16<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, Dummy>::ObserverItemA16(C* o, R (C::*method)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15))
		:pObserver(o), pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15, class Dummy>
	inline IObserver*
	ObserverItemA16<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15, class Dummy>
	inline bool
	ObserverItemA16<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15, class Dummy>
	inline R
	ObserverItemA16<C,Mode, R, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, Dummy>::fireEvent(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, A15 a15) const
	{
		(pObserver->*pMethod)(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
	}




} // namespace Event
} // namespace Yuni
