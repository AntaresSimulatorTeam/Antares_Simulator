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
#include <Eigen/Dense>
#include <Eigen/Sparse>
#pragma once

class LinearExpressionEigen
{
public:
    LinearExpressionEigen(int nTimesteps, int nVars);
    LinearExpressionEigen(const Eigen::SparseMatrix<double>& coeffs,
                          const Eigen::VectorXd& offsets);

    LinearExpressionEigen operator+(const LinearExpressionEigen& b) const;

    LinearExpressionEigen& operator+=(const LinearExpressionEigen& b);

    // --- Soustraction ---
    LinearExpressionEigen operator-(const LinearExpressionEigen& b) const;

    LinearExpressionEigen& operator-=(const LinearExpressionEigen& b);

    // --- Négation ---
    LinearExpressionEigen operator-() const;

    // --- Multiplication ---
    LinearExpressionEigen operator*(const LinearExpressionEigen& b) const;
    LinearExpressionEigen& operator*=(const LinearExpressionEigen& b);

    // --- Division ---
    LinearExpressionEigen operator/(const LinearExpressionEigen& b) const;
    LinearExpressionEigen& operator/=(const LinearExpressionEigen& b);
    void addCoeff(size_t t, size_t col, double value);
    void addVectorCoeff(size_t col, const std::vector<double>& values);
    void addVectorCoeff(size_t col, double value);
    void addOffset(size_t t, double value);
    void addVectorOffset(const std::vector<double>& values);
    void addVectorOffset(double value);

    // --- Getters ---
    const Eigen::VectorXd& offset() const
    {
        return offsets_;
    }

    void setOffset(const Eigen::VectorXd& offsets)
    {
        offsets_ = offsets;
    }

    const Eigen::SparseMatrix<double>& coefPerVar() const
    {
        return coeffs_;
    }

    void setCol(int colIndex, const Eigen::VectorXd& col);
    void setRow(int rowIndex, const Eigen::VectorXd& row);

private:
    Eigen::SparseMatrix<double, Eigen::RowMajor> coeffs_; // [nTimesteps × nVars]
    Eigen::VectorXd offsets_; // [nTimesteps]
};
