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

/*!
** \file
** \brief Singleton creation policies
*/


namespace Yuni
{
namespace Policy
{

/*!
** \brief Creation policies
** \ingroup Policies
*/
namespace Creation
{


	/*!
	** \brief Singleton creation using the new operator and an empty constructor
	** \ingroup Policies
	**
	** \tparam T The data type
	*/
	template <class T>
	class EmptyConstructor final
	{
	public:
		//! Creation of the data
		static T* Create() { return new T(); }

		//! Destruction of the data
		template<class U> static void Destroy(U* data) { delete data; }
	};





} // namespace Creation
} // namespace Policy
} // namespace Yuni
