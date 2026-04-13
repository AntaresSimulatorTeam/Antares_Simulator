// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "columnToArrowAdapter.h"

#include <stdexcept>
#include "parquet_arrow_utils.h"

using namespace Antares::IO::Outputs;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Writer
{

// -----------------------------------------------------------------------
// Helper: convert a vector of optional<T> into (values, is_valid) pair
// Arrow needs a flat value buffer (any value for nulls) + a bool mask
// -----------------------------------------------------------------------
template<typename T>
std::pair<std::vector<T>, std::vector<bool>> SplitOptionals(
  const std::vector<std::optional<T>>& input)
{
    std::vector<T> values(input.size());
    std::vector<bool> is_valid(input.size());
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        is_valid[i] = input[i].has_value();
        values[i] = input[i].value_or(T{}); // placeholder for nulls
    }
    return {values, is_valid};
}

std::vector<std::optional<unsigned>> convertToOptInt(
  const std::vector<std::optional<MipBasisStatus>>& v)
{
    std::vector<std::optional<unsigned>> to_return;
    to_return.reserve(v.size());
    for (const auto& e: v)
    {
        if (e.has_value())
        {
            to_return.push_back(static_cast<unsigned>(*e));
        }
        else
        {
            to_return.push_back(std::nullopt);
        }
    }
    return to_return;
}

// ==========================
// Class StringColumnAdapter
// ==========================
StringColumnAdapter::StringColumnAdapter(const StringColumn* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> StringColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::utf8());
}

std::shared_ptr<arrow::Array> StringColumnAdapter::makeArray() const
{
    arrow::StringBuilder builder;
    ARROW_THROW_NOT_OK(builder.AppendValues(column_->data()));

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());
    return array;
}

// ==========================
// Class DoubleColumnAdapter
// ==========================
DoubleColumnAdapter::DoubleColumnAdapter(const DoubleColumn* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> DoubleColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::float64());
}

std::shared_ptr<arrow::Array> DoubleColumnAdapter::makeArray() const
{
    arrow::DoubleBuilder builder;
    ARROW_THROW_NOT_OK(builder.AppendValues(column_->data()));

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());
    return array;
}

// ============================
// Class IntColumnAdapter
// ============================
IntColumnAdapter::IntColumnAdapter(const IntegralColumn<unsigned>* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> IntColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::uint32());
}

std::shared_ptr<arrow::Array> IntColumnAdapter::makeArray() const
{
    arrow::UInt32Builder builder;
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
    return arrow::field(column_->name(), arrow::utf8());
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

// ================================
// Class OptDoubleColumnAdapter
// ================================
OptDoubleColumnAdapter::OptDoubleColumnAdapter(const OptionalColumn<double>* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> OptDoubleColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::float64());
}

std::shared_ptr<arrow::Array> OptDoubleColumnAdapter::makeArray() const
{
    arrow::DoubleBuilder builder;

    auto [values, valid_values] = SplitOptionals(column_->data());
    ARROW_THROW_NOT_OK(builder.AppendValues(values, valid_values));

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());
    return array;
}

// ================================
// Class OptIntColumnAdapter
// ================================
OptIntColumnAdapter::OptIntColumnAdapter(const OptionalColumn<unsigned>* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> OptIntColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::uint32());
}

std::shared_ptr<arrow::Array> OptIntColumnAdapter::makeArray() const
{
    arrow::UInt32Builder builder;

    auto [values, valid_values] = SplitOptionals(column_->data());
    ARROW_THROW_NOT_OK(builder.AppendValues(values, valid_values));

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());
    return array;
}

// ========================================
// Class OptMipBasisStatusColumnAdapter
// ========================================
OptMipBasisStatusColumnAdapter::OptMipBasisStatusColumnAdapter(
  const OptionalColumn<MipBasisStatus>* column):
    column_(column)
{
}

std::shared_ptr<arrow::Field> OptMipBasisStatusColumnAdapter::makeField() const
{
    return arrow::field(column_->name(), arrow::uint32());
}

std::shared_ptr<arrow::Array> OptMipBasisStatusColumnAdapter::makeArray() const
{
    arrow::UInt32Builder builder;

    auto [values, valid_values] = SplitOptionals(convertToOptInt(column_->data()));
    ARROW_THROW_NOT_OK(builder.AppendValues(values, valid_values));

    std::shared_ptr<arrow::Array> array;
    ARROW_THROW_ASSIGN(array, builder.Finish());
    return array;
}

// ==========================
// Column adapter factory
// ==========================
std::shared_ptr<IColumnAdapter> makeColumnAdapter(const std::unique_ptr<IColumn>& column)
{
    if (auto* c = dynamic_cast<StringColumn*>(column.get()))
    {
        return std::make_shared<StringColumnAdapter>(c);
    }
    if (auto* c = dynamic_cast<DoubleColumn*>(column.get()))
    {
        return std::make_shared<DoubleColumnAdapter>(c);
    }
    if (auto* c = dynamic_cast<IntegralColumn<unsigned>*>(column.get()))
    {
        return std::make_shared<IntColumnAdapter>(c);
    }
    if (auto* c = dynamic_cast<OptionalColumn<std::string>*>(column.get()))
    {
        return std::make_shared<OptStringColumnAdapter>(c);
    }
    if (auto* c = dynamic_cast<OptionalColumn<double>*>(column.get()))
    {
        return std::make_shared<OptDoubleColumnAdapter>(c);
    }
    if (auto* c = dynamic_cast<OptionalColumn<unsigned>*>(column.get()))
    {
        return std::make_shared<OptIntColumnAdapter>(c);
    }
    if (auto* c = dynamic_cast<OptionalColumn<MipBasisStatus>*>(column.get()))
    {
        return std::make_shared<OptMipBasisStatusColumnAdapter>(c);
    }

    throw std::invalid_argument("makeColumnAdapter: column type unknown: " + column->name());
}

} // namespace Antares::Writer
