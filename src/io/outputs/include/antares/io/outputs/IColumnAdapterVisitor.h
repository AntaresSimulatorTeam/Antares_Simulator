// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/optimisation/linear-problem-api/hasStatus.h"

namespace Antares::Writer
{
class IColumnAdapter;
} // namespace Antares::Writer

using namespace Antares::Writer;

namespace Antares::IO::Outputs
{

// Forward declare template
template<typename T>
class TypedColumn;

// Define type aliases
using StringColumn = TypedColumn<std::string>;
using IntegralColumn = TypedColumn<unsigned>;
using DoubleColumn = TypedColumn<double>;

template<typename T>
using OptionalColumn = TypedColumn<std::optional<T>>;

// ================================
// Class IColumnAdapterVisitor
// ================================
class IColumnAdapterVisitor
{
public:
    virtual ~IColumnAdapterVisitor() = default;
    virtual std::shared_ptr<IColumnAdapter> visit(const StringColumn&) = 0;
    virtual std::shared_ptr<IColumnAdapter> visit(const DoubleColumn&) = 0;
    virtual std::shared_ptr<IColumnAdapter> visit(const IntegralColumn&) = 0;
    virtual std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<std::string>&) = 0;
    virtual std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<double>&) = 0;
    virtual std::shared_ptr<IColumnAdapter> visit(const OptionalColumn<unsigned>&) = 0;

    virtual std::shared_ptr<IColumnAdapter>
    visit(const OptionalColumn<Optimisation::LinearProblemApi::MipBasisStatus>&) = 0;
};
} // namespace Antares::IO::Outputs
