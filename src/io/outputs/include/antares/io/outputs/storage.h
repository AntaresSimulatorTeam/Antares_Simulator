#pragma once
#include <stdexcept>
#include <type_traits>

#include "antares/io/outputs/columns.h"

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

// Primary template: applies to any type that's NOT an optional
template<typename T>
struct unwrap_optional
{
    using type = T; // just return the type unchanged
};

// Specialization for std::optional<U>
template<typename U>
struct unwrap_optional<std::optional<U>>
{
    using type = U; // remove the optional and return the inner type
};

template<typename T>
using unwrap_optional_t = typename unwrap_optional<T>::type;

class ColumnBasedStorage
{
public:
    void addStringColumn(const std::string& name)
    {
        addColumn<StringColumn>(name);
    }

    void addIntColumn(const std::string& name)
    {
        addColumn<IntColumn>(name);
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
            getColumn<IntColumn>(column_name).add(static_cast<int>(value));
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            getColumn<DoubleColumn>(column_name).add(static_cast<double>(value));
        }
        else if constexpr (is_optional_v<T>)
        {
            using Inner = unwrap_optional_t<T>;
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

private:
    template<typename T>
    struct always_false: std::false_type
    {
    };

    template<typename ColumnType>
    void addColumn(const std::string& name)
    {
        // columns_[name] = std::make_unique<ColumnType>();
        // column_order_.push_back(name);

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

    // std::unordered_map<std::string, std::unique_ptr<IColumn>> columns_;
    // std::vector<std::string> column_order_;

    std::vector<std::unique_ptr<IColumn>> columns_;
    std::vector<std::string> columnNames_;
    std::unordered_map<std::string, size_t> name_to_index_;
};
