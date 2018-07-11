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
#include "symbol.h"




namespace Yuni
{
namespace DynamicLibrary
{


	inline Symbol::Symbol()
		: pPtr(NULL)
	{}


	inline Symbol::Symbol(Symbol::Handle p)
		: pPtr(p)
	{}


	inline Symbol::Symbol(const Symbol& copy)
		: pPtr(copy.pPtr)
	{}


	inline bool Symbol::null() const
	{
		return (NULL == pPtr);
	}


	inline bool Symbol::valid() const
	{
		return (NULL != pPtr);
	}


	inline Symbol& Symbol::operator = (const Symbol& rhs)
	{
		pPtr = rhs.pPtr;
		return *this;
	}


	inline Symbol& Symbol::operator = (Symbol::Handle hndl)
	{
		pPtr = hndl;
		return *this;
	}


	inline Symbol::Handle Symbol::ptr() const
	{
		return pPtr;
	}





} // namespace DynamicLibrary
} // namespace Yuni
