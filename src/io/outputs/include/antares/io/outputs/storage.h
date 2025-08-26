#pragma once
#include <stdexcept>
#include <unordered_map>

#include "antares/io/outputs/columns.h"

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
            using Inner = typename T::value_type;
            getColumn<OptionalColumn<Inner>>(column_name).add(value);
        }
        else
        {
            throw std::runtime_error("Unsupported type"); // TODO
        }
    }

    size_t rowCount() const
    {
        return columns_.empty() ? 0 : (*columns_.begin())->size();
    }

    const std::unordered_map<std::string, size_t>& columnsNameToIndex() const
    {
        return name_to_index_;
    }

    const IColumn& getColumn(const std::string& name) const
    {
        const auto it = name_to_index_.find(name);
        if (it == name_to_index_.end())
        {
            throw std::runtime_error("Column not found: " + name);
        }
        return *columns_.at(it->second);
    }

    [[nodiscard]] const std::vector<std::unique_ptr<IColumn>>& columns() const
    {
        return columns_;
    }

    [[nodiscard]] const std::vector<std::string>& columnNames() const
    {
        return columnNames_;
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
        if (name_to_index_.contains(name))
        {
            throw std::runtime_error("Column already exists: " + name);
        }

        std::size_t index = columns_.size();
        auto col = std::make_unique<ColumnType>();
        columns_.push_back(std::move(col));
        columnNames_.push_back(name);
        name_to_index_.emplace(std::move(name), index);
    }

    // Access column by name
    template<typename ColumnType>
    ColumnType& getColumn(const std::string& name)
    {
        const auto it = name_to_index_.find(name);
        if (it == name_to_index_.end())
        {
            throw std::runtime_error("Column not found: " + name);
        }
        return dynamic_cast<ColumnType&>(*columns_[it->second]);
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
    std::vector<std::string> columnNames_;
    std::unordered_map<std::string, size_t> name_to_index_;
};
