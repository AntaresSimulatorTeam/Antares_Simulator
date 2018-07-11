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
#include "../static/remove.h"



namespace Yuni
{
namespace Extension
{

	/*!
	** \brief Extension: Get the length of the inner buffer
	*/
	template<class C, class SizeT>
	class Length final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 0, isFixed = 0, fixedLength = 0, };

	public:
		template<class U> static SizeT Value(const U&) {return 0;}
	};


} // namespace Extension
} // namespace Yuni


namespace Yuni
{
namespace Traits
{

	/*!
	** \brief Traits: Length (number of items) of an arbitrary container
	**
	** The length is the number of items contained in the container.
	**
	** \tparam U Any class (Static::Remove::Const<> should be used with this parameter)
	** \tparam SizeT The type to use for the returned length
	*/
	template<class U, class SizeT = size_t>
	class Length final
	{
	public:
		//! The original type without its const qualifier
		typedef typename Static::Remove::Const<U>::Type Type;
		//! The type to use for the returned length
		typedef SizeT SizeType;
		//! Extension
		typedef Extension::Length<Type,SizeT>  ExtensionType;
		enum
		{
			//! A non-zero value if the specialization is valid
			valid = ExtensionType::valid,
			//! A non-zero value when the size is known at compile time
			isFixed = ExtensionType::isFixed,
			//! The fixed length value when it can be known at compile time (can be 0), 0 otherwise
			fixedLength = ExtensionType::fixedLength,
		};

	public:
		/*!
		** \brief Get the length of the container
		**
		** \internal The template T is here to manage some special cases with
		**   the const qualifier, especially when U = const char* const. Your compiler
		**   may complain about the following error :
		**   "invalid conversion from ‘const char* const’ to ‘char*’"
		** \param value A arbitrary variable
		** \return The length of the container
		*/
		template<class T>
		static SizeT Value(const T& value) { return ExtensionType::Value(value); }

	}; // class Length<U>





} // namespace Traits
} // namespace Yuni

#include "extension/length.h"
