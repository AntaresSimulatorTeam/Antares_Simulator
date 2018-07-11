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
namespace GetOpt
{

	template<class U>
	inline void Parser::add(U& var, char shortname)
	{
		// The new option
		IOption* option = (IOption*) new Private::GetOptImpl::Option<U, true>(var, shortname);
		// append the new option
		appendShortOption(option, shortname);
	}


	template<class U>
	inline void Parser::addFlag(U& var, char shortname)
	{
		// The new option
		IOption* option = (IOption*) new Private::GetOptImpl::Option<U, true, false>(var, shortname);
		// append the new option
		appendShortOption(option, shortname);
	}


	template<class U>
	inline void Parser::add(U& var, char shortname, const AnyString& longname)
	{
		IOption* option = (IOption*) new Private::GetOptImpl::Option<U, true>(var, shortname, longname);
		// append the new option
		appendOption(option, shortname);
	}


	template<class U>
	inline void Parser::add(U& var, char shortname, const AnyString& longname, const AnyString& description)
	{
		IOption* option = (IOption*) new Private::GetOptImpl::Option<U, true>(var, shortname, longname, description);
		// append the new option
		appendOption(option, shortname);
	}



	template<class U>
	inline void Parser::addFlag(U& var, char shortname, const AnyString& longname)
	{
		// The new option
		IOption* option = (IOption*) new Private::GetOptImpl::Option<U, true, false>(var, shortname, longname);
		// append the new option
		appendOption(option, shortname);
	}


	template<class U>
	void Parser::addFlag(U& var, char shortname, const AnyString& longname, const AnyString& description)
	{
		// The new option
		IOption* option = (IOption*) new Private::GetOptImpl::Option<U, true, false>(var, shortname, longname, description);
		// append the new option
		appendOption(option, shortname);
	}


	template<class U>
	void Parser::addFlag(U& var, char shortname, const AnyString& longname, const AnyString& description, bool visible)
	{
		// The new option
		IOption* option = (visible)
			? (IOption*) new Private::GetOptImpl::Option<U, true, false>(var, shortname, longname, description)
			: (IOption*) new Private::GetOptImpl::Option<U, false, false>(var, shortname, longname, description);
		// append the new option
		appendOption(option, shortname);
	}


	template<class U>
	void Parser::add(U& var, char shortname, const AnyString& longname, const AnyString& description, bool visible)
	{
		// The new option
		IOption* option = (visible)
			? (IOption*) new Private::GetOptImpl::Option<U, true>(var, shortname, longname, description)
			: (IOption*) new Private::GetOptImpl::Option<U, false>(var, shortname, longname, description);
		// append the new option
		appendOption(option, shortname);
	}



	template<class U>
	inline void Parser::remainingArguments(U& var)
	{
		delete pRemains;
		pRemains = new Private::GetOptImpl::Option<U, false>(var, '\0');
	}


	inline void Parser::addParagraph(const AnyString& text)
	{
		pAllOptions.push_back(new Private::GetOptImpl::Paragraph(text));
	}


	inline uint Parser::errors() const
	{
		return pErrors;
	}


	inline bool Parser::ignoreUnknownArgs() const
	{
		return pIgnoreUnknownArgs;
	}


	inline void Parser::ignoreUnknownArgs(bool ignore)
	{
		pIgnoreUnknownArgs = ignore;
	}





} // namespace GetOpt
} // namespace Yuni

