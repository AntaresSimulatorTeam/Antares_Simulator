// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    std::shared_ptr<StringColumn> addStringColumn(const std::string& name)
    {
        return addColumn<StringColumn>(name);
    }

    std::shared_ptr<IntegralColumn> addIntegralColumn(const std::string& name)
    {
        return addColumn<IntegralColumn>(name);
    }

    std::shared_ptr<DoubleColumn> addDoubleColumn(const std::string& name)
    {
        return addColumn<DoubleColumn>(name);
    }

    template<typename T>
    std::shared_ptr<OptionalColumn<T>> addOptionalColumn(const std::string& name)
    {
        return addColumn<OptionalColumn<T>>(name);
    }

    [[nodiscard]] size_t rowCount() const
    {
        return columns_.empty() ? 0 : (*columns_.begin())->size();
    }

    [[nodiscard]] const std::vector<std::shared_ptr<IColumn>>& columns() const
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
    std::shared_ptr<ColumnType> addColumn(const std::string& name)
    {
        auto it = std::ranges::find_if(columns_,
                                       [&name](const auto& c) { return c->name() == name; });
        if (it != columns_.end())
        {
            throw std::runtime_error("Column already exists: " + name);
        }
        auto col = std::make_shared<ColumnType>(name);
        columns_.push_back(col);
        return col;
    }

    std::vector<std::shared_ptr<IColumn>> columns_;
};
} // namespace Antares::IO::Outputs
