// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "antares/optimisation/linear-problem-api/hasStatus.h"

class IColumn
{
public:
    virtual ~IColumn() = default;
    virtual std::string toString(size_t index) const = 0;
    virtual size_t size() const = 0;
    virtual void reserve(size_t capacity) = 0;
    virtual void clear() = 0;
};

template<typename T>
struct is_optional: std::false_type
{
};

template<typename U>
struct is_optional<std::optional<U>>: std::true_type
{
};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

static std::string FromDouble(const double value)
{
    std::ostringstream oss;
    oss << std::setprecision(15) << value;
    return oss.str();
}

template<typename U>
static std::string FormatValue(const U& v)
{
    if constexpr (std::is_same_v<U, std::string>)
    {
        return v;
    }
    else if constexpr (std::is_floating_point_v<U>)
    {
        return FromDouble(v);
    }
    else if constexpr (std::is_same_v<U, Antares::Optimisation::LinearProblemApi::MipBasisStatus>)
    {
        return StatusToString(v);
    }
    else if constexpr (is_optional_v<U>)
    {
        return v ? FormatValue(*v) : "None";
    }
    else
    {
        return std::to_string(v);
    }
}

template<typename T>
class TypedColumn final: public IColumn
{
public:
    TypedColumn() = default;

    void add(const T& value)
    {
        data_.push_back(value);
    }

    std::string toString(size_t index) const override
    {
        return FormatValue(data_.at(index));
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

    void clear() override
    {
        data_.clear();
    }

private:
    std::vector<T> data_;
};

using StringColumn = TypedColumn<std::string>;
template<typename T>
concept Integral = std::is_integral_v<T>;
template<Integral T>
using IntegralColumn = TypedColumn<T>;
using DoubleColumn = TypedColumn<double>;
template<typename T>
using OptionalColumn = TypedColumn<std::optional<T>>;
