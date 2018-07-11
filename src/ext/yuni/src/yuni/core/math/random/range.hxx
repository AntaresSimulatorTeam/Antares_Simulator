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
#include "range.h"



namespace Yuni
{
namespace Math
{
namespace Random
{


	template<class D, int LoValue, int HiValue, typename T>
	inline Range<D,LoValue,HiValue,T>::Range()
	{}


	template<class D, int LoValue, int HiValue, typename T>
	inline Range<D,LoValue,HiValue,T>::~Range()
	{}


	template<class D, int LoValue, int HiValue, typename T>
	inline const T
	Range<D,LoValue,HiValue,T>::min()
	{
		return T(LoValue);
	}


	template<class D, int LoValue, int HiValue, typename T>
	inline const T
	Range<D,LoValue,HiValue,T>::max()
	{
		return T(HiValue);
	}



	template<class D, int LoValue, int HiValue, typename T>
	inline void
	Range<D,LoValue,HiValue,T>::reset()
	{
		pDistribution.reset();
	}



	namespace
	{
		template<class D, int LoValue, int HiValue, typename T>
		struct AlgorithmFromType
		{
			static inline T Next(D& d)
			{
				// Default algorithm, suitable for Integer types
				return T((double)d.next() / (double(d.max()) + 1.) * T(HiValue - LoValue)) + LoValue;
			}
		};

		template<class D, int LoValue, int HiValue>
		struct AlgorithmFromType<D, LoValue, HiValue, float>
		{
			static inline float Next(D& d)
			{
				return float(float(d.next()) / ((float(d.max()) + 1.) / float(HiValue - LoValue)) + float(LoValue));
			}
		};

		template<class D, int LoValue, int HiValue>
		struct AlgorithmFromType<D, LoValue, HiValue, double>
		{
			static inline double Next(D& d)
			{
				return double(d.next()) / ((double(d.max()) + 1.) / double(HiValue)) + double(LoValue);
			}
		};

		template<class D, int LoValue, int HiValue>
		struct AlgorithmFromType<D, LoValue, HiValue, long double>
		{
			static inline long double Next(D& d)
			{
				return (long double)(d.next()) / (((long double)(d.max()) + 1.) / (long double)(HiValue)) + (long double)(LoValue);
			}
		};

	} // anonymouse namespace



	template<class D, int LoValue, int HiValue, typename T>
	inline const T
	Range<D,LoValue,HiValue,T>::next()
	{
		return AlgorithmFromType<D, LoValue, HiValue, T>::Next(pDistribution);
	}




} // namespace Random
} // namespace Math
} // namespace Yuni
