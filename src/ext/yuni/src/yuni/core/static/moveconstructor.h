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
namespace Static
{

	/*!
	** \brief Move Constructor (C++ idiom)
	**
	** Transfer the ownership of a resource held by an object to a new object.
	** (Also known as the Colvin-Gibbons trick)
	*/
	template <class T>
	class MoveConstructor
	{
	public:
		MoveConstructor(T& r)
			:pReference(r)
		{}

		MoveConstructor(const MoveConstructor& r)
			:pReference(r.pReference)
		{}

		//! Case-operator
		operator T& () const  {return pReference;}


	private:
		T& pReference;
	};


} // namespace Static
} // namespace Yuni
