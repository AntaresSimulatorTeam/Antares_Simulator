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
#include <cstdlib>
#include <stdexcept>



namespace Yuni
{
namespace Policy
{

/*!
** \brief Singleton lifetime policies
**
** \ingroup Policies
*/
namespace Lifetime
{

	//! Prototype for the function used to destroy an instance
	typedef void (*DestructionFunction)();


	/*!
	** \brief Follow the normal C++ behaviour: first created, last destroyed
	**
	** \ingroup Policies
	*/
	template<class T>
	class Normal final
	{
	public:
		/*!
		** \brief Use atexit() to schedule destruction of the instance
		*/
		static void ScheduleDestruction(DestructionFunction callback) { std::atexit(callback); }

		/*!
		** If a dead reference is detected, throw an exception
		*/
		static void OnDeadReference() YUNI_NORETURN { throw std::runtime_error("Singleton dead reference detected !"); }
	};


	/*!
	** \brief Phoenix singletons are automatically reborn if used after destruction
	**
	** \ingroup Policies
	*/
	template<class T>
	class Phoenix final
	{
	public:
		/*!
		** \brief Use atexit() to schedule destruction of the instance
		*/
		static void ScheduleDestruction(DestructionFunction callback) { std::atexit(callback); }

		/*!
		** If a dead reference is detected, do nothing, it should work anyway
		*/
		static void OnDeadReference() { }
	};


	/*!
	** \brief Singletons with longevity are destroyed in increasing value of their longevity
	**
	** \ingroup Policies
	**
	** Among singletons with the same longevity, last in first out order is ensured
	*/
	template<class T>
	class WithLongevit final
	{
	public:
		/*!
		** \brief Track various longevities in a priority list
		*/
		static void ScheduleDestruction(DestructionFunction /*callback*/) { }

		/*!
		** \brief Throw if longevities were not given properly
		*/
		static void OnDeadReference() { throw std::runtime_error("Singleton dead reference detected !"); }
	};


namespace Private
{


	template<class T>
	static void SetLongevity(T* /*pInstance*/, uint /*longevity*/)
	{

	}


} // namespace Private


} // namespace Lifetime
} // namespace Policy
} // namespace Yuni
