/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_INIFILE_HXX__
# define __ANTARES_LIBS_INIFILE_HXX__


namespace Antares
{

	inline bool IniFile::loaded() const
	{
		return not pFilename.empty();
	}


	inline bool IniFile::empty() const
	{
		return not firstSection;
	}


	inline IniFile::Section::Section() :
		firstProperty(nullptr),
		lastProperty(nullptr),
		next(nullptr)
	{}


	inline IniFile::Section::Section(const AnyString& name) :
		name(name),
		firstProperty(nullptr),
		lastProperty(nullptr),
		next(nullptr)
	{
	}


	template<class U>
	IniFile::Property::Property(const AnyString& key, const U& value) :
		key(key),
		value(value),
		next(nullptr)
	{
		this->key.trim();
		this->key.toLower();
		this->value.trim();
	}


	inline bool IniFile::Section::empty() const
	{
		return (NULL == firstProperty);
	}


	template<class U>
	IniFile::Property* IniFile::Section::add(const AnyString& key, const U& value)
	{
		auto* p = new Property(key, value);
		if (!lastProperty)
			firstProperty = p;
		else
			lastProperty->next = p;
		lastProperty = p;
		return p;
	}


	template<class U, class StringT>
	inline U IniFile::Section::read(const StringT& key, const U& defValue) const
	{
		auto* property = find(key);
		return (property ? property->value.template to<U>() : defValue);
	}


	inline IniFile::Section* IniFile::addSection(const AnyString& name)
	{
		return add(new Section(name));
	}


	inline const YString& IniFile::filename() const
	{
		return pFilename;
	}


	template<class CallbackT>
	void IniFile::each(const CallbackT& callback)
	{
		for (auto* section = firstSection; section; section = section->next)
			callback(*section);
	}


	template<class CallbackT>
	void IniFile::each(const CallbackT& callback) const
	{
		for (auto* section = firstSection; section; section = section->next)
			callback(*section);
	}


	template<class CallbackT>
	void IniFile::Section::each(const CallbackT& callback)
	{
		for (auto* property = firstProperty; property; property = property->next)
			callback(*property);
	}


	template<class CallbackT>
	void IniFile::Section::each(const CallbackT& callback) const
	{
		for (auto* property = firstProperty; property; property = property->next)
			callback(*property);
	}


	template<class CallbackT>
	void IniFile::properties(const CallbackT& callback)
	{
		for (auto* section = firstSection; section; section = section->next)
		{
			for (auto* property = section->firstProperty; property; property = property->next)
				callback(*property);
		}
	}


	template<class CallbackT>
	void IniFile::properties(const CallbackT& callback) const
	{
		for (auto* section = firstSection; section; section = section->next)
		{
			for (auto* property = section->firstProperty; property; property = property->next)
				callback(*property);
		}
	}





} // namespace Antares

#endif // __ANTARES_LIBS_INIFILE_HXX__
