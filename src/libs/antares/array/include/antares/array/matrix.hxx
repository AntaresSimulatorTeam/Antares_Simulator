// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares
{
inline unsigned int Matrix::width() const noexcept
{
    return storage_.width();
}

inline unsigned int Matrix::height() const noexcept
{
    return storage_.height();
}

inline Matrix::ConstColumnType Matrix::operator[](unsigned int column) const
{
    return storage_[column];
}

inline Matrix::ColumnType Matrix::operator[](unsigned int column)
{
    return storage_[column];
}

inline Matrix::ConstColumnType Matrix::column(unsigned int n) const
{
    return (*this)[n];
}

inline Matrix::ColumnType Matrix::column(unsigned int n)
{
    return (*this)[n];
}

template<class PredicateT>
bool Matrix::containsOnlyZero(const PredicateT& predicate) const
{
    for (unsigned int x = 0; x != width(); ++x)
    {
        for (const auto& value: storage_[x])
        {
            // Exact zero is required: small nonzero values must be serialized.
            if (predicate(value) != 0.) // NOSONAR
            {
                return false;
            }
        }
    }
    return true;
}
} // namespace Antares
