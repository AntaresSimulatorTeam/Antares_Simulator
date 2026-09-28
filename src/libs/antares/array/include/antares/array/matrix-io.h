// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_H
#define ANTARES_ARRAY_MATRIX_IO_H

#include <filesystem>

#include "matrix.h"

namespace Antares::MatrixIO
{
template<class T, class ReadWriteT = T>
bool load(Matrix<T, ReadWriteT>& matrix,
          const AnyString& filename,
          uint minWidth,
          uint maxHeight,
          uint options = Matrix<T, ReadWriteT>::optNone,
          typename Matrix<T, ReadWriteT>::BufferType* buffer = nullptr)
{
    return matrix.loadFromCSVFile(filename, minWidth, maxHeight, options, buffer);
}

template<class T, class ReadWriteT = T>
bool load(Matrix<T, ReadWriteT>& matrix,
          const std::filesystem::path& filename,
          uint minWidth = 1,
          uint maxHeight = 0,
          uint options = Matrix<T, ReadWriteT>::optNone,
          typename Matrix<T, ReadWriteT>::BufferType* buffer = nullptr)
{
    return matrix.loadFromCSVFile(filename.string(), minWidth, maxHeight, options, buffer);
}

template<class T, class ReadWriteT = T>
void saveToBuffer(const Matrix<T, ReadWriteT>& matrix, std::string& data, uint precision = 6)
{
    matrix.saveToBuffer(data, precision);
}

template<class T, class ReadWriteT = T>
bool saveToCSVFile(const Matrix<T, ReadWriteT>& matrix,
                  const AnyString& filename,
                  uint precision = 6,
                  bool printDimensions = false,
                  bool saveEvenIfAllZero = false)
{
    return matrix.saveToCSVFile(filename, precision, printDimensions, saveEvenIfAllZero);
}

template<class T, class ReadWriteT = T, class Predicate = typename Matrix<T, ReadWriteT>::PredicateIdentity>
bool save(const Matrix<T, ReadWriteT>& matrix,
          const AnyString& filename,
          uint precision = 6,
          bool printDimensions = false,
          Predicate predicate = {},
          bool saveEvenIfAllZero = false)
{
    return matrix.saveToCSVFile(filename,
                                precision,
                                printDimensions,
                                predicate,
                                saveEvenIfAllZero);
}
} // namespace Antares::MatrixIO

#endif
