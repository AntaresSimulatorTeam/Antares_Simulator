// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>

#ifdef _MSC_VER
#define ANTARES_MATRIX_SNPRINTF sprintf_s
#else
#define ANTARES_MATRIX_SNPRINTF snprintf
#endif

#include <antares/utils/utils.h>

namespace Antares
{
namespace // anonymous
{
template<class T>
struct MatrixScalar
{
    static inline void Append(std::string& file, T v, const char* const)
    {
        if (Utils::isZero(v))
        {
            file.append(std::to_string(0));
        }
        else
        {
            file.append(std::to_string(v));
        }
    }
};

template<>
struct MatrixScalar<double>
{
    static void Append(std::string& file, double v, const char* const format)
    {
        if (Utils::isZero(v))
        {
            file += '0';
        }
        else
        {
            char ConversionBuffer[128];
            const int sizePrintf = Utils::isZero(v - floor(v))
                                     ? ANTARES_MATRIX_SNPRINTF(ConversionBuffer,
                                                               sizeof(ConversionBuffer),
                                                               "%.0f",
                                                               v)
                                     : ANTARES_MATRIX_SNPRINTF(ConversionBuffer,
                                                               sizeof(ConversionBuffer),
                                                               format,
                                                               v);

            if (sizePrintf >= 0 and sizePrintf < (int)(sizeof(ConversionBuffer)))
            {
                file += (const char*)ConversionBuffer;
            }
            else
            {
                file += "ERR";
            }
        }
    }
};

template<>
struct MatrixScalar<float>
{
    static void Append(std::string& file, float v, const char* const format)
    {
        if (Utils::isZero(v))
        {
            file += '0';
        }
        else
        {
            char ConversionBuffer[128];
            const int sizePrintf = Utils::isZero(v - floor(v))
                                     ? ANTARES_MATRIX_SNPRINTF(ConversionBuffer,
                                                               sizeof(ConversionBuffer),
                                                               "%.0f",
                                                               (double)v)
                                     : ANTARES_MATRIX_SNPRINTF(ConversionBuffer,
                                                               sizeof(ConversionBuffer),
                                                               format,
                                                               (double)v);

            if (sizePrintf >= 0 and sizePrintf < (int)(sizeof(ConversionBuffer)))
            {
                file += (const char*)ConversionBuffer;
            }
            else
            {
                file += "ERR";
            }
        }
    }
};

} // anonymous namespace

template<class T, class PredicateT>
void matrixToBuffer(const Matrix<T>& matrix,
                    std::string& data,
                    PredicateT& predicate,
                    bool isDecimal,
                    unsigned int precision)
{
    static constexpr std::array formats = {
      "%.0f",
      "%.1f",
      "%.2f",
      "%.3f",
      "%.4f",
      "%.5f",
      "%.6f",
      "%.7f",
      "%.8f",
      "%.9f",
      "%.10f",
      "%.11f",
      "%.12f",
      "%.13f",
      "%.14f",
      "%.15f",
      "%.16f",
    };

    const char* format = formats[0];
    if (isDecimal)
    {
        format = formats[std::min(precision, static_cast<unsigned int>(formats.size() - 1))];
    }

    for (unsigned int y = 0; y != matrix.height(); ++y)
    {
        for (unsigned int x = 0; x != matrix.width(); ++x)
        {
            if (x)
            {
                data += '\t';
            }
            MatrixScalar<T>::Append(data,
                                    static_cast<T>(predicate(matrix[x][y])),
                                    format);
        }
        data += '\n';
    }
}

} // namespace Antares

#undef ANTARES_MATRIX_SNPRINTF

#endif // __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
