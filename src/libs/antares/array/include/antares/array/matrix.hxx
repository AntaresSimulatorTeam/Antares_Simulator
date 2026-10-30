// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_HXX__

#include <algorithm>
#include <cmath>
#include <utility>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

namespace Antares
{
inline Matrix::Matrix() = default;

inline Matrix::Matrix(const Matrix&) = default;

inline Matrix::Matrix(unsigned int w, unsigned int h):
    width_(0),
    height_(0)
{
    resize(w, h);
}

inline unsigned int Matrix::width() const noexcept
{
    return width_;
}

inline unsigned int Matrix::height() const noexcept
{
    return height_;
}

inline Matrix::ColumnType& Matrix::mutableColumn(unsigned int column) const
{
    assert(column < width_);
    return columns_[column];
}

inline Matrix::Matrix(Matrix&& rhs) noexcept:
    width_(rhs.width_),
    height_(rhs.height_),
    columns_(std::move(rhs.columns_))
{
    rhs.width_ = 0;
    rhs.height_ = 0;
}

inline void Matrix::zero()
{
    for (auto& column: columns_)
    {
        std::fill(column.begin(), column.end(), 0.);
    }
}

inline void Matrix::averageTimeseries(bool roundValues)
{
    if (width_ > 1)
    {
        ColumnType& first = columns_[0];

        // Add the values of each time series to the first one.
        for (unsigned int i = 1; i != width_; ++i)
        {
            ColumnType& column = columns_[i];
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

inline void Matrix::fill(double v)
{
    for (auto& column: columns_)
    {
        std::fill(column.begin(), column.end(), v);
    }
}

inline void Matrix::fillUnit()
{
    zero();

    const auto diagonalSize = std::min(width_, height_);
    for (unsigned int i = 0; i != diagonalSize; ++i)
    {
        columns_[i][i] = 1.;
    }
}

inline void Matrix::reset(unsigned int w, unsigned int h)
{
    const bool sameSize = w == width_ && h == height_;
    resize(w, h);
    if (sameSize)
    {
        zero();
    }
}

inline void Matrix::pasteToColumn(unsigned int x, const double* data)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    std::copy(data, data + height_, columns_[x].begin());
}

inline void Matrix::fillColumn(unsigned int x, double value)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    std::fill(columns_[x].begin(), columns_[x].end(), value);
}

inline void Matrix::columnToZero(unsigned int x)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    std::fill(columns_[x].begin(), columns_[x].end(), 0.);
}

inline bool Matrix::empty() const
{
    return (!width_) or (!height_);
}

inline void Matrix::clear()
{
    columns_.clear();
    width_ = 0;
    height_ = 0;
}

inline void Matrix::reset()
{
    clear();
}

inline void Matrix::resize(unsigned int w, unsigned int h)
{
    assert(w <= 50000 and "The new width_ seems a bit excessive");
    assert(h <= 50000 and "The new height_ seems a bit excessive");

    if (w != width_ or h != height_)
    {
        if (!w or !h)
        {
            clear();
        }
        else
        {
            width_ = w;
            height_ = h;
            columns_.clear();
            columns_.resize(w);
            for (auto& column: columns_)
            {
                column.resize(h);
            }
        }
    }
}

inline void Matrix::resizeWithoutDataLost(unsigned int x, unsigned int y, double defVal)
{
    if (!x or !y)
    {
        clear();
    }
    else if (x <= width_ and y <= height_)
    {
        columns_.resize(x);
        for (auto& column: columns_)
        {
            column.resize(y);
        }
        width_ = x;
        height_ = y;
    }
    else
    {
        const Matrix copy(*this);
        resize(x, y);
        const unsigned int minW = std::min(x, copy.width_);
        const unsigned int minH = std::min(y, copy.height_);

        for (unsigned int i = 0; i < minW; ++i)
        {
            ColumnType& column = columns_[i];
            std::copy_n(copy.columns_[i].begin(), minH, column.begin());
            std::fill(column.begin() + minH, column.end(), defVal);
        }

        for (unsigned int i = minW; i < x; ++i)
        {
            std::fill(columns_[i].begin(), columns_[i].end(), defVal);
        }
    }
    logs.debug() << "  :: end resizeWithoutDataLost";
}

inline void Matrix::multiplyAllEntriesBy(double c)
{
    if (columns_.empty())
    {
        return;
    }

    if (!Utils::isZero(c))
    {
        for (auto& column: columns_)
        {
            for (auto& value: column)
            {
                value *= c;
            }
        }
    }
    else
    {
        zero();
    }
}

inline void Matrix::roundAllEntries()
{
    for (auto& column: columns_)
    {
        for (auto& value: column)
        {
            value = std::round(value);
        }
    }
}

template<class PredicateT>
bool Matrix::containsOnlyZero(const PredicateT& predicate) const
{
    for (const auto& column: columns_)
    {
        for (const auto& value: column)
        {
            if (!Utils::isZero(predicate(value)))
            {
                return false;
            }
        }
    }
    return true;
}

inline void Matrix::copyFrom(const Matrix& rhs)
{
    assert(&rhs != this and "Undefined behavior");

    if (rhs.empty())
    {
        clear();
    }
    else
    {
        resize(rhs.width(), rhs.height());
        for (unsigned int x = 0; x != rhs.width(); ++x)
        {
            columns_[x] = rhs[x];
        }
    }
}

inline void Matrix::copyFrom(const Matrix* rhs)
{
    if (rhs)
    {
        copyFrom(*rhs);
    }
}

inline void Matrix::swap(Matrix& rhs) noexcept
{
    using std::swap;
    swap(width_, rhs.width_);
    swap(height_, rhs.height_);
    swap(columns_, rhs.columns_);
}

inline Matrix& Matrix::operator=(const Matrix& rhs)
{
    copyFrom(rhs);
    return *this;
}

inline Matrix& Matrix::operator=(Matrix&& rhs) noexcept
{
    width_ = rhs.width_;
    height_ = rhs.height_;
    columns_ = std::move(rhs.columns_);
    rhs.width_ = 0;
    rhs.height_ = 0;
    return *this;
}

inline bool MatrixTestForAtLeastOnePositiveValue(const Matrix& m)
{
    for (unsigned int x = 0; x < m.width(); ++x)
    {
        const auto& column = m[x];
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

inline const Matrix::ColumnType& Matrix::operator[](unsigned int column) const
{
    assert(column < width_);
    return columns_[column];
}

inline Matrix::ColumnType& Matrix::operator[](unsigned int column)
{
    assert(column < width_);
    return columns_[column];
}

inline const Matrix::ColumnType& Matrix::column(unsigned int n) const
{
    assert(n < width_);
    return columns_[n];
}

inline Matrix::ColumnType& Matrix::column(unsigned int n)
{
    assert(n < width_);
    return columns_[n];
}
} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_MATRIX_HXX__
