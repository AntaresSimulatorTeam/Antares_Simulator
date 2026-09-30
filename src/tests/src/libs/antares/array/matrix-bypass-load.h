// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__

#include <antares/array/matrix-io.h>

#include "fill-matrix.h"

namespace Antares::UnitTests
{
struct PredicateIdentity
{
    template<class U>
    inline U operator()(const U& value) const
    {
        return value;
    }
};
} // namespace Antares::UnitTests

template<class T = double, class ReadWriteT = T>
class Matrix_load_bypass: public Matrix_easy_to_fill<T, ReadWriteT>
{
    using BufferType = MatrixIO::BufferType;

public:
    Matrix_load_bypass():
        Matrix_easy_to_fill<T, ReadWriteT>()
    {
    }

    Matrix_load_bypass(unsigned int height, unsigned int width):
        Matrix_easy_to_fill<T, ReadWriteT>(height, width)
    {
    }

    Matrix_load_bypass(unsigned int height,
                       unsigned int width,
                       const std::vector<T>& vec):
        Matrix_easy_to_fill<T, ReadWriteT>(height, width, vec)
    {
    }

public:
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

    void set_precision(unsigned int precision)
    {
        buffer_precision_ = precision;
    }

    void print_dimensions(bool print_dims)
    {
        buffer_print_dimensions_ = print_dims;
    }

    MatrixIO::BufferType* build_buffer()
    {
        auto* buffer_to_return = new MatrixIO::BufferType;
        std::string buffer;
        Antares::UnitTests::PredicateIdentity predicate;

        MatrixIO::saveToBuffer(*mtx_to_build_buffer_with_,
                               buffer,
                               buffer_precision_,
                               buffer_print_dimensions_,
                               predicate);

        buffer_to_return->append(buffer);

        return buffer_to_return;
    }

private:
    unsigned int buffer_precision_;
    bool buffer_print_dimensions_;
    Matrix_easy_to_fill<T, ReadWriteT>* mtx_to_build_buffer_with_;
};

template<class T = double, class ReadWriteT = T>
class Matrix_mock_load_to_buffer: public Matrix<T, ReadWriteT>
{
public:
    Matrix_mock_load_to_buffer():
        Matrix<T, ReadWriteT>()
    {
    }

    Matrix_mock_load_to_buffer(unsigned int height, unsigned int width):
        Matrix<T, ReadWriteT>(height, width)
    {
    }

    Matrix_mock_load_to_buffer(unsigned int height,
                               unsigned int width,
                               const std::vector<T>& vec):
        Matrix<T, ReadWriteT>(height, width, vec)
    {
    }

};

#endif // __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__
