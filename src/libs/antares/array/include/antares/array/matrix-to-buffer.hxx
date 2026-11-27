// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <stdexcept>

namespace Antares
{
namespace // anonymous
{
struct MatrixScalar
{
    static void Append(std::string& file, double v, unsigned int precision)
    {
        if (!std::isfinite(v))
        {
            throw MatrixSerializationError("Cannot serialize non-finite matrix value");
        }
        if (v == 0.) // NOSONAR
        {
            v = 0.;
        }
        // A finite double in fixed notation needs at most 309 integer digits,
        // a sign, a decimal point and the supported fractional digits.
        char buffer[328];
        // Only exactly integral values can be printed without fractional digits.
        const int digits = (v == std::trunc(v)) ? 0 : static_cast<int>(precision); // NOSONAR
        const auto [end, error] = std::to_chars(buffer,
                                                buffer + sizeof(buffer),
                                                v,
                                                std::chars_format::fixed,
                                                digits);
        if (error != std::errc{})
        {
            throw MatrixSerializationError("Failed to format matrix value");
        }
        file.append(buffer, end);
    }
};

} // anonymous namespace

template<class PredicateT>
void matrixToBuffer(const Matrix& matrix,
                    std::string& data,
                    PredicateT& predicate,
                    unsigned int precision)
{
    precision = std::min(precision, 16u);

    for (unsigned int y = 0; y != matrix.height(); ++y)
    {
        for (unsigned int x = 0; x != matrix.width(); ++x)
        {
            if (x)
            {
                data += '\t';
            }
            MatrixScalar::Append(data, static_cast<double>(predicate(matrix[x][y])), precision);
        }
        data += '\n';
    }
}

} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
