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
#include "../../yuni.h"
#include "remove.h"


/*!
** \brief Macro for implementing a check for Determining whether a class has a specific typedef or not
*/
#define YUNI_IMPL_STATIC_HAS_TYPEDEF(TYPEDEF)  \
		template<class T> \
		class TYPEDEF final \
		{ \
			typedef typename ::Yuni::Static::Remove::All<T>::Type  Type; \
			typedef struct {char a[2];}   TrueType; \
			typedef char  FalseType; \
			template<class C> static TrueType  deduce(typename C::TYPEDEF const *); \
			template<class C> static FalseType deduce(...); \
		\
		public: \
			enum \
			{ \
				yes = sizeof(deduce<Type>(0)) == sizeof(TrueType), \
				no = !yes, \
			}; \
		} /* class */






namespace Yuni
{
namespace Static
{
namespace HasTypedef
{

	/*!
	** \brief Determine whether a class has the typedef `Ptr` or not
	**
	** \code
	** std::cout << Static::HasTypedef::Ptr<MyClass>::yes << std::endl;
	** \endcode
	*/
	YUNI_IMPL_STATIC_HAS_TYPEDEF(Ptr);


	/*!
	** \brief Determine whether a class has the typedef `IntrusiveSmartPtrType` or not
	**
	** This test is mainly used to prevent misuse of some incompatible smartptr.
	*/
	YUNI_IMPL_STATIC_HAS_TYPEDEF(IntrusiveSmartPtrType);


	/*!
	** \brief Determine whether a class has the typedef `CStringType` or not
	**
	** This test is mainly used to detect Yuni string where templates can not be used.
	*/
	YUNI_IMPL_STATIC_HAS_TYPEDEF(CStringType);




} // namespace HasMethod
} // namespace Static
} // namespace Yuni
