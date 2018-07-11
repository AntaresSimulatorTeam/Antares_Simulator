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

	/*!
	** \class NonMovable
	** \brief Prevent objects of a class from being move-constructed or assigned to each other
	**
	** \code
	** class ClassThatCanNotBeCopied : private NonMovable<ClassThatCanNotBeCopied>
	** {
	** // ...
	** };
	** \endcode
	*/
	#if defined(YUNI_HAS_CPP_MOVE)

	template<class T>
	class YUNI_DECL NonMovable
	{
	protected:
		// default constructor
		NonMovable() = default;
		// copy constructor
		NonMovable(const NonMovable&) = default;
		// no move constructor
		NonMovable(NonMovable&&) = delete;
		// no copy operator
		NonMovable& operator = (NonMovable&&) = delete;
		// default destructor
		~NonMovable() = default;

	}; // class NonMovable


	#else

	template<class T> class YUNI_DECL NonMovable
	{
	protected:
		//! Default constructor
		NonCopyable() {}
		//! Protected non-virtual destructor
		~NonCopyable() {}
	};

	#endif




} // namespace Yuni
