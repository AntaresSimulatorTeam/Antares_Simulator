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
#include "../preprocessor/std.h"




/*!
** \def YUNI_STATIC_ASSERT(X,ID)
** \brief Assert at compile time
**
**
** YUNI_STATIC_ASSERT is like assert for C/C++ (man 3 assert), except that the
** test is done at compile time.
**
** Here is an example to produce an error to prevent compilation from a missing
** implementation :
** \code
** #include <iostream>
** #include <yuni/core/static/assert.h>
**
**
** template<typename T>
** struct Dummy
** {
**	void foo()
**	{
**		YUNI_STATIC_ASSERT(false, TheImplementationIsMissing);
**	}
** };
**
** template<>
** struct Dummy<int>
** {
**	void foo()
**	{
**		std::cout << "Called Dummy<int>::foo()" << std::endl;
**	}
** };
**
** int main()
** {
**	// As the static assert is within a method and not the class itself,
**	// it is possible to instanciate the class in all cases.
**	// But this is not true for the method `foo()`.
**
**	Dummy<int> dummy0;
**	// The following statement will compile
**	dummy0.foo();
**
**	Dummy<uint> dummy1;
**	// But this one will fail
**	dummy1.foo();
**
**	// Error with gcc 3.x :
**	// ./main.cpp: In member function `void Dummy<T>::foo() [with T = uint]':
**	//	./main.cpp:36:   instantiated from here
**	//	./main.cpp:11: error: creating array with size zero (` Assert_TheImplementationIsMissing')
**
**	// Error with gcc 4.x :
**	// ./main.cpp: In member function ‘void Dummy<T>::foo() [with T = uint]’:
**	//	./main.cpp:36:   instantiated from here
**	//	./main.cpp:11: error: creating array with negative size (‘(StaticAssert_TheImplementationIsMissing::._67)-0x000000001’)
**	return 0;
** }
** \endcode
**
** This macro might be used anywhere.
** <br />
** When using several static asserts within the same scope, each ID must be
** unique.
**
** \param X An expression
** \param ID An unique ID for the scope
*/
#ifdef YUNI_HAS_CPP_STATIC_ASSERT
#	define YUNI_STATIC_ASSERT(X, ID)   static_assert((X), #ID)
#else
#	define YUNI_STATIC_ASSERT(X, ID)  \
		struct StaticAssert_##ID { \
			enum { Assert_##ID = Yuni::Static::Assert<(0 != (X))>::result }; \
		}; \
		typedef char invokeStaticAssert_##ID[StaticAssert_##ID::Assert_##ID]
#endif



namespace Yuni
{
namespace Static
{

	template<int N> struct Assert   final { enum {result =  1}; }; // No error

	template<> struct Assert<false> final { enum {result = -1}; }; // Error


} // namespace Static
} // namespaec Yuni
