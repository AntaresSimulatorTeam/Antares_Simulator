// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>

#include "antares/io/outputs/columns.h"

namespace arrow
{
class Field;
class Array;
} // namespace arrow

namespace Antares::Writer
{

// ===========================
// Class IColumnAdapter
// ===========================
class IColumnAdapter
{
public:
    virtual ~IColumnAdapter() = default;
    virtual std::shared_ptr<arrow::Field> makeField() const = 0;
    virtual std::shared_ptr<arrow::Array> makeArray() const = 0;
};

// ================================
// Class StringColumnAdapter
// ================================
class StringColumnAdapter: public IColumnAdapter
{
public:
    explicit StringColumnAdapter(const IO::Outputs::StringColumn* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::StringColumn* column_;
};

// ================================
// Class DoubleColumnAdapter
// ================================
class DoubleColumnAdapter: public IColumnAdapter
{
public:
    explicit DoubleColumnAdapter(const IO::Outputs::DoubleColumn* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::DoubleColumn* column_;
};

// ================================
// Class IntColumnAdapter
// ================================
class IntColumnAdapter: public IColumnAdapter
{
public:
    explicit IntColumnAdapter(const IO::Outputs::IntegralColumn* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::IntegralColumn* column_;
};

// ================================
// Class OptStringColumnAdapter
// ================================
class OptStringColumnAdapter: public IColumnAdapter
{
public:
    explicit OptStringColumnAdapter(const IO::Outputs::OptionalColumn<std::string>* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::OptionalColumn<std::string>* column_;
};

// ================================
// Class OptDoubleColumnAdapter
// ================================
class OptDoubleColumnAdapter: public IColumnAdapter
{
public:
    explicit OptDoubleColumnAdapter(const IO::Outputs::OptionalColumn<double>* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::OptionalColumn<double>* column_;
};

// ================================
// Class OptIntColumnAdapter
// ================================
class OptIntColumnAdapter: public IColumnAdapter
{
public:
    explicit OptIntColumnAdapter(const IO::Outputs::OptionalColumn<unsigned>* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::OptionalColumn<unsigned>* column_;
};

// ================================
// Class OptMipBasisStatusColumnAdapter
// ================================
class OptMipBasisStatusColumnAdapter: public IColumnAdapter
{
public:
    explicit OptMipBasisStatusColumnAdapter(
      const IO::Outputs::OptionalColumn<Optimisation::LinearProblemApi::MipBasisStatus>* column);
    std::shared_ptr<arrow::Field> makeField() const override;
    std::shared_ptr<arrow::Array> makeArray() const override;

private:
    const IO::Outputs::OptionalColumn<Optimisation::LinearProblemApi::MipBasisStatus>* column_;
};

// ===========================
// Column adapter factory
// ===========================
std::shared_ptr<IColumnAdapter> makeColumnAdapter(
  const std::unique_ptr<IO::Outputs::IColumn>& column);

} // namespace Antares::Writer
