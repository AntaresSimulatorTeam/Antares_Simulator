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
#include "luhn.h"



namespace Yuni
{
namespace Algorithm
{


	inline bool Luhn::IsValidCreditCardNumber(const AnyString& s)
	{
		return (Mod10(s) == 0);
	}



} // namespace Algorithm
} // namespace Yuni
