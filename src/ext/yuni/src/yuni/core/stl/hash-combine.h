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
#include <functional>


namespace Yuni
{

	/*!
	** \brief Hash combine, to create hash for complex struct
	**
	** \code
	** class Complex
	** {
	**		int a;
	**		std::string text;
	** };
	**
	** namespace std
	** {
	**		template<> struct hash<Complex>
	**		{
	**			inline size_t operator()(const Complex& a) const
	**			{
	**				size_t seed = 0;
	**				Yuni::HashComine(seed, name.x);
	**				Yuni::HashComine(seed, name.text);
	**				return seed;
	**			}
	**		};
	** }
	** \endcode
	** \note inspired from Boot's hash_combine
	*/
	template <class T>
	inline void HashCombine(std::size_t& seed, const T& value)
	{
		std::hash<T> hasher;
		seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}



} // namespace Yuni
