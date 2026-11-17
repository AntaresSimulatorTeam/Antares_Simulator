// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

#include <antares/array/matrix.h>

namespace Antares
{
Matrix::Matrix() = default;

Matrix::Matrix(const Matrix&) = default;

Matrix::Matrix(unsigned int w, unsigned int h)
{
    resize(w, h);
}

Matrix::Matrix(Matrix&& rhs) noexcept:
    ColumnMatrix<double>(std::move(rhs))
{
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
    const bool sameSize = w == width_ && h == height_;
    resize(w, h);
    if (sameSize)
    {
        zero();
    }
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
    columns_.clear();
    width_ = 0;
    height_ = 0;
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

    if (w != width_ or h != height_)
    {
        if (!w or !h)
        {
            clear();
        }
        else
        {
            std::vector<std::vector<double>> newColumns(w);
            for (auto& column: newColumns)
            {
                column.resize(h);
            }
            columns_.swap(newColumns);
            width_ = w;
            height_ = h;
        }
    }
}

void Matrix::resizeWithoutDataLost(unsigned int x, unsigned int y, double defVal)
{
    if (x > kMaxDimension || y > kMaxDimension)
    {
        throw std::length_error("Matrix dimensions exceed the supported limit");
    }

    if (!x or !y)
    {
        clear();
    }
    else if (x <= width_ and y <= height_)
    {
        if (x == width_)
        {
            for (unsigned int i = 0; i < x; ++i)
            {
                columns_[i].resize(y);
            }
        }
        else
        {
            std::vector<std::vector<double>> newColumns(columns_.begin(), columns_.begin() + x);
            for (auto& column: newColumns)
            {
                column.resize(y);
            }
            columns_.swap(newColumns);
        }
        width_ = x;
        height_ = y;
    }
    else
    {
        const unsigned int preservedWidth = std::min(x, width_);
        const unsigned int preservedHeight = std::min(y, height_);
        std::vector<std::vector<double>> newColumns(x);

        for (unsigned int i = 0; i < x; ++i)
        {
            auto& column = newColumns[i];
            column.assign(y, defVal);
            if (i < preservedWidth)
            {
                std::copy_n(columns_[i].begin(), preservedHeight, column.begin());
            }
        }

        columns_.swap(newColumns);
        width_ = x;
        height_ = y;
    }
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
    if (&rhs == this)
    {
        return;
    }

    if (rhs.empty())
    {
        clear();
    }
    else
    {
        Matrix copy(rhs);
        swap(copy);
    }
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
    using std::swap;
    swap(width_, rhs.width_);
    swap(height_, rhs.height_);
    swap(columns_, rhs.columns_);
}

Matrix& Matrix::operator=(const Matrix& rhs)
{
    copyFrom(rhs);
    return *this;
}

Matrix& Matrix::operator=(Matrix&& rhs) noexcept
{
    if (this == &rhs)
    {
        return *this;
    }
    width_ = rhs.width_;
    height_ = rhs.height_;
    columns_ = std::move(rhs.columns_);
    rhs.width_ = 0;
    rhs.height_ = 0;
    return *this;
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
