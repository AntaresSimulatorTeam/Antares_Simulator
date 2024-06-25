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

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__

#include "fill-matrix.h"

using namespace Yuni;

namespace Antares
{
namespace UnitTests
{
struct PredicateIdentity
{
    template<class U>
    inline U operator()(const U& value) const
    {
        return value;
    }
};
} // namespace UnitTests
} // namespace Antares

template<class T = double, class ReadWriteT = T>
class Matrix_load_bypass: public Matrix_easy_to_fill<T, ReadWriteT>
{
    using BufferType = typename Matrix<T, ReadWriteT>::BufferType;

public:
    Matrix_load_bypass():
        Matrix_easy_to_fill<T, ReadWriteT>(),
        loadFromCSVFile_called(false){};

    Matrix_load_bypass(uint height, uint width):
        Matrix_easy_to_fill<T, ReadWriteT>(height, width),
        loadFromCSVFile_called(false){};

    Matrix_load_bypass(uint height, uint width, const vector<T>& vec):
        Matrix_easy_to_fill<T, ReadWriteT>(height, width, vec),
        loadFromCSVFile_called(false){};

    bool loadFromCSVFile(const AnyString& /* filename */,
                         uint /* minWidth */,
                         uint /* maxHeight */,
                         uint /* options */,
                         BufferType* /* buffer */) override
    {
        loadFromCSVFile_called = true;
        return true;
    }

public:
    bool loadFromCSVFile_called;
};

template<class T = double, class ReadWriteT = T>
class fake_buffer_factory
{
public:
    fake_buffer_factory():
        buffer_precision_(0),
        buffer_print_dimensions_(false)
    {
    }

    ~fake_buffer_factory() = default;

    void matrix_to_build_buffer_with(Matrix_easy_to_fill<T, ReadWriteT>* mtx)
    {
        mtx_to_build_buffer_with_ = mtx;
    }

    void set_precision(uint precision)
    {
        buffer_precision_ = precision;
    }

    void print_dimensions(bool print_dims)
    {
        buffer_print_dimensions_ = print_dims;
    }

    Clob* build_buffer()
    {
        Clob* buffer_to_return = new Clob;
        std::string buffer;
        Antares::UnitTests::PredicateIdentity predicate;

        mtx_to_build_buffer_with_->saveToFileDescriptor(buffer,
                                                        buffer_precision_,
                                                        buffer_print_dimensions_,
                                                        predicate);

        buffer_to_return->append(buffer);

        return buffer_to_return;
    }

private:
    uint buffer_precision_;
    bool buffer_print_dimensions_;
    Matrix_easy_to_fill<T, ReadWriteT>* mtx_to_build_buffer_with_;
};

template<class T = double, class ReadWriteT = T>
class Matrix_mock_load_to_buffer: public Matrix<T, ReadWriteT>
{
public:
    Matrix_mock_load_to_buffer():
        Matrix<T, ReadWriteT>(),
        fake_mtx_error_when_loading_(IO::errNone){};

    Matrix_mock_load_to_buffer(uint height, uint width):
        Matrix<T, ReadWriteT>(height, width),
        fake_mtx_error_when_loading_(IO::errNone){};

    Matrix_mock_load_to_buffer(uint height, uint width, const vector<T>& vec):
        Matrix<T, ReadWriteT>(height, width, vec),
        fake_mtx_error_when_loading_(IO::errNone){};

    IO::Error loadFromFileToBuffer(typename Matrix<T, ReadWriteT>::BufferType& /* buffer */,
                                   const AnyString& /* filename */) const override
    {
        return fake_mtx_error_when_loading_;
    }

    void error_when_loading_from_file(IO::Error err)
    {
        fake_mtx_error_when_loading_ = err;
    }

private:
    IO::Error fake_mtx_error_when_loading_;
};

#endif // __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__
