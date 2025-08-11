/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class IColumn
{
public:
    virtual ~IColumn() = default;
    virtual void addFromString(const std::string& value) = 0;
    virtual std::string toString(size_t index) const = 0;
    virtual size_t size() const = 0;
    virtual void reserve(size_t capacity) = 0;
};

template<typename T>
class TypedColumn: public IColumn
{
public:
    explicit TypedColumn(std::function<std::string(const T&)> toStringFn = defaultToString,
                         std::function<T(const std::string&)> fromStringFn = defaultFromString):
        toStringFn_(toStringFn),
        fromStringFn_(fromStringFn)
    {
    }

    void add(const T& value)
    {
        data_.push_back(value);
    }

    void addFromString(const std::string& value) override
    {
        data_.push_back(fromStringFn_(value));
    }

    std::string toString(size_t index) const override
    {
        return toStringFn_(data_.at(index));
    }

    const T& get(size_t index) const
    {
        return data_.at(index);
    }

    size_t size() const override
    {
        return data_.size();
    }

    void reserve(size_t capacity) override
    {
        data_.reserve(capacity);
    }

    const std::vector<T>& data() const
    {
        return data_;
    }

    static std::string defaultToString(const T& v)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return v;
        }
        else
        {
            return std::to_string(v);
        }
    }

    static T defaultFromString(const std::string& s)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return s;
        }
        else
        {
            T v;
            std::istringstream iss(s);
            iss >> v;
            return v;
        }
    }

private:
    std::vector<T> data_;
    std::function<std::string(const T&)> toStringFn_;
    std::function<T(const std::string&)> fromStringFn_;
};

class StringColumn: public TypedColumn<std::string>
{
public:
    StringColumn():
        TypedColumn()
    {
    }
};

class IntColumn: public TypedColumn<int>
{
public:
    IntColumn():
        TypedColumn()
    {
    }
};

class DoubleColumn: public TypedColumn<double>
{
public:
    static std::string FromDouble(const double v)
    {
        std::ostringstream oss;
        oss << std::setprecision(15) << v;
        return oss.str();
    }

    static double ToDouble(const std::string& s)
    {
        return std::stod(s);
    }

    DoubleColumn():
        TypedColumn(FromDouble, ToDouble)
    {
    }
};

template<typename T>
class OptionalColumn: public TypedColumn<std::optional<T>>
{
public:
    std::string FromOptional(const std::optional<T>& option)
    {
        if (option.has_value())
        {
            if constexpr (std::is_floating_point_v<T>)
            {
                return FromDouble(option.value());
            }
            else
            {
                TypedColumn<T>::defaultToString(option.value());
            }
        }
        else
        {
            return "NONE";
        }
    }

    std::optional<T> ToOptional(const std::string& s)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return DoubleColumn::ToDouble(s);
        }
        else
        {
            return TypedColumn<T>::defaultFromString(s);
        }
    }

    OptionalColumn():
        TypedColumn<T>(FromOptional, ToOptional)

    {
    }
};
