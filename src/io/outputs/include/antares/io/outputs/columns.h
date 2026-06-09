// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "antares/io/outputs/IColumnAdapterVisitor.h"
#include "antares/optimisation/linear-problem-api/hasStatus.h"

// Forward declarations
namespace Antares::Writer
{
class IColumnAdapter;
} // namespace Antares::Writer

using namespace Antares::Writer;

namespace Antares::IO::Outputs
{

class IColumn
{
public:
    explicit IColumn(const std::string name):
        name_(name)
    {
    }

    virtual ~IColumn() = default;

    // gp : not sure that toString(index) should stay in this class : it's only used if
    // gp : we want to write the simulation table in csv format,
    // gp : so it's not a responsibility of the column to know how to format itself as a string, but
    // gp : rather of the csv writer to know how to format a column value as a string.
    [[nodiscard]] virtual std::string toString(size_t index) const = 0;

    [[nodiscard]] virtual size_t size() const = 0;
    virtual void reserve(size_t capacity) = 0;
    virtual void clear() = 0;

    // Accept visitor and return adapter directly
    virtual std::shared_ptr<IColumnAdapter> accept(IColumnAdapterVisitor& visitor) const = 0;

    std::string name() const
    {
        return name_;
    }

private:
    std::string name_;
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

[[maybe_unused]] static std::string FromDouble(const double value)
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
    else if constexpr (std::is_same_v<U, Optimisation::LinearProblemApi::MipBasisStatus>)
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
    explicit TypedColumn(std::string name):
        IColumn(name)
    {
    }

    void add(const T& value)
    {
        data_.push_back(value);
    }

    [[nodiscard]] std::string toString(size_t index) const override
    {
        return FormatValue(data_.at(index));
    }

    [[nodiscard]] size_t size() const override
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

    std::shared_ptr<IColumnAdapter> accept(IColumnAdapterVisitor& visitor) const override
    {
        return visitor.visit(*this);
    }

private:
    std::vector<T> data_;
};
} // namespace Antares::IO::Outputs
