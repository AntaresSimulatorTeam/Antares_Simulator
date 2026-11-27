// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
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
    fill(0.);
}

void Matrix::averageTimeseries(bool roundValues)
{
    const unsigned int currentWidth = width();
    const unsigned int currentHeight = height();
    if (currentWidth > 1)
    {
        auto first = storage_[0];

        // Add the values of each time series to the first one.
        for (unsigned int i = 1; i != currentWidth; ++i)
        {
            auto column = storage_[i];
            for (unsigned int j = 0; j != currentHeight; ++j)
            {
                first[j] += column[j];
            }
        }

        const double coeff = 1. / currentWidth;
        if (roundValues)
        {
            for (unsigned int j = 0; j != currentHeight; ++j)
            {
                first[j] = std::round(first[j] * coeff);
            }
        }
        else
        {
            for (unsigned int j = 0; j != currentHeight; ++j)
            {
                first[j] *= coeff;
            }
        }

        storage_.shrinkWidth(1);
    }
}

void Matrix::fill(double v)
{
    for (unsigned int x = 0; x != width(); ++x)
    {
        auto column = storage_[x];
        std::fill(column.begin(), column.end(), v);
    }
}

void Matrix::fillUnit()
{
    zero();

    const auto diagonalSize = std::min(width(), height());
    for (unsigned int i = 0; i != diagonalSize; ++i)
    {
        storage_[i][i] = 1.;
    }
}

void Matrix::reset(unsigned int w, unsigned int h)
{
    if (w > kMaxDimension || h > kMaxDimension)
    {
        throw std::length_error("Matrix dimensions exceed the supported limit");
    }
    storage_.reset(w, h);
}

void Matrix::pasteToColumn(unsigned int x, ConstColumnType data)
{
    assert(x < width() and "Invalid column index (bigger than `this->width()`)");
    if (data.size() != height())
    {
        throw std::invalid_argument("Column height does not match matrix");
    }
    std::copy(data.begin(), data.end(), storage_[x].begin());
}

void Matrix::fillColumn(unsigned int x, double value)
{
    assert(x < width() and "Invalid column index (bigger than `this->width()`)");
    auto column = storage_[x];
    std::fill(column.begin(), column.end(), value);
}

void Matrix::columnToZero(unsigned int x)
{
    assert(x < width() and "Invalid column index (bigger than `this->width()`)");
    auto column = storage_[x];
    std::fill(column.begin(), column.end(), 0.);
}

bool Matrix::empty() const
{
    return (!width()) or (!height());
}

void Matrix::clear()
{
    storage_.clear();
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

    storage_.resize(w, h);
}

void Matrix::resizeWithoutDataLost(unsigned int x, unsigned int y, double defVal)
{
    if (x > kMaxDimension || y > kMaxDimension)
    {
        throw std::length_error("Matrix dimensions exceed the supported limit");
    }

    storage_.resizeWithoutDataLost(x, y, defVal);
}

void Matrix::multiplyAllEntriesBy(double c)
{
    for (unsigned int x = 0; x != width(); ++x)
    {
        for (auto& value: storage_[x])
        {
            value *= c;
        }
    }
}

void Matrix::roundAllEntries()
{
    for (unsigned int x = 0; x != width(); ++x)
    {
        for (auto& value: storage_[x])
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
    storage_.swap(rhs.storage_);
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
