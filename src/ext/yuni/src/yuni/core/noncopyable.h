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
	** \class NonCopyable
	** \brief Prevent objects of a class from being copy-constructed or assigned to each other
	**
	** \code
	** class ClassThatCanNotBeCopied : private NonCopyable<ClassThatCanNotBeCopied>
	** {
	** // ...
	** };
	** \endcode
	*/
	#if defined(YUNI_HAS_CPP_MOVE)

	template<class T>
	class YUNI_DECL NonCopyable
	{
	public:
		NonCopyable() = default;
		// no copy constructor
		NonCopyable(const NonCopyable&) = delete;
		// no copy operator
		template<class U> NonCopyable& operator = (const U&) = delete;
		NonCopyable& operator = (const NonCopyable&) = delete;
		// destructor
		~NonCopyable() = default;

	}; // class NonCopyable


	#else

	template<class T>
	class YUNI_DECL NonCopyable
	{
	protected:
		//! Default constructor
		NonCopyable() {}
		//! Protected non-virtual destructor
		~NonCopyable() {}


	private:
		// Private copy constructor
		NonCopyable(const NonCopyable &) {}
		// Private copy operator
		template<class U> T& operator = (const U&) {return *static_cast<T*>(this);}
		NonCopyable& operator = (const NonCopyable&) {return *static_cast<T*>(this);}

	}; // class NonCopyable

	#endif




} // namespace Yuni
