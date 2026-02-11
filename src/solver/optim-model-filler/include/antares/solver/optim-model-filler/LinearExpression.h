// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>
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
    LinearExpression(const LinearExpression& other) = default;
    void mergeDuplicateCoefficients();

    LinearExpression& operator*=(double factor);
    LinearExpression& operator+=(const LinearExpression& other);
    LinearExpression& operator-=(const LinearExpression& other);
    LinearExpression operator-() const;
    LinearExpression operator/(const LinearExpression& other) const;
    LinearExpression& operator*=(const LinearExpression& other);
    LinearExpression& operator^=(const LinearExpression& other);

    void addVariable(int index, double value);
    void constant(double c);
    double constant() const;

    using const_iterator = std::vector<std::pair<int, double>>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;
    const std::pair<int, double>& operator[](std::size_t) const;
    std::size_t size() const;

    bool hasCoefs() const;

private:
    std::vector<std::pair<int, double>> coefs_;
    double constant_ = 0.;
};

template<class Op>
double applyOperation(const std::vector<const LinearExpression*>& expressions, Op op)
{
    std::vector<double> constants(expressions.size(), 0);
    for (std::size_t i = 0; i < expressions.size(); ++i)
    {
        const auto& expression = expressions[i];
        if (expression->hasCoefs())
        {
            throw std::invalid_argument(std::string("Operator '") + typeid(op).name()
                                        + "' cannot be applied to expressions with "
                                          "coefficients (non-constant expressions).");
        }
        constants[i] = expression->constant();
    }

    return op(constants);
}

} // namespace Antares::Optimization
