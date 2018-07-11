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
#include <stdexcept>
#include <typeinfo>



namespace Yuni
{
namespace Exceptions
{


	/*!
	** \brief Is thrown in case of impossible cast from one type to another.
	** \ingroup Exceptions
	**
	** \see Any
	*/
	struct BadCast : public std::bad_cast
	{
		/*!
		** \brief Constructor
		*/
		BadCast(const std::type_info& src, const std::type_info& dest)
			: from(src.name()), to(dest.name())
		{ }

		/*!
		** \brief Description printed by libstdc++.
		*/
		virtual const char *what() const throw ()
		{
			return "Bad cast";
		}

		/*!
		** \brief Cast source
		*/
		const char* from;

		/*!
		** \brief Cast target
		*/
		const char* to;
	};




} // namespace Exceptions
} // namespace Yuni
