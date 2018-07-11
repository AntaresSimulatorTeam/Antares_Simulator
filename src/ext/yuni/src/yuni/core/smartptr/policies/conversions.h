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




namespace Yuni
{
namespace Policy
{

/*!
** \brief Conversion policies
** \ingroup Policies
*/
namespace Conversion
{


	/*!
	** \ingroup Policies
	*/
	struct Allow
	{
		enum { allow = true };
		static void swapPointer(Allow&) {}
	};


	/*!
	** \ingroup Policies
	*/
	struct Disallow
	{
		//! Default constructor
		Disallow() {}
		/*!
		** \brief Copy constructor
		**
		** It is possible to initialize a `Disallow` policy with an `Allow` policy
		*/
		Disallow(const Allow&) {}

		enum { allow = false };

		static void swapPointer(Disallow&) {}
	};



} // namespace Conversion
} // namespace Policy
} // namespace Yuni

