/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#pragma once
#include <span>

#include <antares/expressions/visitors/HelpVisitNode.h>

#include "LinearExpression.h"

namespace Antares::Optimization
{
class TimeDependentLinearExpression final
{
public:
    explicit TimeDependentLinearExpression(std::size_t nbTimesteps);
    explicit TimeDependentLinearExpression(const std::span<const double>& values);

    // Constant expression
    explicit TimeDependentLinearExpression(LinearExpression&& expr);
    explicit TimeDependentLinearExpression(const std::vector<std::pair<int, double>>& coefs,
                                           double constant);

    std::vector<double> constant() const;

    void mergeDuplicateCoefficients();

    std::size_t size() const;

    bool isConstant() const;

    using iterator = std::vector<LinearExpression>::iterator;
    iterator begin();
    iterator end();

    using const_iterator = std::vector<LinearExpression>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;

    LinearExpression& operator[](std::size_t idx);
    const LinearExpression& operator[](std::size_t idx) const;

    TimeDependentLinearExpression& operator+=(const TimeDependentLinearExpression& other);
    TimeDependentLinearExpression& operator-=(const TimeDependentLinearExpression& other);

    void rotate(int shift);

    TimeDependentLinearExpression& operator*=(double factor);
    TimeDependentLinearExpression& operator*=(const TimeDependentLinearExpression& other);
    TimeDependentLinearExpression operator-() const;
    TimeDependentLinearExpression operator/(const TimeDependentLinearExpression& other) const;

private:
    void expandTo(std::size_t nbTimesteps);

    std::vector<LinearExpression> v_;
};

// gp : Multiple applyOperation on vectors. Avoid code duplication ?
// gp : Note : getMaxSize is used inside each occurrences.
// gp : So if there is code duplication on applyOperation, it should be defined
// gp : at the same location.
template<class Op>
TimeDependentLinearExpression applyOperation(const std::vector<TimeDependentLinearExpression>& lhs,
                                             Op op)
{
    const auto maxSize = Expressions::Visitors::getMaxSize(lhs);
    std::vector<const LinearExpression*> row(lhs.size());
    std::vector<double> constants(maxSize);

    for (std::size_t t = 0; t < maxSize; ++t)
    {
        for (int c = 0; c < lhs.size(); ++c)
        {
            const auto& e = lhs[c];
            row[c] = &(e.size() < maxSize ? e[0] : e[t]);
        }
        constants[t] = applyOperation(row, op);
    }

    return TimeDependentLinearExpression(constants);
}

} // namespace Antares::Optimization
