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
** \brief Checking policies
** \ingroup Policies
*/
namespace Checking
{

	/*!
	** \brief Do not make any check
	** \ingroup Policies
	*/
	template<class T>
	class None
	{
	public:
		None() {}
		template<class U> None(const None<U>&) {}

		static void onDefault(const T&) {}

		static void onInit(const T&) {}

		static void onDereference(const T&) {}

		static void swapPointer(None&) {}

	}; // class None



	/*!
	** \brief Ensure the pointer can never be null
	** \ingroup Policies
	**
	** The default constructor (which inits at null) is disabled statically
	** by not defining onDefault.
	*/
	template<class T>
	class NeverNull
	{
	public:
		NeverNull() {}
		template<class U> NeverNull(const NeverNull<U>&) { }

		static void onInit(const T& ptr) { if (!ptr) throw; }

		static void onDereference(const T&) {}

		static void swapPointer(NeverNull&) {}

	}; // class NotNull






} // namespace Checking
} // namespace Policy
} // namespace Yuni

