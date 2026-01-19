// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    std::unique_ptr<I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>>
    get_dumper(const Matrix<T, ReadWriteT>* mtx, std::string& data, PredicateT& predicate);
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
        predicate_(predicate)
    {
    }

    virtual ~I_mtx_to_buffer_dumper() = default;

    void set_print_format(bool isDecimal, uint precision);
    virtual void run() = 0;

protected:
    const Matrix<T, ReadWriteT>* mtx_;
    std::string& buffer_;
    PredicateT& predicate_;
    std::string format_;
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
