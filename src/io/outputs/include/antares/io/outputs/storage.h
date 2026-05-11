#pragma once
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "antares/io/outputs/columns.h"

namespace Antares::IO::Outputs
{
class ColumnBasedStorage
{
public:
    void addStringColumn(const std::string& name)
    {
        addColumn<StringColumn>(name);
    }

    template<Integral T>
    void addIntegralColumn(const std::string& name)
    {
        addColumn<IntegralColumn<T>>(name);
    }

    void addDoubleColumn(const std::string& name)
    {
        addColumn<DoubleColumn>(name);
    }

    template<typename T>
    void addOptionalColumn(const std::string& name)
    {
        addColumn<OptionalColumn<T>>(name);
    }

    template<typename T>
    void addValue(const std::string& column_name, const T& value)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            getColumn<StringColumn>(column_name).add(value);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            getColumn<IntegralColumn<T>>(column_name).add(value);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            getColumn<DoubleColumn>(column_name).add(static_cast<double>(value));
        }
        else if constexpr (is_optional_v<T>)
        {
            using Inner = T::value_type;
            getColumn<OptionalColumn<Inner>>(column_name).add(value);
        }
        else
        {
            throw std::runtime_error("Unsupported type"); // TODO
        }
    }

    [[nodiscard]] size_t rowCount() const
    {
        return columns_.empty() ? 0 : (*columns_.begin())->size();
    }

    [[nodiscard]] const IColumn& getColumn(const std::string& name) const
    {
        const auto it = std::ranges::find_if(columns_,
                                             [&name](const auto& c) { return c->name() == name; });
        if (it == columns_.end())
        {
            throw std::runtime_error("Column not found: " + name);
        }
        return **it;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<IColumn>>& columns() const
    {
        return columns_;
    }

    void clear() const
    {
        for (auto& column: columns_)
        {
            column->clear();
        }
    }

private:
    template<typename ColumnType>
    void addColumn(const std::string& name)
    {
        auto it = std::ranges::find_if(columns_,
                                       [&name](const auto& c) { return c->name() == name; });
        if (it != columns_.end())
        {
            throw std::runtime_error("Column already exists: " + name);
        }
        auto col = std::make_unique<ColumnType>(name);
        columns_.push_back(std::move(col));
    }

    // Access column by name
    template<typename ColumnType>
    ColumnType& getColumn(const std::string& name)
    {
        const auto it = std::ranges::find_if(columns_,
                                             [&name](const auto& c) { return c->name() == name; });
        if (it == columns_.end())
        {
            throw std::runtime_error("Column not found: " + name);
        }
        return dynamic_cast<ColumnType&>(**it);
    }

    // Access column by index
    template<typename ColumnType>
    ColumnType& getColumn(const size_t index)
    {
        if (index >= columns_.size())
        {
            throw std::out_of_range("Column index out of range");
        }
        return dynamic_cast<ColumnType&>(*columns_[index]);
    }

    std::vector<std::unique_ptr<IColumn>> columns_;
};
} // namespace Antares::IO::Outputs
