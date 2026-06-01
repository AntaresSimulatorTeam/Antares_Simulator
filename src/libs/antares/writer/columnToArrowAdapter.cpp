// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "columnToArrowAdapter.h"

// Arrow / Parquet — full headers needed for implementation
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <optional>
#include <stdexcept>
#include <typeinfo>
#include <vector>

#include <antares/exception/InvalidArgumentError.hpp>
#include "antares/io/outputs/IColumnAdapterVisitor.h"

#include "private/parquet_arrow_utils.h"

using namespace Antares::IO::Outputs;
using namespace Antares::Error;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Writer
{

// ===========================
// Helper: add optional values to Arrow builder
// ===========================

template<typename T, typename B>
void addOptionalsToBuilder(const std::vector<std::optional<T>>& in, B& builder)
{
    std::vector<T> values(in.size());
    std::vector<bool> valid(in.size());

    std::ranges::transform(in, values.begin(), [](auto& e) { return e.value_or(T{}); });
    std::transform(in.begin(), in.end(), valid.begin(), [](auto& e) { return e.has_value(); });

    throwOnStatusKO(builder.AppendValues(values, valid));
}

std::vector<std::optional<unsigned>> to_optional_int(
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
    throwOnStatusKO(builder.AppendValues(column_->data()));
    return throwOnResultKO(builder.Finish());
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
    throwOnStatusKO(builder.AppendValues(column_->data()));
    return throwOnResultKO(builder.Finish());
}

// ============================
// Class IntColumnAdapter
// ============================
IntColumnAdapter::IntColumnAdapter(const IntegralColumn* column):
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
    throwOnStatusKO(builder.AppendValues(column_->data()));
    return throwOnResultKO(builder.Finish());
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
            throwOnStatusKO(builder.Append(*element));
        }
        else
        {
            throwOnStatusKO(builder.AppendNull());
        }
    }

    return throwOnResultKO(builder.Finish());
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
    addOptionalsToBuilder(column_->data(), builder);
    return throwOnResultKO(builder.Finish());
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
    addOptionalsToBuilder(column_->data(), builder);
    return throwOnResultKO(builder.Finish());
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
    addOptionalsToBuilder(to_optional_int(column_->data()), builder);
    return throwOnResultKO(builder.Finish());
}

// ===============================
// Column adapter factory
// ===============================
class ColumnAdapterFactory: public IColumnAdapterVisitor
{
public:
    std::shared_ptr<IColumnAdapter> visit(const StringColumn& col) override
    {
        return std::make_shared<StringColumnAdapter>(&col);
    }

    std::shared_ptr<IColumnAdapter> visit(const DoubleColumn& col) override
    {
        return std::make_shared<DoubleColumnAdapter>(&col);
    }

    std::shared_ptr<IColumnAdapter> visit(const IntegralColumn& col) override
    {
        return std::make_shared<IntColumnAdapter>(&col);
    }

    std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<std::string>& col) override
    {
        return std::make_shared<OptStringColumnAdapter>(&col);
    }

    std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<double>& col) override
    {
        return std::make_shared<OptDoubleColumnAdapter>(&col);
    }

    std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<unsigned>& col) override
    {
        return std::make_shared<OptIntColumnAdapter>(&col);
    }

    std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<MipBasisStatus>& col) override
    {
        return std::make_shared<OptMipBasisStatusColumnAdapter>(&col);
    }
};

std::shared_ptr<IColumnAdapter> makeColumnAdapter(const std::unique_ptr<IColumn>& column)
{
    if (!column)
    {
        throw InvalidArgumentError("makeColumnAdapter: null column");
    }

    ColumnAdapterFactory factory;

    try
    {
        return column->accept(factory);
    }
    catch (const std::bad_cast& e)
    {
        // This shouldn't happen if all column types are properly registered
        std::string err_msg = "makeColumnAdapter: column type unknown: " + column->name()
                              + " (dynamic type: " + std::string(typeid(*column).name()) + ")";
        throw InvalidArgumentError(err_msg);
    }
}

} // namespace Antares::Writer
