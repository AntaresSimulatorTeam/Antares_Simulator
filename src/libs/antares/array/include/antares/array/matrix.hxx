// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_HXX__

#include <cmath>
#include <type_traits>
#include <utility>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

namespace Antares
{
template<class T, class ReadWriteT>
inline Matrix<T, ReadWriteT>::Matrix():
    width_(0),
    height_(0),
    columns_()
{
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::Matrix(unsigned int w, unsigned int h):
    width_(w),
    height_(h)
{
    columns_.assign(w, ColumnType(h));
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::Matrix(const Matrix<T, ReadWriteT>& rhs):
    width_(rhs.width_),
    height_(rhs.height_),
    columns_(rhs.columns_)
{
}

template<class T, class ReadWriteT>
unsigned int Matrix<T, ReadWriteT>::width() const noexcept
{
    return width_;
}

template<class T, class ReadWriteT>
unsigned int Matrix<T, ReadWriteT>::height() const noexcept
{
    return height_;
}

template<class T, class ReadWriteT>
typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::mutableColumn(
  unsigned int column) const
{
    assert(column < width_);
    return columns_[column];
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::Matrix(Matrix<T, ReadWriteT>&& rhs) noexcept:
    width_(rhs.width_),
    height_(rhs.height_),
    columns_(std::move(rhs.columns_))
{
    rhs.width_ = 0;
    rhs.height_ = 0;
}

template<class T, class ReadWriteT>
template<class U, class V>
Matrix<T, ReadWriteT>::Matrix(const Matrix<U, V>& rhs):
    width_(0),
    height_(0),
    columns_()
{
    copyFrom(rhs);
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::zero()
{
    for (unsigned int i = 0; i != width_; ++i)
    {
        ColumnType& column = columns_[i];
        std::fill(column.begin(), column.end(), T{});
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::averageTimeseries(bool roundValues)
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

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::fill(const T& v)
{
    for (unsigned int i = 0; i != width_; ++i)
    {
        ColumnType& column = columns_[i];

        for (unsigned int j = 0; j != height_; ++j)
        {
            column[j] = v;
        }
    }
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::fillUnit()
{
    for (unsigned int i = 0; i != width_; ++i)
    {
        ColumnType& column = columns_[i];

        std::fill(column.begin(), column.end(), T{});

        column[i] = T(1);
    }
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::reset(unsigned int w, unsigned int h)
{
    resize(w, h);
    zero();
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::pasteToColumn(unsigned int x, const U* data)
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

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::fillColumn(unsigned int x, const T& value)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    ColumnType& column = columns_[x];

    for (unsigned int y = 0; y != height_; ++y)
    {
        column[y] = value;
    }
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::columnToZero(unsigned int x)
{
    assert(x < width_ and "Invalid column index (bigger than `this->width_`)");
    ColumnType& column = columns_[x];

    std::fill(column.begin(), column.end(), T{});
}

template<class T, class ReadWriteT>
inline bool Matrix<T, ReadWriteT>::empty() const
{
    return (!width_) or (!height_);
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::clear()
{
    columns_.clear();
    width_ = 0;
    height_ = 0;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::reset()
{
    clear();
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::resize(unsigned int w, unsigned int h)
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

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::resizeWithoutDataLost(unsigned int x,
                                                  unsigned int y,
                                                  const T& defVal)
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
            const Matrix<T, ReadWriteT> copy(*this);
            resize(x, y);
            // Copy values
            unsigned int minW = (x < copy.width_) ? x : copy.width_;
            unsigned int minH = (y < copy.height_) ? y : copy.height_;

            for (unsigned int i = 0; i < minW; ++i)
            {
                ColumnType& column = columns_[i];

                std::copy_n(copy.columns_[i].begin(), minH, column.begin());

                for (unsigned int j = minH; j < y; ++j)
                {
                    column[j] = defVal;
                }
            }

            if (defVal == T())
            {
                for (unsigned int i = minW; i < x; ++i)
                {
                    std::fill(columns_[i].begin(), columns_[i].end(), T{});
                }
            }
            else
            {
                for (unsigned int i = minW; i < x; ++i)
                {
                    std::fill(columns_[i].begin(), columns_[i].end(), defVal);
                }
            }
        }
    }
    logs.debug() << "  :: end resizeWithoutDataLost";
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::multiplyAllEntriesBy(const U& c)
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

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::roundAllEntries()
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

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::containsOnlyZero() const
{
    for (const auto& column: columns_)
    {
        for (const auto& value: column)
        {
            if (!Utils::isZero(static_cast<T>(value)))
            {
                return false;
            }
        }
    }
    return true;
}

template<class T, class ReadWriteT>
template<class U, class V>
void Matrix<T, ReadWriteT>::copyFrom(const Matrix<U, V>& rhs)
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

template<class T, class ReadWriteT>
template<class U, class V>
inline void Matrix<T, ReadWriteT>::copyFrom(const Matrix<U, V>* rhs)
{
    if (rhs)
    {
        copyFrom(*rhs);
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::swap(Matrix<T, ReadWriteT>& rhs) noexcept
{
    // argument deduction lookup (ADL)
    using std::swap;
    swap(this->width_, rhs.width_);
    swap(this->height_, rhs.height_);
    swap(this->columns_, rhs.columns_);
}

template<class T, class ReadWriteT>
inline Matrix<T, ReadWriteT>& Matrix<T, ReadWriteT>::operator=(const Matrix<T, ReadWriteT>& rhs)
{
    copyFrom(rhs);
    return *this;
}

template<class T, class ReadWriteT>
inline Matrix<T, ReadWriteT>& Matrix<T, ReadWriteT>::operator=(Matrix<T, ReadWriteT>&& rhs) noexcept
{
    // Free existing resources before taking new ones
    width_ = rhs.width_;
    height_ = rhs.height_;
    columns_ = std::move(rhs.columns_);
    rhs.width_ = 0;
    rhs.height_ = 0;
    return *this;
}

template<class T, class ReadWriteT>
template<class U>
inline Matrix<T, ReadWriteT>& Matrix<T, ReadWriteT>::operator=(const Matrix<U>& rhs)
{
    copyFrom(rhs);
    return *this;
}

template<class T1, class T2>
bool MatrixTestForAtLeastOnePositiveValue(const Matrix<T1, T2>& m)
{
    if (m.width() and m.height())
    {
        unsigned int y;
        for (unsigned int x = 0; x < m.width(); ++x)
        {
            const auto& col = m[x];
            for (y = 0; y < m.height(); ++y)
            {
                if (col[y] > T1(0))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

template<class T, class ReadWriteT>
inline const typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::operator[](
  unsigned int column) const
{
    assert(column < width_);
    return columns_[column];
}

template<class T, class ReadWriteT>
inline typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::operator[](
  unsigned int column)
{
    assert(column < width_);
    return columns_[column];
}

template<class T, class ReadWriteT>
inline const typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::column(
  unsigned int n) const
{
    assert(n < width_);
    return columns_[n];
}

template<class T, class ReadWriteT>
inline typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::column(unsigned int n)
{
    assert(n < width_);
    return columns_[n];
}
} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_MATRIX_HXX__
