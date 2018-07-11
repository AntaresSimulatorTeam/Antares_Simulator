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
#include "remove.h"



namespace Yuni
{
namespace Static
{

	/*!
	** \brief Choose statically between two types
	**
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/core/static/if.h>
	**
	** template<bool x86_64>
	** class IntWrapper
	** {
	** public:
	**     typedef typename Yuni::Static::If<x86_64, int64, int32>::Type  IntType;
	**
	** };
	** \endcode
	*/
	template <bool b, typename IfTrue, typename IfFalse>
	struct If final
	{
		typedef IfTrue ResultType;
		typedef IfTrue Type;
		typedef typename Remove::All<IfTrue>::Type   RetTrue;
		typedef typename Remove::All<IfFalse>::Type  RetFalse;

		static RetTrue& choose (RetTrue& tr, RetFalse&)
		{return tr;}

		static const RetTrue& choose (const RetTrue& tr, const RetFalse&)
		{ return tr; }

	}; // struct If




	template <typename IfTrue, typename IfFalse>
	struct If<false, IfTrue, IfFalse> final
	{
		typedef IfFalse ResultType;
		typedef IfFalse Type;
		typedef typename Remove::All<IfTrue>::Type   RetTrue;
		typedef typename Remove::All<IfFalse>::Type  RetFalse;

		static RetFalse& choose (RetTrue&, RetFalse& fa)
		{ return fa; }

		static const RetFalse& choose (const RetTrue&, const RetFalse& fa)
		{ return fa; }
	};



} // namespace Static
} // namespaec Yuni
