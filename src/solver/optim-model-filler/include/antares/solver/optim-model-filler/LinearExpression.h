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

#include <utility>
#include <vector>

namespace Antares::Optimization
{
class LinearExpression final
{
public:
    LinearExpression();
    explicit LinearExpression(double constant);
    explicit LinearExpression(const std::vector<std::pair<int, double>>& coefs, double constant);

    void mergeDuplicateCoefficients();

    LinearExpression& operator*=(double factor);
    LinearExpression& operator+=(const LinearExpression& other);
    LinearExpression& operator-=(const LinearExpression& other);
    LinearExpression operator-() const;
    LinearExpression& operator*=(const LinearExpression& other);
    void addVariable(int index, double value);
    double constant() const;

    using const_iterator = std::vector<std::pair<int, double>>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;
    const std::pair<int, double>& operator[](std::size_t) const;
    std::size_t size() const;

private:
    bool hasCoefs() const;
    std::vector<std::pair<int, double>> coefs_;
    double constant_ = 0.;
};

} // namespace Antares::Optimization
