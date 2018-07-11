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
	** \brief Extension: Convert any type into a mere C-String
	*/
	template<class C> class IntoCString final
	{
	public:
		enum { valid = 0, converted = 0, zeroTerminated = 0, };

	public:
		template<class U> static const char* Perform(const U&) {return NULL;}

	};

} // namespace Extension
} // namespace Yuni


namespace Yuni
{
namespace Traits
{

	/*!
	** \brief Traits: C-String (const char*) representation of an arbitrary contrainer
	**
	** \tparam U Any class. See the specialization of the class Yuni::Extension::IntoCString
	*/
	template<class U>
	class CString final
	{
	public:
		//! The original type without its const qualifier
		typedef typename Static::Remove::Const<U>::Type Type;
		//! Extension
		typedef Extension::IntoCString<Type> ExtensionType;
		enum
		{
			//! A non-zero value if the specialization is valid
			valid = ExtensionType::valid,
			//! A non-zero value if the data have been converted
			converted = ExtensionType::converted,
			//! A non-zero value if the data is zero-terminated
			zeroTerminated = ExtensionType::zeroTerminated,
		};

	public:
		/*!
		** \brief Get a C-String representation of an arbitrary contrainer
		**
		** \internal The template T is here to manage some special cases with
		**   the const qualifier, especially when U = const char* const. Your compiler
		**   may complain about the following error :
		**   "invalid conversion from ‘const char* const’ to ‘char*’"
		** \param value A arbitrary variable
		** \return The equivalent of a `const char*` pointer (can be NULL)
		*/
		template<class T>
		static const char* Perform(const T& value) { return ExtensionType::Perform(value); }

	}; // class CString<U>




} // namespace Traits
} // namespace Yuni

#include "extension/into-cstring.h"
