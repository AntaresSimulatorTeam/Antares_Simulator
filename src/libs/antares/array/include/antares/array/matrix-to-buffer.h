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

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__

#include <yuni/core/string.h>

namespace Antares
{
template<class T, class ReadWriteT>
class Matrix;
}

namespace Antares
{
// Forward declarations
const char* get_format(bool isDecimal, uint precision);
template<class T, class ReadWriteT, class PredicateT>
class I_mtx_to_buffer_dumper;

class matrix_to_buffer_dumper_factory
{
public:
    matrix_to_buffer_dumper_factory()
    {
    }

    ~matrix_to_buffer_dumper_factory()
    {
    }

    template<class T, class ReadWriteT, class PredicateT>
    I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>* get_dumper(const Matrix<T, ReadWriteT>* mtx,
                                                                  std::string& data,
                                                                  PredicateT& predicate);
};

template<class T, class ReadWriteT, class PredicateT>
class I_mtx_to_buffer_dumper
{
public:
    I_mtx_to_buffer_dumper(const Matrix<T, ReadWriteT>* mtx,
                           std::string& data,
                           PredicateT& predicate):
        mtx_(mtx),
        buffer_(data),
        predicate_(predicate),
        format_(nullptr)
    {
    }

    void set_print_format(bool isDecimal, uint precision);
    virtual void run() = 0;

    ~I_mtx_to_buffer_dumper()
    {
        delete format_;
    }

protected:
    const Matrix<T, ReadWriteT>* mtx_;
    std::string& buffer_;
    PredicateT& predicate_;
    const char* format_;
};

template<class T, class ReadWriteT, class PredicateT>
class one_column__dumper: public I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>
{
public:
    one_column__dumper(const Matrix<T, ReadWriteT>* mtx, std::string& data, PredicateT& predicate):
        I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate)
    {
    }

    void run() override;
};

template<class T, class ReadWriteT, class PredicateT>
class multiple_columns__dumper: public I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>
{
public:
    multiple_columns__dumper(const Matrix<T, ReadWriteT>* mtx,
                             std::string& data,
                             PredicateT& predicate):
        I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate)
    {
    }

    void run() override;
};

} // namespace Antares

#include "matrix-to-buffer.hxx"

#endif // __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
