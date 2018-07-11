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
#include "../static/if.h"
#include "binaryfunctions.h"
#include "loop.h"

#ifdef YUNI_HAS_CPP_LAMBDA
#include <utility>
#endif


namespace Yuni
{
namespace Functional
{

	#ifdef YUNI_HAS_CPP_LAMBDA
	//! Forward declaration
	template<class T, class CallbackT>
	struct Mapping;
	#endif


	//! Filter selects elements from the view that fulfill a certain predicate
	template<class T, class CallbackT>
	struct Filter final
	{
	public:
		typedef T  LoopType;
		typedef typename LoopType::ElementType  ElementType;
		typedef Filter<T, CallbackT>  Type;

		// If there is a mapping somewhere before, we must not pass by reference
		enum { hasMapping = LoopType::hasMapping };
		typedef typename Static::If<hasMapping, ElementType, const ElementType&>::Type  ResultT;

	public:
		Filter(const LoopType& loop, const CallbackT& callback)
			: pLoop(loop)
			, pCallback(callback)
		{}

		void reset() { pLoop.reset(); }

		bool empty() const { return pLoop.empty(); }

		bool next()
		{
			bool notFinished;
			do
			{
				notFinished = pLoop.next();
			}
			while (notFinished && not pCallback(pLoop.current()));
			return notFinished;
		}

		// By copy or reference depending on whether there was a mapping before
		ResultT current() const { return pLoop.current(); }

	private:
		LoopType pLoop;
		const CallbackT& pCallback;
	};



# ifdef YUNI_HAS_CPP_LAMBDA


	template<class T, class CallbackT>
	struct Mapping final
	{
	public:
		typedef T  LoopType;
		typedef decltype(std::declval<CallbackT>()(std::declval<T>().current()))  ElementType;
		typedef Mapping<T, CallbackT>  Type;

		enum { hasMapping = true };

	public:
		Mapping(const LoopType& loop, const CallbackT& callback)
			: pLoop(loop)
			, pCallback(callback)
		{}

		void reset() { pLoop.reset(); }

		bool empty() const { return pLoop.empty(); }

		bool next() { return pLoop.next(); }

		// Always by copy
		ElementType current() const { return pCallback(pLoop.current()); }

	private:
		LoopType pLoop;
		const CallbackT& pCallback;
	};

# endif


	/*
	template<class T>
	class Loop final
	{
	public:
		typedef uint ElementType;
		Loop(const T& loop)
			: pLoop(loop)
		{}
		void reset() {}
		bool empty() const {return true;}
		bool next() { return false; }
		ElementType& current() {}
		const ElementType& current() const {}

	private:
		const T& pLoop;
	};
	*/

	template<class T, bool UserTypeT>
	struct View final
	{
	public:
		typedef typename Static::If<UserTypeT, Loop<T>, T>::Type  LoopType;
		typedef typename LoopType::ElementType ElementType;
		typedef typename Static::If<LoopType::hasMapping, ElementType, const ElementType&>::Type  ResultT;

	public:
		View(const T& loop)
			: pLoop(loop)
		{}

		template<class BeginT, class EndT>
		View(BeginT begin, EndT end)
			: pLoop(begin, end)
		{}

		void reset() { pLoop.reset(); }
		bool empty() const {return pLoop.empty();}
		bool next() { return pLoop.next(); }
		//ElementType& current() { return pLoop.current(); }
		ResultT current() const { return pLoop.current(); }

		//! View filtering
		template<class C>
		View<Filter<LoopType, C>, false> filter(const C& callback) const
		{
			return Filter<LoopType, C>(pLoop, callback);
		}

#ifdef YUNI_HAS_CPP_LAMBDA
		//! View mapping
		template<class C>
		View<Mapping<LoopType, C>, false> map(const C& callback) const
		{
			return Mapping<LoopType, C>(pLoop, callback);
		}
#endif

		//! Loop on all elements
		template<class CallbackT>
		void each(const CallbackT& callback)
		{
			if (empty())
				return;
			do
			{
				if (not callback(current()))
					break;
			} while (next());
		}

		//! User-defined folding
		template<class FoldedT, class AccumulatorT>
		FoldedT fold(const FoldedT& initval, const AccumulatorT& accumulate)
		{
			FoldedT result = initval;

			if (empty())
				return result;
			do
			{
				if (not accumulate(result, current()))
					break;
			} while (next());
			return result;
		}


		//! Pre-defined folding : maximum
		uint count()
		{
			return map([](const ElementType&) -> uint { return 1; }).sum();
		}


		//! Pre-defined folding : maximum
		ElementType max(const ElementType& lowBound = std::numeric_limits<ElementType>::min())
		{
			return fold(lowBound, Max<ElementType>());
		}

		//! Pre-defined folding : minimum
		ElementType min(const ElementType& highBound = std::numeric_limits<ElementType>::max())
		{
			return fold(highBound, Min<ElementType>());
		}

		//! Pre-defined folding : sum
		ElementType sum(const ElementType& initVal = 0)
		{
			return fold(initVal, Add<ElementType>());
		}

		//! Pre-defined folding : difference
		ElementType diff(const ElementType& initVal)
		{
			return fold(initVal, Sub<ElementType>());
		}

		//! Pre-defined folding : product
		ElementType product(const ElementType& initVal = 1)
		{
			return fold(initVal, Mul<ElementType>());
		}

		//! Pre-defined folding : quotient
		ElementType quotient(const ElementType& initVal)
		{
			return fold(initVal, Div<ElementType>());
		}

		//! Pre-defined folding : mean / mathematical average
		template<class FoldedT = ElementType>
		FoldedT average()
		{
			uint size = 0u;
			return fold((FoldedT)0, [&size](FoldedT& result, const ElementType& elt) -> bool
			{
				Add<FoldedT>()(result, elt);
				++size;
				return true;
			}) / (FoldedT)size;
		}

		//! Pre-defined folding : both the min and the max in a single pass
		std::pair<ElementType, ElementType> minMax(
			const ElementType& lowBound = std::numeric_limits<ElementType>::min(),
			const ElementType& highBound = std::numeric_limits<ElementType>::max())
		{
			return fold(std::pair<ElementType, ElementType>(highBound, lowBound),
				[](std::pair<ElementType, ElementType>& minMax, const ElementType& elt) -> bool
			{
				Min<ElementType>()(minMax.first, elt);
				Max<ElementType>()(minMax.second, elt);
				return true;
			});
		}

		//! Pre-defined folding : value range (maximum - minimum)
		ElementType valueRange()
		{
			std::pair<ElementType, ElementType> bounds = minMax();
			return bounds.second - bounds.first;
		}

	public:
		LoopType pLoop;
	};




} // namespace Functional
} // namespace Yuni
