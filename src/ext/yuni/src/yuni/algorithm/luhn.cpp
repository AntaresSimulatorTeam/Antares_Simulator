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
#include "luhn.h"
#include "../core/string.h"


namespace Yuni
{
namespace Algorithm
{

	int Luhn::Mod10(const AnyString& s)
	{
		// The string must have at least one char
		if (s.size() > 1)
		{
			// The algorithm :
			// 1 - Counting from the check digit, which is the rightmost, and moving
			//     left, double the value of every second digit.
			// 2 - Sum the digits of the products together with the undoubled digits
			//     from the original number.
			// 3 - If the total ends in 0 (put another way, if the total modulo 10 is
			//     congruent to 0), then the number is valid according to the Luhn formula
			//
			static const int prefetch[] = {0, 2, 4, 6, 8, 1, 3, 5, 7, 9};

			int sum = 0;
			bool alternate = true;

			const AnyString::iterator end = s.end();
			// For each char
			for (AnyString::iterator i = s.begin(); end != i; ++i)
			{
				// Each char in the string must be a digit
				if (!String::IsDigit(i.value()))
					return false;
				// The `real` digit
				int n = i.value() - '0';
				// Computing the sum
				sum += (alternate = !alternate) ? prefetch[n] : n;
			}
			return sum % 10;
		}
		return -1;
	}



} // namespace Algorithm
} // namespace Yuni


