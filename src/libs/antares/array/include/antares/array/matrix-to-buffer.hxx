/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__

#ifdef YUNI_OS_MSVC
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

template<class T, class ReadWriteT, class PredicateT>
I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>* matrix_to_buffer_dumper_factory::get_dumper(
  const Matrix<T, ReadWriteT>* mtx,
  std::string& data,
  PredicateT& predicate)
{
    if (mtx->width == 1)
    {
        return new one_column__dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate);
    }
    else
    {
        return new multiple_columns__dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate);
    }
}

template<class T, class ReadWriteT, class PredicateT>
void I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>::set_print_format(bool isDecimal,
                                                                         uint precision)
{
    // Determining the string format to use according the given precision
    format_ = "%.0f";

    if (isDecimal and precision)
    {
        const char* const sfmt[] = {
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
        assert(precision <= 16);
        format_ = sfmt[precision];
    }
}

template<class T, class ReadWriteT, class PredicateT>
void one_column__dumper<T, ReadWriteT, PredicateT>::run()
{
    for (uint y = 0; y != (this->mtx_)->height; ++y)
    {
        MatrixScalar<ReadWriteT>::Append(this->buffer_,
                                         (ReadWriteT)this->predicate_((this->mtx_)->entry[0][y]),
                                         this->format_);
        this->buffer_ += '\n';
    }
}

template<class T, class ReadWriteT, class PredicateT>
void multiple_columns__dumper<T, ReadWriteT, PredicateT>::run()
{
    for (uint y = 0; y < (this->mtx_)->height; ++y)
    {
        MatrixScalar<ReadWriteT>::Append(this->buffer_,
                                         (ReadWriteT)this->predicate_((this->mtx_)->entry[0][y]),
                                         this->format_);
        for (uint x = 1; x < (this->mtx_)->width; ++x)
        {
            this->buffer_ += '\t';
            MatrixScalar<ReadWriteT>::Append(this->buffer_,
                                             (ReadWriteT)this->predicate_(
                                               (this->mtx_)->entry[x][y]),
                                             this->format_);
        }
        this->buffer_ += '\n';
    }
}

} // namespace Antares

#undef ANTARES_MATRIX_SNPRINTF

#endif // __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
