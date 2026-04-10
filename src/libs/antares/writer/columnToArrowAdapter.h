// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/io/api.h>

#include "antares/io/outputs/columns.h"

namespace Antares::Writer
{

// ===========================
// Class IColumnAdapter
// ===========================
class IColumnAdapter
{
public:
    virtual std::shared_ptr<arrow::Field> makeField() const = 0;
    virtual std::shared_ptr<arrow::Array> makeArray() const = 0;
};

// ================================
// Class StringColumnAdapter
// ================================
class StringColumnAdapter: public IColumnAdapter
{
public:
    StringColumnAdapter(const IO::Outputs::StringColumn* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::StringColumn* column_;
};

// ================================
// Class OptStringColumnAdapter
// ================================
class OptStringColumnAdapter: public IColumnAdapter
{
public:
    OptStringColumnAdapter(const IO::Outputs::OptionalColumn<std::string>* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::OptionalColumn<std::string>* column_;
};

// ===========================
// Column adapter factory
// ===========================
std::shared_ptr<IColumnAdapter> makeColumnAdapter(
  const std::unique_ptr<IO::Outputs::IColumn>& column);

} // namespace Antares::Writer
