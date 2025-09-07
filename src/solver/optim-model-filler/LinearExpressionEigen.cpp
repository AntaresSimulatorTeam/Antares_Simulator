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
// TODO just to print matrix
#include <iostream>
#include <span>

#include <antares/solver/optim-model-filler/LinearExpressionEigen.h>

LinearExpressionEigen::LinearExpressionEigen(int nRows, int nCols):
    coeffs_(nRows, nCols),
    offsets_(nRows)
{
    // TODO
    offsets_.setZero();
}

void LinearExpressionEigen::print() const
{
    std::cout << coeffs_ << std::endl;
}

LinearExpressionEigen::LinearExpressionEigen(
  const Eigen::SparseMatrix<double, Eigen::RowMajor>& coeffs,
  const Eigen::VectorXd& offsets)
{
    if (coeffs.rows() != offsets.size())
    {
        throw std::invalid_argument(
          "Coefficient matrix and offset vector must have the same number of rows");
    }
    coeffs_ = coeffs;
    offsets_ = offsets;
}

LinearExpressionEigen LinearExpressionEigen::operator+(const LinearExpressionEigen& b) const
{
    LinearExpressionEigen out = *this;
    out += b;
    return out;
}

void LinearExpressionEigen::CheckLinearExpressionSize(const LinearExpressionEigen& a,
                                                      const LinearExpressionEigen& b)
{
    if (a.coeffs_.rows() != b.coeffs_.rows() || a.coeffs_.cols() != b.coeffs_.cols())
    {
        throw std::invalid_argument("Dimension mismatch");
    }
    if (a.offsets_.size() != b.offsets_.size())
    {
        throw std::invalid_argument("Offset dimension mismatch");
    }
}

LinearExpressionEigen& LinearExpressionEigen::operator+=(const LinearExpressionEigen& b)
{
    CheckLinearExpressionSize(*this, b);
    coeffs_ += b.coeffs_;
    offsets_ += b.offsets_;
    return *this;
}

LinearExpressionEigen LinearExpressionEigen::operator-(const LinearExpressionEigen& b) const
{
    LinearExpressionEigen out = *this;
    out -= b;
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator-=(const LinearExpressionEigen& b)
{
    CheckLinearExpressionSize(*this, b);
    coeffs_ -= b.coeffs_;
    offsets_ -= b.offsets_;
    return *this;
}

LinearExpressionEigen LinearExpressionEigen::operator-() const
{
    LinearExpressionEigen out(coeffs_.rows(), coeffs_.cols());
    out.coeffs_ = -coeffs_;
    out.offsets_ = -offsets_;
    return out;
}

LinearExpressionEigen LinearExpressionEigen::operator*(const LinearExpressionEigen& b) const
{
    auto out = *this;
    out *= b;
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator*=(const LinearExpressionEigen& b)
{
    bool aHasVars = (coeffs_.nonZeros() > 0);
    bool bHasVars = (b.coeffs_.nonZeros() > 0);

    if (aHasVars && bHasVars)
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
    if (offsets_.size() != b.offsets_.size())
    {
        throw std::invalid_argument("Offset dimension mismatch in operator*=");
    }

    if (!aHasVars)
    {
        coeffs_ = b.coeffs_;
        for (int k = 0; k < coeffs_.outerSize(); ++k)
        {
            for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(coeffs_, k); it;
                 ++it)
            {
                it.valueRef() *= offsets_[it.row()];
            }
        }
    }
    else if (!bHasVars)
    {
        for (int k = 0; k < coeffs_.outerSize(); ++k)
        {
            for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(coeffs_, k); it;
                 ++it)
            {
                it.valueRef() *= b.offsets_[it.row()];
            }
        }
    }

    offsets_ = offsets_.cwiseProduct(b.offsets_);

    return *this;
}

LinearExpressionEigen LinearExpressionEigen::operator/(const LinearExpressionEigen& b) const
{
    auto out = *this;
    out /= b;
    return out;
}

LinearExpressionEigen& LinearExpressionEigen::operator/=(const LinearExpressionEigen& b)
{
    if (b.coeffs_.nonZeros() > 0)
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }

    if (offsets_.size() != b.offsets_.size())
    {
        throw std::invalid_argument("Offset dimension mismatch in operator*=");
    }

    for (int k = 0; k < coeffs_.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(coeffs_, k); it; ++it)
        {
            it.valueRef() /= b.offsets_[it.row()];
        }
    }

    offsets_ = offsets_.cwiseQuotient(b.offsets_);
    return *this;
}

void LinearExpressionEigen::setCoeff(size_t row, size_t col, double value)
{
    if (row >= static_cast<size_t>(coeffs_.rows()) || col >= static_cast<size_t>(coeffs_.cols()))
    {
        throw std::out_of_range("Index out of range in setCoeff");
    }
    if (std::abs(value) > 1e-16) // TODO
    {
        coeffs_.coeffRef(row, col) = value;
    }
}

// Ajoute un vecteur de coefficients temps-dépendant
void LinearExpressionEigen::setCol(size_t colIndex, const std::vector<double>& values)
{
    if (colIndex >= static_cast<size_t>(coeffs_.cols()))
    {
        throw std::out_of_range("Column index out of range");
    }
    if (values.size() != static_cast<size_t>(coeffs_.rows()))
    {
        throw std::invalid_argument("Vector size must match number of rows");
    }
    for (auto t = 0; t < values.size(); ++t)
    {
        setCoeff(t, colIndex, values[t]);
    }
}

void LinearExpressionEigen::setCol(size_t col, double value)
{
    for (size_t t = 0; t < coeffs_.col(col).size(); ++t)
    {
        setCoeff(t, col, value);
    }
}

void LinearExpressionEigen::setOffset(size_t row, double value)
{
    if (row >= static_cast<size_t>(coeffs_.rows()) || row >= static_cast<size_t>(coeffs_.cols()))
    {
        throw std::out_of_range("Index out of range in setOffset");
    }
    if (std::abs(value) < 1e-16) // TODO
    {
        offsets_(row) = value;
    }
}

void LinearExpressionEigen::setOffset(const std::vector<double>& values)
{
    for (size_t t = 0; t < values.size(); ++t)
    {
        setOffset(t, values[t]);
    }
}

void LinearExpressionEigen::setOffset(double value)
{
    // TODO add in one shot
    for (size_t t = 0; t < offsets_.size(); ++t)
    {
        setOffset(t, value);
    }
}

/*
 *
 * the setCol method with a sparse vector, it's not as straightforward because:
 * For row-major matrices, column operations are less efficient
 * coeffs_.col(colIndex) doesn't return a sparse vector that can be directly assigned
 *
 */

void LinearExpressionEigen::setCol(int colIndex, const Eigen::SparseVector<double>& col)
{
    if (colIndex < 0 || colIndex >= coeffs_.cols())
    {
        throw std::out_of_range("Column index out of range");
    }
    if (col.size() != coeffs_.rows())
    {
        throw std::invalid_argument("Column size mismatch in setCol()");
    }
    // Clear the column
    for (int k = 0; k < coeffs_.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(coeffs_, k); it; ++it)
        {
            if (it.col() == colIndex)
            {
                it.valueRef() = 0.0;
            }
        }
    }
    // Set non-zero values
    for (Eigen::SparseVector<double>::InnerIterator it(col); it; ++it)
    {
        coeffs_.coeffRef(it.index(), colIndex) = it.value();
    }
}

void LinearExpressionEigen::setRow(int rowIndex, const Eigen::SparseVector<double>& row)
{
    if (rowIndex < 0 || rowIndex >= coeffs_.rows())
    {
        throw std::out_of_range("Row index out of range");
    }
    if (row.size() != coeffs_.cols())
    {
        throw std::invalid_argument("Row size mismatch in setRow()");
    }

    coeffs_.row(rowIndex) = row;
}
