// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_HXX__

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <utility>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

namespace Antares
{
template<class T>
Matrix<T>::Matrix() = default;

template<class T>
Matrix<T>::Matrix(const Matrix<T>&) = default;

template<class T>
Matrix<T>::Matrix(unsigned int w, unsigned int h):
    width_(0),
    height_(0)
{
    resize(w, h);
}

template<class T>
unsigned int Matrix<T>::width() const noexcept
{
    return width_;
}

template<class T>
unsigned int Matrix<T>::height() const noexcept
{
    return height_;
}

template<class T>
typename Matrix<T>::ColumnType& Matrix<T>::mutableColumn(unsigned int column) const
{
    assert(column < width_);
    return columns_[column];
}

template<class T>
Matrix<T>::Matrix(Matrix<T>&& rhs) noexcept:
    width_(rhs.width_),
    height_(rhs.height_),
    columns_(std::move(rhs.columns_))
{
    rhs.width_ = 0;
    rhs.height_ = 0;
}

template<class T>
template<class U>
Matrix<T>::Matrix(const Matrix<U>& rhs):
    width_(0),
    height_(0),
    columns_()
{
    copyFrom(rhs);
}

template<class T>
inline void Matrix<T>::zero()
{
    for (unsigned int i = 0; i != width_; ++i)
    {
        ColumnType& column = columns_[i];
        std::fill(column.begin(), column.end(), T{});
    }
}

template<class T>
void Matrix<T>::averageTimeseries(bool roundValues)
{
    if (width_ > 1)
    {
        ColumnType& first = columns_[0];

        // add the values of each timeseries to the first one
        for (unsigned int i = 1; i != width_; ++i)
        {
            ColumnType& column = columns_[i];
            for (unsigned int j = 0; j != height_; ++j)
            {
                first[j] += column[j];
            }
        }

        // average
        double coeff = 1. / width_;
        if (roundValues)
        {
            for (unsigned int j = 0; j != height_; ++j)
            {
                const double d = first[j] * coeff;
                first[j] = std::round(d);
            }
        }
        else
        {
            for (unsigned int j = 0; j != height_; ++j)
            {
                first[j] *= coeff;
            }
        }

        // Release all timeseries no longer needed
        for (unsigned int i = 1; i != width_; ++i)
        {
            columns_[i].clear();
        }
        columns_.resize(1);
        // reset the width_ to 1
        width_ = 1;
    }
}

template<class T>
void Matrix<T>::fill(const T& v)
{
    for (auto& column: columns_)
    {
        std::fill(column.begin(), column.end(), v);
    }
}

template<class T>
void Matrix<T>::fillUnit()
{
    for (auto& column: columns_)
    {
        std::fill(column.begin(), column.end(), T{});
    }

    const auto diagonalSize = std::min(width_, height_);
    for (unsigned int i = 0; i != diagonalSize; ++i)
    {
        columns_[i][i] = T(1);
    }
}

template<class T>
inline void Matrix<T>::reset(unsigned int w, unsigned int h)
{
    resize(w, h);
    zero();
}

template<class T>
template<class U>
void Matrix<T>::pasteToColumn(unsigned int x, const U* data)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    ColumnType& column = columns_[x];

    // if the two types are strictly equal, we can perform some major
    // optimisations
    if (std::is_same_v<T, U>)
    {
        std::copy(data, data + height_, column.begin());
    }
    else
    {
        // ...otherwise we have to copy each item by hand in any cases
        for (unsigned int y = 0; y != height_; ++y)
        {
            column[y] = (T)data[y];
        }
    }
}

template<class T>
void Matrix<T>::fillColumn(unsigned int x, const T& value)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    ColumnType& column = columns_[x];

    std::fill(column.begin(), column.end(), value);
}

template<class T>
inline void Matrix<T>::columnToZero(unsigned int x)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    ColumnType& column = columns_[x];

    std::fill(column.begin(), column.end(), T{});
}

template<class T>
inline bool Matrix<T>::empty() const
{
    return (!width_) or (!height_);
}

template<class T>
void Matrix<T>::clear()
{
    columns_.clear();
    width_ = 0;
    height_ = 0;
}

template<class T>
void Matrix<T>::reset()
{
    clear();
}

template<class T>
void Matrix<T>::resize(unsigned int w, unsigned int h)
{
    // Asserts
    // This limit is correlated with the maximal amount of years
    // See the routine GeneralData::fixBadValues() if some changes are needed
    assert(w <= 50000 and "The new width_ seems a bit excessive");
    assert(h <= 50000 and "The new height_ seems a bit excessive");

    // Checking if the matrix really needs to be resized
    if (w != width_ or h != height_)
    {
        if (!w or !h)
        {
            clear();
        }
        else
        {
            // Assigning the new size
            width_ = w;
            height_ = h;

            // Allocating the columns_ for the matrix
            columns_.assign(w, ColumnType(h));
        }
    }
}

template<class T>
void Matrix<T>::resizeWithoutDataLost(unsigned int x, unsigned int y, const T& defVal)
{
    if (!x or !y)
    {
        clear();
    }
    else
    {
        if (x <= width_ and y <= height_) // shrinking
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
            const Matrix<T> copy(*this);
            resize(x, y);
            // Copy values
            const unsigned int minW = std::min(x, copy.width_);
            const unsigned int minH = std::min(y, copy.height_);

            for (unsigned int i = 0; i < minW; ++i)
            {
                ColumnType& column = columns_[i];

                std::copy_n(copy.columns_[i].begin(), minH, column.begin());

                for (unsigned int j = minH; j < y; ++j)
                {
                    column[j] = defVal;
                }
            }

            for (unsigned int i = minW; i < x; ++i)
            {
                std::fill(columns_[i].begin(), columns_[i].end(), defVal);
            }
        }
    }
    logs.debug() << "  :: end resizeWithoutDataLost";
}

template<class T>
template<class U>
void Matrix<T>::multiplyAllEntriesBy(const U& c)
{
    if (columns_.empty())
    {
        return;
    }

    if (!Utils::isZero(c))
    {
        for (unsigned int x = 0; x != width_; ++x)
        {
            ColumnType& column = columns_[x];

            for (unsigned int y = 0; y != height_; ++y)
            {
                column[y] *= (T)c;
            }
        }
    }
    else
    {
        zero();
    }
}

template<class T>
void Matrix<T>::roundAllEntries()
{
    for (unsigned int x = 0; x != width_; ++x)
    {
        ColumnType& col = columns_[x];
        for (unsigned int y = 0; y != height_; ++y)
        {
            col[y] = (T)std::round(col[y]);
        }
    }
}

template<class T>
bool Matrix<T>::containsOnlyZero() const
{
    PredicateIdentity predicate;
    return containsOnlyZero(predicate);
}

template<class T>
template<class PredicateT>
bool Matrix<T>::containsOnlyZero(PredicateT& predicate) const
{
    for (const auto& column: columns_)
    {
        for (const auto& value: column)
        {
            if (!Utils::isZero(static_cast<T>(predicate(value))))
            {
                return false;
            }
        }
    }
    return true;
}

template<class T>
template<class U>
void Matrix<T>::copyFrom(const Matrix<U>& rhs)
{
    assert((void*)(&rhs) != (void*)this and "Undefined behavior");

    if (rhs.empty())
    {
        clear();
    }
    else
    {
        // resize the matrix
        resize(rhs.width(), rhs.height());
        // copy raw values
        for (unsigned int x = 0; x != rhs.width(); ++x)
        {
            auto& column = columns_[x];
            const auto& src = rhs[x];

            // if the two types are strictly equal, we can perform some major
            // optimisations
            if (std::is_same_v<T, U>)
            {
                std::copy(src.begin(), src.end(), column.begin());
            }
            else
            {
                // ...otherwise we have to copy each item by hand in any cases
                for (unsigned int y = 0; y != height_; ++y)
                {
                    column[y] = (T)src[y];
                }
            }
        }
    }
}

template<class T>
template<class U>
inline void Matrix<T>::copyFrom(const Matrix<U>* rhs)
{
    if (rhs)
    {
        copyFrom(*rhs);
    }
}

template<class T>
void Matrix<T>::swap(Matrix<T>& rhs) noexcept
{
    // argument deduction lookup (ADL)
    using std::swap;
    swap(this->width_, rhs.width_);
    swap(this->height_, rhs.height_);
    swap(this->columns_, rhs.columns_);
}

template<class T>
inline Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs)
{
    copyFrom(rhs);
    return *this;
}

template<class T>
inline Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rhs) noexcept
{
    // Free existing resources before taking new ones
    width_ = rhs.width_;
    height_ = rhs.height_;
    columns_ = std::move(rhs.columns_);
    rhs.width_ = 0;
    rhs.height_ = 0;
    return *this;
}

template<class T>
template<class U>
inline Matrix<T>& Matrix<T>::operator=(const Matrix<U>& rhs)
{
    copyFrom(rhs);
    return *this;
}

template<class T>
bool MatrixTestForAtLeastOnePositiveValue(const Matrix<T>& m)
{
    if (m.width() and m.height())
    {
        unsigned int y;
        for (unsigned int x = 0; x < m.width(); ++x)
        {
            const auto& col = m[x];
            for (y = 0; y < m.height(); ++y)
            {
                if (col[y] > T(0))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

template<class T>
inline const typename Matrix<T>::ColumnType& Matrix<T>::operator[](unsigned int column) const
{
    assert(column < width_);
    return columns_[column];
}

template<class T>
inline typename Matrix<T>::ColumnType& Matrix<T>::operator[](unsigned int column)
{
    assert(column < width_);
    return columns_[column];
}

template<class T>
inline const typename Matrix<T>::ColumnType& Matrix<T>::column(unsigned int n) const
{
    assert(n < width_);
    return columns_[n];
}

template<class T>
inline typename Matrix<T>::ColumnType& Matrix<T>::column(unsigned int n)
{
    assert(n < width_);
    return columns_[n];
}
} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_MATRIX_HXX__
