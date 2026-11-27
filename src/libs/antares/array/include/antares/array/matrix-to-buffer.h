// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__

#include <stdexcept>
#include <string>

#include "matrix.h"

namespace Antares
{
class MatrixSerializationError: public std::invalid_argument
{
public:
    using std::invalid_argument::invalid_argument;
};

template<class PredicateT>
void matrixToBuffer(const Matrix& matrix,
                    std::string& data,
                    PredicateT& predicate,
                    unsigned int precision);

} // namespace Antares

#include "matrix-to-buffer.hxx"

#endif // __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
