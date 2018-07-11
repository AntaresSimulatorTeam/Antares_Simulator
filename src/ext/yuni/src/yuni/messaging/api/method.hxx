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
#ifndef __YUNI_MESSAGE_API_METHOD_HXX__
# define __YUNI_MESSAGE_API_METHOD_HXX__


namespace Yuni
{
namespace Messaging
{
namespace API
{

	inline Method::Method() :
		pCallback(nullptr)
	{}


	inline const String& Method::name() const
	{
		return pName;
	}


	inline const String& Method::brief() const
	{
		return pBrief;
	}


	inline Method& Method::brief(const AnyString& text)
	{
		pBrief = text;
		return *this;
	}


	inline Method& Method::invoke(Callback callback)
	{
		pCallback = callback;
		return *this;
	}


	inline Method::Callback Method::invoke() const
	{
		return pCallback;
	}


	inline Method& Method::option(const AnyString& key, const AnyString& value)
	{
		pOptions[(pTmp = key)] = value;
		return *this;
	}


	inline const String& Method::option(const AnyString& key) const
	{
		return pOptions[(pTmp = key)];
	}


	inline const Method::Parameter::Hash& Method::params() const
	{
		return pParams;
	}


	inline Method::Parameter::Parameter() :
		hasDefault(false)
	{
	}





} // namespace API
} // namespace Messaging
} // namespace Yuni

#endif // __YUNI_MESSAGE_API_METHOD_HXX__
