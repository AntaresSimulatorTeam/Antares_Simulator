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
#include "table.h"


namespace Yuni
{
namespace Math
{
namespace Random
{


	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline const char* Table<D,TableSize,Cyclic,TP>::Name()
	{
		return D::Name();
	}



	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline Table<D,TableSize,Cyclic,TP>::Table()
		:D(), pOffset()
	{}


	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline Table<D,TableSize,Cyclic,TP>::~Table()
	{}


	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline void
	Table<D,TableSize,Cyclic,TP>::reset()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		static_cast<D*>(this)->reset();
		fillWL();
	}


	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline void
	Table<D,TableSize,Cyclic,TP>::fill()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		fillWL();
	}

	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	void
	Table<D,TableSize,Cyclic,TP>::fillWL()
	{
		// Regenerate all items in the cache
		for (int i = 0; i != TableSize; ++i)
			pCache[i] = static_cast<D*>(this)->next();
		// The index must be reset to `-1`, in order to use the first random number
		// to the next call to `next()`.
		// See `next()` for more informations.
		pOffset = -1;
	}


	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline int
	Table<D,TableSize,Cyclic,TP>::size()
	{
		return TableSize;
	}




	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline const typename Table<D,TableSize,Cyclic,TP>::Value
	Table<D,TableSize,Cyclic,TP>::next()
	{
		// Lock
		typename ThreadingPolicy::MutexLocker locker(*this);

		// In order to keep the best performances, we have to avoid post increments
		// and temporary variables.
		// A way to achieve this is to use a pre-increment and a simple test
		// for the index, then to directly return the result.
		if (++pOffset == TableSize)
		{
			if (!Cyclic) // The random numbers must be re-generated if not cyclic
				fillWL();
			// Restting the offset, which may be equal to `TableSize` or to `-1` (see `fillWL()`)
			pOffset = 0;
			return pCache[0];
		}
		return pCache[pOffset];
	}



	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	template<class U>
	inline Table<D,TableSize,Cyclic,TP>&
	Table<D,TableSize,Cyclic,TP>::operator >> (U& u)
	{
		u = next();
		return *this;
	}


	template<class D, int TableSize, bool Cyclic, template<class> class TP>
	inline const typename Table<D,TableSize,Cyclic,TP>::Value
	Table<D,TableSize,Cyclic,TP>::operator () ()
	{
		return next();
	}





} // namespace Random
} // namespace Math
} // namespace Yuni
