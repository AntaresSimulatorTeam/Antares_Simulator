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
#include "int.h"


namespace Yuni
{
namespace Atomic
{

	template<int Size, template<class> class TP>
	inline Int<Size,TP>::Int() :
		# if YUNI_ATOMIC_MUST_USE_MUTEX != 1
		pValue()
		# else
		TP<Int<Size,TP> >(), pValue()
		# endif
	{}


	template<int Size, template<class> class TP>
	inline Int<Size,TP>::Int(sint16 v) :
		# if YUNI_ATOMIC_MUST_USE_MUTEX != 1
		pValue(static_cast<ScalarType>(v))
		# else
		TP<Int<Size,TP> >(), pValue((ScalarType)v)
		# endif
	{}

	template<int Size, template<class> class TP>
	inline Int<Size,TP>::Int(sint32 v) :
		# if YUNI_ATOMIC_MUST_USE_MUTEX != 1
		pValue(static_cast<ScalarType>(v))
		# else
		TP<Int<Size,TP> >(), pValue((ScalarType)v)
		# endif
	{}


	template<int Size, template<class> class TP>
	inline Int<Size,TP>::Int(sint64 v) :
		# if YUNI_ATOMIC_MUST_USE_MUTEX != 1
		pValue(static_cast<ScalarType>(v))
		# else
		TP<Int<Size,TP> >(), pValue(static_cast<ScalarType>(v))
		# endif
	{}




	template<int Size, template<class> class TP>
	inline Int<Size,TP>::Int(const Int<Size,TP>& v) :
		# if YUNI_ATOMIC_MUST_USE_MUTEX != 1
		pValue(static_cast<ScalarType>(v))
		# else
		TP<Int<Size,TP> >(), pValue((ScalarType)v.pValue)
		# endif
	{
	}

	template<int Size, template<class> class TP>
	template<int Size2, template<class> class TP2>
	inline Int<Size,TP>::Int(const Int<Size2,TP2>& v) :
		# if YUNI_ATOMIC_MUST_USE_MUTEX != 1
		pValue()
		# else
		TP<Int<Size,TP> >(), pValue()
		# endif
	{
		if (threadSafe)
			Private::AtomicImpl::Operator<size,TP>::Increment(*this, static_cast<ScalarType>(v.pValue));
		else
			pValue = static_cast<ScalarType>(v.pValue);
	}


	template<int Size, template<class> class TP>
	inline Int<Size,TP>::operator ScalarType () const
	{
		return (threadSafe)
			? Private::AtomicImpl::Operator<size,TP>::Increment(*this, 0)
			: (pValue);
	}


	template<int Size, template<class> class TP>
	inline typename Int<Size,TP>::ScalarType Int<Size,TP>::operator ++ ()
	{
		return (threadSafe)
			? Private::AtomicImpl::Operator<size,TP>::Increment(*this)
			: (++pValue);
	}


	template<int Size, template<class> class TP>
	inline typename Int<Size,TP>::ScalarType Int<Size,TP>::operator -- ()
	{
		return (threadSafe)
			? Private::AtomicImpl::Operator<size,TP>::Decrement(*this)
			: (--pValue);
	}


	template<int Size, template<class> class TP>
	inline typename Int<Size,TP>::ScalarType Int<Size,TP>::operator ++ (int)
	{
		return (threadSafe)
			? Private::AtomicImpl::Operator<size,TP>::Increment(*this) - 1
			: (pValue++);
	}


	template<int Size, template<class> class TP>
	inline typename Int<Size,TP>::ScalarType Int<Size,TP>::operator -- (int)
	{
		return (threadSafe)
			? Private::AtomicImpl::Operator<size,TP>::Decrement(*this) + 1
			: (pValue--);
	}


	template<int Size, template<class> class TP>
	inline bool Int<Size,TP>::operator ! () const
	{
		return (threadSafe)
			? (0 == Private::AtomicImpl::Operator<size,TP>::Increment(*this, 0))
			: (0 == pValue);

	}


	template<int Size, template<class> class TP>
	inline Int<Size,TP>& Int<Size,TP>::operator = (const ScalarType& v)
	{
		if (threadSafe)
			Private::AtomicImpl::Operator<size,TP>::Set(*this, v);
		else
			pValue = v;
		return *this;
	}


	template<int Size, template<class> class TP>
	inline Int<Size,TP>& Int<Size,TP>::operator += (const ScalarType& v)
	{
		if (threadSafe)
			Private::AtomicImpl::Operator<size,TP>::Increment(*this, v);
		else
			pValue += v;
		return *this;
	}


	template<int Size, template<class> class TP>
	inline Int<Size,TP>& Int<Size,TP>::operator -= (const ScalarType& v)
	{
		if (threadSafe)
			Private::AtomicImpl::Operator<size,TP>::Decrement(*this, v);
		else
			pValue -= v;
		return *this;
	}


	template<int Size, template<class> class TP>
	inline void Int<Size,TP>::zero()
	{
		if (threadSafe)
			Private::AtomicImpl::Operator<size,TP>::Zero(*this);
		else
			pValue = 0;
	}






} // namespace Atomic
} // namespace Yuni
