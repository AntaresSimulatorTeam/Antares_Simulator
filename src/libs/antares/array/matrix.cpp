// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

#include <antares/array/matrix.h>

namespace Antares
{
Matrix::Matrix(unsigned int w, unsigned int h)
{
    resize(w, h);
}

void Matrix::zero()
{
    for (auto& column: columns_)
    {
        std::fill(column.begin(), column.end(), 0.);
    }
}

void Matrix::averageTimeseries(bool roundValues)
{
    if (width_ > 1)
    {
        auto& first = columns_[0];

        // Add the values of each time series to the first one.
        for (unsigned int i = 1; i != width_; ++i)
        {
            auto& column = columns_[i];
            for (unsigned int j = 0; j != height_; ++j)
            {
                first[j] += column[j];
            }
        }

        const double coeff = 1. / width_;
        if (roundValues)
        {
            for (unsigned int j = 0; j != height_; ++j)
            {
                first[j] = std::round(first[j] * coeff);
            }
        }
        else
        {
            for (unsigned int j = 0; j != height_; ++j)
            {
                first[j] *= coeff;
            }
        }

        for (unsigned int i = 1; i != width_; ++i)
        {
            columns_[i].clear();
        }
        columns_.resize(1);
        width_ = 1;
    }
}

void Matrix::fill(double v)
{
    for (auto& column: columns_)
    {
        std::fill(column.begin(), column.end(), v);
    }
}

void Matrix::fillUnit()
{
    zero();

    const auto diagonalSize = std::min(width_, height_);
    for (unsigned int i = 0; i != diagonalSize; ++i)
    {
        columns_[i][i] = 1.;
    }
}

void Matrix::reset(unsigned int w, unsigned int h)
{
    if (w > kMaxDimension || h > kMaxDimension)
    {
        throw std::length_error("Matrix dimensions exceed the supported limit");
    }
    ColumnMatrix<double>::reset(w, h);
}

void Matrix::pasteToColumn(unsigned int x, ConstColumnType data)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    if (data.size() != height_)
    {
        throw std::invalid_argument("Column height does not match matrix");
    }
    std::copy(data.begin(), data.end(), columns_[x].begin());
}

void Matrix::fillColumn(unsigned int x, double value)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    std::fill(columns_[x].begin(), columns_[x].end(), value);
}

void Matrix::columnToZero(unsigned int x)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    std::fill(columns_[x].begin(), columns_[x].end(), 0.);
}

bool Matrix::empty() const
{
    return (!width_) or (!height_);
}

void Matrix::clear()
{
    ColumnMatrix<double>::clear();
}

void Matrix::reset()
{
    clear();
}

void Matrix::resize(unsigned int w, unsigned int h)
{
    if (w > kMaxDimension || h > kMaxDimension)
    {
        throw std::length_error("Matrix dimensions exceed the supported limit");
    }

    ColumnMatrix<double>::resize(w, h);
}

void Matrix::resizeWithoutDataLost(unsigned int x, unsigned int y, double defVal)
{
    if (x > kMaxDimension || y > kMaxDimension)
    {
        throw std::length_error("Matrix dimensions exceed the supported limit");
    }

    ColumnMatrix<double>::resizeWithoutDataLost(x, y, defVal);
}

void Matrix::multiplyAllEntriesBy(double c)
{
    for (auto& column: columns_)
    {
        for (auto& value: column)
        {
            value *= c;
        }
    }
}

void Matrix::roundAllEntries()
{
    for (auto& column: columns_)
    {
        for (auto& value: column)
        {
            value = std::round(value);
        }
    }
}

bool Matrix::containsOnlyZero() const
{
    return containsOnlyZero(std::identity{});
}

void Matrix::copyFrom(const Matrix& rhs)
{
    *this = rhs;
}

void Matrix::copyFrom(const Matrix* rhs)
{
    if (rhs)
    {
        copyFrom(*rhs);
    }
}

void Matrix::swap(Matrix& rhs) noexcept
{
    ColumnMatrix<double>::swap(rhs);
}

bool MatrixTestForAtLeastOnePositiveValue(const Matrix& m)
{
    for (unsigned int x = 0; x < m.width(); ++x)
    {
        const auto column = m[x];
        for (unsigned int y = 0; y < m.height(); ++y)
        {
            if (column[y] > 0.)
            {
                return true;
            }
        }
    }
    return false;
}

} // namespace Antares
