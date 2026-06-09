// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include <antares/expressions/expression.h>

namespace Antares::ModelerStudy::SystemModel
{

/// An extra output expression
class ExtraOutput final
{
public:
    ExtraOutput(std::string id, Expression expression):
        id_(std::move(id)),
        expression_(std::move(expression))
    {
    }

    const std::string& Id() const
    {
        return id_;
    }

    const Expression& expression() const
    {
        return expression_;
    }

private:
    std::string id_;
    Expression expression_;
};

} // namespace Antares::ModelerStudy::SystemModel
