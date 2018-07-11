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
#include <algorithm>
#include "../static/remove.h"


namespace Yuni
{
namespace STL
{


	/*!
	** \brief Functor to delete a pointer: used in automatic clearing of an STL container
	**
	** \tparam T The original type of items of a STL container
	*/
	template <typename T>
	struct Delete
	{
		inline void operator () (T* ptr) const
		{
			delete ptr;
		}
	}; // class Delete



	/*!
	** \brief Syntactic sugar to free all pointers in an STL container and clear it
	**
	** \param container The container to clear
	** \tparam T The type of items to remove
	** \tparam U A STL container
	**
	** \code
	** typedef std::vector<int> List;
	** List list;
	** Yuni::STL::DeleteAndClear<int, List>(list);
	** \endcode
	*/
	template <typename T, typename U>
	inline void DeleteAndClear(U& container)
	{
		// The original type
		typedef typename Static::Remove::All<T>::Type Type;

		// Delete each item
		std::for_each(container.begin(), container.end(), Delete<Type>());
		// Clear the container
		container.clear();
	}



} // namespace STL
} // namespace Yuni

