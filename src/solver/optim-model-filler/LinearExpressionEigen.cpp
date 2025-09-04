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
#include <antares/solver/optim-model-filler/LinearExpressionEigen.h>

LinearExpressionEigen::LinearExpressionEigen(int nTimesteps, int nVars):
    coeffs_(Eigen::MatrixXd::Zero(nTimesteps, nVars)),
    offsets_(Eigen::VectorXd::Zero(nTimesteps))
{
}

LinearExpressionEigen LinearExpressionEigen::operator+(const LinearExpressionEigen& b) const
{
    LinearExpressionEigen out = *this;
    out += b;
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator+=(const LinearExpressionEigen& b)
{
    coeffs_ += b.coeffs_;
    offsets_ += b.offsets_;
    return a;
}

LinearExpressionEigen LinearExpressionEigen::operator-(const LinearExpressionEigen& b) const
{
    LinearExpressionEigen out = *this;
    out -= b;
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator-=(const LinearExpressionEigen& b)
{
    coeffs_ -= b.coeffs_;
    offsets_ -= b.offsets_;
    return a;
}

LinearExpressionEigen LinearExpressionEigen::operator-() const
{
    LinearExpressionEigen out(offsets_.size(), coeffs_.cols());
    out.coeffs_ = -coeffs_;
    out.offsets_ = -offsets_;
    return out;
}

LinearExpressionEigen LinearExpressionEigen::operator*(const LinearExpressionEigen& b) const
{
    bool aHasVars = (coeffs_.cwiseAbs().maxCoeff() > 1e-12);
    bool bHasVars = (b.coeffs_.cwiseAbs().maxCoeff() > 1e-12);
    if (aHasVars && bHasVars)
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
    LinearExpressionEigen out(offsets_.size(), coeffs_.cols());
    if (!aHasVars)
    {
        out.coeffs_ = offsets_.asDiagonal() * b.coeffs_;
        out.offsets_ = offsets_.cwiseProduct(b.offsets_);
    }
    else if (!bHasVars)
    {
        out.coeffs_ = coeffs_.array().colwise() * b.offsets_.array();
        out.offsets_ = offsets_.cwiseProduct(b.offsets_);
    }
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator*=(const LinearExpressionEigen& b)
{
    bool aHasVars = (coeffs_.cwiseAbs().maxCoeff() > 1e-12);
    bool bHasVars = (b.coeffs_.cwiseAbs().maxCoeff() > 1e-12);

    if (aHasVars && bHasVars)
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }

    if (!aHasVars)
    {
        // a is parameter -> scale b
        // coeffs_ = offsets_ * b.coeffs_  (ligne par ligne)
        coeffs_ = offsets_.asDiagonal() * b.coeffs_;
        offsets_ = offsets_.cwiseProduct(b.offsets_);
    }
    else if (!bHasVars)
    {
        // b is parameter -> scale a
        coeffs_ = coeffs_.array().colwise() * b.offsets_.array();
        offsets_ = offsets_.cwiseProduct(b.offsets_);
    }

    return *this;
}

LinearExpressionEigen LinearExpressionEigen::operator/(const LinearExpressionEigen& b) const
{
    if (b.coeffs_.cwiseAbs().maxCoeff() > 1e-12)
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }
    LinearExpressionEigen out(offsets_.size(), coeffs_.cols());
    out.coeffs_ = coeffs_.array().colwise() / b.offsets_.array();
    out.offsets_ = offsets_.array() / b.offsets_.array();
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator/=(const LinearExpressionEigen& b)
{
    if (b.coeffs_.cwiseAbs().maxCoeff() > 1e-12)
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }
    coeffs_ = coeffs_.array().colwise() / b.offsets_.array();
    offsets_ = offsets_.array() / b.offsets_.array();
    return *this;
}

void LinearExpressionEigen::addCoeff(size_t t, size_t col, double value)
{
    coeffs_(t, col) = value;
}

// Ajoute un vecteur de coefficients temps-dépendant
void LinearExpressionEigen::addVectorCoeff(size_t col, const std::vector<double>& values)
{
    // TODO add in one shot
    for (size_t t = 0; t < values.size(); ++t)
    {
        addCoeff(t, col, values[t]);
    }
}

void LinearExpressionEigen::addVectorCoeff(size_t col, double value)
{
    // TODO add in one shot
    for (size_t t = 0; t < coeffs_.col(col).size(); ++t)
    {
        addCoeff(t, col, value);
    }
}

void LinearExpressionEigen::addOffset(size_t t, double value)
{
    offsets_(t) = value;
}

void LinearExpressionEigen::addVectorOffset(const std::vector<double>& values)
{
    // TODO add in one shot
    for (size_t t = 0; t < values.size(); ++t)
    {
        addOffset(t, values[t]);
    }
}

void LinearExpressionEigen::addVectorOffset(double value)
{
    // TODO add in one shot
    for (size_t t = 0; t < offsets_.size(); ++t)
    {
        addOffset(t, value);
    }
}

void LinearExpressionEigen::setCol(int colIndex, const Eigen::VectorXd& col)
{
    if (col.size() != coeffs_.rows())
    {
        throw std::invalid_argument("Column size mismatch in setCol()");
    }
    coeffs_.col(colIndex) = col;
}

void LinearExpressionEigen::setRow(int rowIndex, const Eigen::VectorXd& row)
{
    if (row.size() != coeffs_.cols())
    {
        throw std::invalid_argument("Row size mismatch in setRow()");
    }
    coeffs_.row(rowIndex) = row;
}
