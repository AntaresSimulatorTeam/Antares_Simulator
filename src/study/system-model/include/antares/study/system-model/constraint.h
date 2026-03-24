// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include <antares/expressions/expression.h>

#include "optimConfig.h"

namespace Antares::ModelerStudy::SystemModel
{

enum class OutOfBoundsProcessingMode
{
    CYCLIC,
    DROP
};

/// A constraint linking variables and parameters of a model together
class Constraint final
{
public:
    Constraint(
      std::string id,
      Expression expression,
      Solver::Config::Location location = Solver::Config::Location::SUBPROBLEMS,
      OutOfBoundsProcessingMode outOfBoundsProcessingMode = OutOfBoundsProcessingMode::CYCLIC,
      bool isBindingConstraint = false):
        id_(std::move(id)),
        expression_(std::move(expression)),
        isBindingConstraint_(isBindingConstraint),
        location_(location),
        outOfBoundsProcessingMode_(outOfBoundsProcessingMode)
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

    [[nodiscard]] Solver::Config::Location location() const
    {
        return location_;
    }

    [[nodiscard]] OutOfBoundsProcessingMode outOfBoundsProcessingMode() const
    {
        return outOfBoundsProcessingMode_;
    }

    [[nodiscard]] bool isBindingConstraint() const
    {
        return isBindingConstraint_;
    }

private:
    std::string id_;
    Expression expression_;
    bool isBindingConstraint_ = false;
    Solver::Config::Location location_ = Solver::Config::Location::SUBPROBLEMS;
    OutOfBoundsProcessingMode outOfBoundsProcessingMode_ = OutOfBoundsProcessingMode::CYCLIC;
};

} // namespace Antares::ModelerStudy::SystemModel
