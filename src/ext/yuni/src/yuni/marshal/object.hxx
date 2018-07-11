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
#include "object.h"



namespace Yuni
{
namespace Marshal
{

	inline Object::Object()
		: pType(otNil)
	{}


	inline Object::Object(Type type, InternalDatatype value)
		: pType(type)
		, pValue(value)
	{}

	#ifdef YUNI_HAS_CPP_MOVE
	inline Object::Object(Object&& rhs)
		: pType(rhs.pType)
		, pValue(rhs.pValue)
	{
		rhs.pType = otNil;
	}
	#endif


	inline void Object::assign(const char* string)
	{
		assign(AnyString(string));
	}


	template<uint N>
	inline void Object::assign(const char string[N])
	{
		assign(AnyString(string));
	}


	template<uint N>
	inline Object& Object::operator = (const char string[N])
	{
		assign(AnyString(string));
		return *this;
	}


	inline Object& Object::operator = (const Object& rhs)
	{
		assign(rhs);
		return *this;
	}


	template<class T>
	inline Object& Object::operator = (const T& value)
	{
		assign(value);
		return *this;
	}


	template<class T>
	inline Object& Object::operator += (const T& value)
	{
		append(value);
		return *this;
	}


	inline Object::Type Object::type() const
	{
		return pType;
	}


	template<class T>
	inline void Object::push_back(const T& value)
	{
		append(value);
	}


	inline void Object::swap(Object& second)
	{
		std::swap(pType, second.pType);
		std::swap(pValue, second.pValue);
	}




} // namespace Marshal
} // namespace Yuni
