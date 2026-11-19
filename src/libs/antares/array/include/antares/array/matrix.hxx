// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares
{
inline unsigned int Matrix::width() const noexcept
{
    return ColumnMatrix<double>::width();
}

inline unsigned int Matrix::height() const noexcept
{
    return ColumnMatrix<double>::height();
}

inline Matrix::ConstColumnType Matrix::operator[](unsigned int column) const
{
    return ColumnMatrix<double>::operator[](column);
}

inline Matrix::ColumnType Matrix::operator[](unsigned int column)
{
    return ColumnMatrix<double>::operator[](column);
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
    for (const auto& column: columns_)
    {
        for (const auto& value: column)
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
