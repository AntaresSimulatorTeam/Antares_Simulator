// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares
{
inline bool IniFile::loaded() const
{
    return not filename_.empty();
}

inline bool IniFile::empty() const
{
    return not firstSection;
}

inline IniFile::Section::Section(const AnyString& name):
    name(name)
{
}

template<class U>
IniFile::Property::Property(const AnyString& key, const U& value):
    key(key),
    value(value)
{
    this->key.trim();
    this->key.toLower();
    this->value.trim();
}

inline bool IniFile::Section::empty() const
{
    return !firstProperty;
}

template<class U>
IniFile::Property* IniFile::Section::add(const AnyString& key, const U& value)
{
    auto* p = new Property(key, value);
    if (!lastProperty)
    {
        firstProperty = p;
    }
    else
    {
        lastProperty->next = p;
    }
    lastProperty = p;
    return p;
}

template<class U>
IniFile::Property* IniFile::Section::add(const AnyString& key, const std::optional<U>& value)
{
    if (value.has_value())
    {
        return add(key, value.value());
    }
    return nullptr;
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

inline const std::string& IniFile::filename() const
{
    return filename_;
}

template<class CallbackT>
void IniFile::each(const CallbackT& callback)
{
    for (auto* section = firstSection; section; section = section->next)
    {
        callback(*section);
    }
}

template<class CallbackT>
void IniFile::each(const CallbackT& callback) const
{
    for (auto* section = firstSection; section; section = section->next)
    {
        callback(*section);
    }
}

template<class CallbackT>
void IniFile::Section::each(const CallbackT& callback)
{
    for (auto* property = firstProperty; property; property = property->next)
    {
        callback(*property);
    }
}

template<class CallbackT>
void IniFile::Section::each(const CallbackT& callback) const
{
    for (auto* property = firstProperty; property; property = property->next)
    {
        callback(*property);
    }
}

template<class CallbackT>
void IniFile::properties(const CallbackT& callback)
{
    for (auto* section = firstSection; section; section = section->next)
    {
        for (auto* property = section->firstProperty; property; property = property->next)
        {
            callback(*property);
        }
    }
}

template<class CallbackT>
void IniFile::properties(const CallbackT& callback) const
{
    for (auto* section = firstSection; section; section = section->next)
    {
        for (auto* property = section->firstProperty; property; property = property->next)
        {
            callback(*property);
        }
    }
}

} // namespace Antares
