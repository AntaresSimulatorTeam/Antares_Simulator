// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__

#include <string>

namespace Antares
{
template<class T>
class Matrix;
}

namespace Antares
{
template<class T, class PredicateT>
void matrixToBuffer(const Matrix<T>& matrix,
                    std::string& data,
                    PredicateT& predicate,
                    bool isDecimal,
                    unsigned int precision);

} // namespace Antares

#include "matrix-to-buffer.hxx"

#endif // __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
