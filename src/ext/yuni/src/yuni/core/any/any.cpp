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
#include <algorithm>
#include "any.h"


namespace Yuni
{


	Any::Any()
	{
		pTable = Private::Any::Table<Private::Any::Empty>::Get();
		pObject = NULL;
	}


	Any::Any(const Any& rhs)
	{
		pTable = Private::Any::Table<Private::Any::Empty>::Get();
		assign(rhs);
	}


	Any::~Any()
	{
		pTable->staticDelete(&pObject);
	}


	Any& Any::assign(const Any& rhs)
	{
		// Are we copying from the same type (using the same table) ?
		if (pTable == rhs.pTable)
		{
			// If so, we can avoid reallocation
			pTable->move(&rhs.pObject, &pObject);
		}
		else
		{
			reset();
			rhs.pTable->clone(&rhs.pObject, &pObject);
			pTable = rhs.pTable;
		}
		return *this;
	}


	void Any::Swap(Any& one, Any& other)
	{
		std::swap(one.pTable, other.pTable);
		std::swap(one.pObject, other.pObject);
	}


	void Any::reset()
	{
		if (!this->empty())
		{
			pTable->staticDelete(&pObject);
			pTable = Private::Any::Table<Private::Any::Empty>::Get();
			pObject = NULL;
		}
	}



} // namespace Yuni
