// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <cassert>
#include <span>
#include <utility>
#include <vector>

namespace Antares
{
template<class T>
class ColumnMatrix
{
public:
    using ColumnType = std::span<T>;
    using ConstColumnType = std::span<const T>;

    ColumnMatrix() = default;
    ColumnMatrix(const ColumnMatrix&) = default;

    ColumnMatrix& operator=(const ColumnMatrix& rhs)
    {
        if (this != &rhs)
        {
            ColumnMatrix copy(rhs);
            swap(copy);
        }
        return *this;
    }

    ColumnMatrix(ColumnMatrix&& rhs) noexcept
    {
        swap(rhs);
    }

    ColumnMatrix& operator=(ColumnMatrix&& rhs) noexcept
    {
        if (this != &rhs)
        {
            ColumnMatrix moved(std::move(rhs));
            swap(moved);
        }
        return *this;
    }

    void reset(unsigned int width, unsigned int height)
    {
        if (!width || !height)
        {
            clear();
            return;
        }
        if (width == width_ && height == height_)
        {
            for (auto& column: columns_)
            {
                std::fill(column.begin(), column.end(), T{});
            }
            return;
        }
        std::vector<std::vector<T>> columns(width, std::vector<T>(height));
        columns_.swap(columns);
        width_ = width;
        height_ = height;
    }

    void clear() noexcept
    {
        columns_.clear();
        width_ = height_ = 0;
    }

    // Resizing to the current shape leaves values unchanged.
    void resize(unsigned int width, unsigned int height)
    {
        if (width == width_ && height == height_)
        {
            return;
        }
        if (!width || !height)
        {
            clear();
            return;
        }
        std::vector<std::vector<T>> columns(width);
        for (auto& column: columns)
        {
            column.resize(height);
        }
        columns_.swap(columns);
        width_ = width;
        height_ = height;
    }

    void resizeWithoutDataLost(unsigned int width, unsigned int height, const T& defaultValue = T{})
    {
        if (!width || !height)
        {
            clear();
        }
        else if (width <= width_ && height <= height_)
        {
            if (width == width_)
            {
                for (auto& column: columns_)
                {
                    column.resize(height);
                }
            }
            else
            {
                std::vector<std::vector<T>> columns(columns_.begin(), columns_.begin() + width);
                for (auto& column: columns)
                {
                    column.resize(height);
                }
                columns_.swap(columns);
            }
            width_ = width;
            height_ = height;
        }
        else
        {
            const auto preservedWidth = std::min(width, width_);
            const auto preservedHeight = std::min(height, height_);
            std::vector<std::vector<T>> columns(width);
            for (unsigned int i = 0; i < width; ++i)
            {
                columns[i].assign(height, defaultValue);
                if (i < preservedWidth)
                {
                    std::copy_n(columns_[i].begin(), preservedHeight, columns[i].begin());
                }
            }
            columns_.swap(columns);
            width_ = width;
            height_ = height;
        }
    }

    unsigned int width() const noexcept
    {
        return width_;
    }

    unsigned int height() const noexcept
    {
        return height_;
    }

    // Views are invalidated when storage is resized, cleared or replaced.
    ColumnType operator[](unsigned int index)
    {
        assert(index < width_);
        return columns_[index];
    }

    ConstColumnType operator[](unsigned int index) const
    {
        assert(index < width_);
        return columns_[index];
    }

    ColumnType column(unsigned int index)
    {
        return (*this)[index];
    }

    ConstColumnType column(unsigned int index) const
    {
        return (*this)[index];
    }

    void swap(ColumnMatrix& rhs) noexcept
    {
        using std::swap;
        swap(width_, rhs.width_);
        swap(height_, rhs.height_);
        columns_.swap(rhs.columns_);
    }

protected:
    unsigned int width_ = 0;
    unsigned int height_ = 0;
    std::vector<std::vector<T>> columns_;
};
} // namespace Antares
