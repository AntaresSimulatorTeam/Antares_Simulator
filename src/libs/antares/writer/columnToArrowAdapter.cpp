// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "columnToArrowAdapter.h"

using namespace Antares::IO::Outputs;

// Helper: throw on Arrow errors
#define ARROW_THROW_NOT_OK(expr)                      \
    do                                                \
    {                                                 \
        arrow::Status _st = (expr);                   \
        if (!_st.ok())                                \
            throw std::runtime_error(_st.ToString()); \
    } while (0)

#define ARROW_THROW_ASSIGN(lhs, expr)                           \
    do                                                          \
    {                                                           \
        auto _res = (expr);                                     \
        if (!_res.ok())                                         \
            throw std::runtime_error(_res.status().ToString()); \
        lhs = std::move(_res).ValueOrDie();                     \
    } while (0)

namespace Antares::Writer
{

// ==========================
// Class StringColumnAdapter
// ==========================
StringColumnAdapter::StringColumnAdapter(const StringColumn* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> StringColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::int32());
}

std::shared_ptr<arrow::Array> StringColumnAdapter::makeArray() const
{
    arrow::StringBuilder builder;
    ARROW_THROW_NOT_OK(builder.AppendValues(column_->data()));

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());

    return array;
}

// ================================
// Class OptStringColumnAdapter
// ================================
OptStringColumnAdapter::OptStringColumnAdapter(const OptionalColumn<std::string>* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> OptStringColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::int32());
}

std::shared_ptr<arrow::Array> OptStringColumnAdapter::makeArray() const
{
    arrow::StringBuilder builder;

    for (const auto& element: column_->data())
    {
        if (element.has_value())
        {
            ARROW_THROW_NOT_OK(builder.Append(*element));
        }
        else
        {
            ARROW_THROW_NOT_OK(builder.AppendEmptyValue());
        }
    }

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());

    return array;
}

// ==========================
// Column adapter factory
// ==========================
std::shared_ptr<IColumnAdapter> makeColumnAdapter(const std::unique_ptr<IColumn>& column)
{
    if (StringColumn* c = dynamic_cast<StringColumn*>(column.get()))
    {
        return std::make_shared<StringColumnAdapter>(c);
    }
    if (OptionalColumn<std::string>* c = dynamic_cast<OptionalColumn<std::string>*>(column.get()))
    {
        return std::make_shared<OptStringColumnAdapter>(c);
    }

    else
    {
        throw std::invalid_argument("ParquetTableWriter: column type unknown");
    }
}

} // namespace Antares::Writer
