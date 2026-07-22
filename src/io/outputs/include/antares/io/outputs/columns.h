// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <cstdint>
#include <fmt/format.h>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
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
    explicit IColumn(const std::string& name):
        name_(name)
    {
    }

    virtual ~IColumn() = default;

    // gp : not sure that toString(index) should stay in this class : it's only used if
    // gp : we want to write the simulation table in csv format,
    // gp : so it's not a responsibility of the column to know how to format itself as a string, but
    // gp : rather of the csv writer to know how to format a column value as a string.
    [[nodiscard]] virtual std::string toString(size_t index) const = 0;

    // Formats the value at `index` directly into `buf`, quoting it if needed.
    // Used by the CSV writer's hot path to avoid allocating a std::string per cell.
    virtual void appendTo(fmt::memory_buffer& buf, size_t index) const = 0;

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
    return fmt::format("{:.15g}", value);
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
    else if constexpr (is_optional_v<U>)
    {
        return v ? FormatValue(*v) : "None";
    }
    else
    {
        return std::to_string(v);
    }
}

// Appends `cell` to `buf`, quoting it only when it contains a character that requires it.
// Appending straight into the shared buffer (rather than returning a std::string per cell)
// avoids one allocation/copy per cell in the common, non-quoted case.
inline void AppendEscaped(fmt::memory_buffer& buf, const std::string& cell)
{
    bool needs_quotes = cell.find_first_of(",\n\r\t\"") != std::string::npos;
    if (!needs_quotes)
    {
        buf.append(cell);
        return;
    }

    buf.push_back('"');
    for (char c: cell)
    {
        if (c == '"')
        {
            buf.push_back('"');
        }
        buf.push_back(c);
    }
    buf.push_back('"');
}

// Formats `v` directly into `buf`. Only the std::string case needs the escaping scan above:
// numeric/status values can never contain characters that require CSV quoting.
template<typename U>
static void AppendFormattedValue(fmt::memory_buffer& buf, const U& v)
{
    if constexpr (std::is_same_v<U, std::string>)
    {
        AppendEscaped(buf, v);
    }
    else if constexpr (std::is_floating_point_v<U>)
    {
        fmt::format_to(std::back_inserter(buf), "{:.15g}", v);
    }
    else if constexpr (is_optional_v<U>)
    {
        if (v)
        {
            AppendFormattedValue(buf, *v);
        }
        else
        {
            buf.append(std::string_view("None"));
        }
    }
    else
    {
        fmt::format_to(std::back_inserter(buf), "{}", v);
    }
}

template<typename T>
class TypedColumn final: public IColumn
{
public:
    explicit TypedColumn(const std::string& name):
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

    void appendTo(fmt::memory_buffer& buf, size_t index) const override
    {
        AppendFormattedValue(buf, data_.at(index));
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

// Column of strings with few distinct values (component / output names): each
// distinct string is stored once in a dictionary and rows only hold a 32-bit
// index into it, instead of one std::string (and its heap allocation) per row.
// An absent value (std::nullopt) is encoded as nullIndex.
class InternedStringColumn final: public IColumn
{
public:
    static constexpr uint32_t nullIndex = std::numeric_limits<uint32_t>::max();

    explicit InternedStringColumn(const std::string& name):
        IColumn(name)
    {
    }

    void add(const std::string& value)
    {
        indices_.push_back(intern(value));
    }

    void add(const std::optional<std::string>& value)
    {
        indices_.push_back(value ? intern(*value) : nullIndex);
    }

    [[nodiscard]] std::string toString(size_t index) const override
    {
        const uint32_t dictionaryIndex = indices_.at(index);
        return dictionaryIndex == nullIndex ? "None" : dictionary_[dictionaryIndex];
    }

    void appendTo(fmt::memory_buffer& buf, size_t index) const override
    {
        const uint32_t dictionaryIndex = indices_.at(index);
        if (dictionaryIndex == nullIndex)
        {
            buf.append(std::string_view("None"));
        }
        else
        {
            AppendEscaped(buf, dictionary_[dictionaryIndex]);
        }
    }

    [[nodiscard]] size_t size() const override
    {
        return indices_.size();
    }

    void reserve(size_t capacity) override
    {
        indices_.reserve(capacity);
    }

    void clear() override
    {
        indices_.clear();
        dictionary_.clear();
        lookup_.clear();
    }

    const std::vector<uint32_t>& indices() const
    {
        return indices_;
    }

    const std::vector<std::string>& dictionary() const
    {
        return dictionary_;
    }

    std::shared_ptr<IColumnAdapter> accept(IColumnAdapterVisitor& visitor) const override
    {
        return visitor.visit(*this);
    }

private:
    uint32_t intern(const std::string& value)
    {
        auto [it, inserted] = lookup_.try_emplace(value, static_cast<uint32_t>(dictionary_.size()));
        if (inserted)
        {
            dictionary_.push_back(value);
        }
        return it->second;
    }

    std::vector<uint32_t> indices_;
    std::vector<std::string> dictionary_;
    std::unordered_map<std::string, uint32_t> lookup_;
};
} // namespace Antares::IO::Outputs
