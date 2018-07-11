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
#ifndef __YUNI_CORE_VALIDATOR_DEFAULT_HXX__
# define __YUNI_CORE_VALIDATOR_DEFAULT_HXX__


namespace Yuni
{
namespace Validator
{
namespace Text
{


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>::ExceptionList()
	{}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>::ExceptionList(const ExceptionList<DefaultPolicy>& rhs)
		:pExceptionList(rhs.pExceptionList)
	{}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
	inline ExceptionList<DefaultPolicy>::ExceptionList(const ExceptionList<OtherDefaultPolicy>& rhs)
		:pExceptionList(rhs.pExceptionList)
	{}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>::ExceptionList(const String::Vector& rhs)
		:pExceptionList(rhs)
	{}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>::ExceptionList(const String::List& rhs)
	{
		if (!rhs.empty())
		{
			const String::List::const_iterator end = rhs.end();
			for (String::List::const_iterator i = rhs.begin(); i != end; ++i)
				pExceptionList.push_back(*i);
		}
	}




	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	template<typename U>
	bool ExceptionList<DefaultPolicy>::validate(const U& s) const
	{
		if (!pExceptionList.empty())
		{
			const String::Vector::const_iterator end = pExceptionList.end();
			for (String::Vector::const_iterator i = pExceptionList.begin(); i != end; ++i)
			{
				if (*i == s)
					return !DefaultPolicy;
			}
		}
		return DefaultPolicy;
	}



	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	template<class U>
	inline void ExceptionList<DefaultPolicy>::exception(const U& e)
	{
		pExceptionList.push_back(e);
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator = (const ExceptionList<OtherDefaultPolicy>& rhs)
	{
		pExceptionList = rhs.pExceptionList;
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator = (const String::Vector& rhs)
	{
		pExceptionList = rhs;
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator = (const String::List& rhs)
	{
		pExceptionList.clear();
		if (!rhs.empty())
		{
			const String::List::const_iterator end = rhs.end();
			for (String::List::const_iterator i = rhs.begin(); i != end; ++i)
				pExceptionList.push_back(*i);
		}
		return *this;
	}




	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator += (const AnyString& rhs)
	{
		pExceptionList.push_back(rhs);
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator += (const ExceptionList<OtherDefaultPolicy>& rhs)
	{
		pExceptionList += rhs.pExceptionList;
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator += (const String::Vector& rhs)
	{
		pExceptionList += rhs;
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator += (const String::List& rhs)
	{
		if (!rhs.empty())
		{
			const String::List::const_iterator end = rhs.end();
			for (String::List::const_iterator i = rhs.begin(); i != end; ++i)
				pExceptionList.push_back(*i);
		}
		return *this;
	}



	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator << (const AnyString& rhs)
	{
		pExceptionList.push_back(rhs);
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator << (const ExceptionList<OtherDefaultPolicy>& rhs)
	{
		pExceptionList += rhs.pExceptionList;
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	inline ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator << (const String::Vector& rhs)
	{
		pExceptionList += rhs;
		return *this;
	}


	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	ExceptionList<DefaultPolicy>& ExceptionList<DefaultPolicy>::operator << (const String::List& rhs)
	{
		if (!rhs.empty())
		{
			const String::List::const_iterator end = rhs.end();
			for (String::List::const_iterator i = rhs.begin(); i != end; ++i)
				pExceptionList.push_back(*i);
		}
		return *this;
	}




} // namespace Text
} // namespace Validator
} // namespace Yuni

#endif // __YUNI_CORE_VALIDATOR_DEFAULT_H__
