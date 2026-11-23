// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_H
#define ANTARES_ARRAY_MATRIX_IO_H

#include <filesystem>
#include <functional>
#include <optional>
#include <string>

#include "matrix-to-buffer.h"

namespace Antares::MatrixIO
{
enum class WidthPolicy
{
    AtLeast,
    Exact
};
enum class RowPolicy
{
    Exact,
    Prefix,
    // A header may select a height between minHeaderHeight and the requested
    // height. Headerless files are read as a prefix of the requested height.
    HeaderOrPrefix,
    // A header selects its height; otherwise consume the requested prefix.
    HeaderHeightOrPrefix
};

// minWidth and height specify the requested matrix shape. AtLeast allows
// additional columns; Exact requires exactly minWidth. The row policy controls
// extra rows and whether a header can select the height. minHeaderHeight only
// applies to HeaderOrPrefix and must not exceed height.
struct LoadOptions
{
    unsigned int minWidth;
    unsigned int height;
    WidthPolicy widthPolicy = WidthPolicy::AtLeast;
    RowPolicy rowPolicy = RowPolicy::Exact;
    unsigned int minHeaderHeight = 0;
};

struct LoadError
{
    std::filesystem::path filename;
    std::optional<std::size_t> row;
    std::optional<std::size_t> column;
    std::string message;
};

// Rows and columns in LoadError are one-based. Empty cells contain zero; an
// empty file represents a zero matrix of the requested size. By default all
// rows must match the requested height; Prefix explicitly permits extra rows.
// HeaderOrPrefix accepts an exact header-selected height in [minHeaderHeight,
// height], or consumes a prefix of height rows when there is no header.
// HeaderHeightOrPrefix consumes the header-selected height, if present, or
// the requested height otherwise, ignoring any subsequent rows.
// Returns an error on failure and leaves destination unchanged.
std::optional<LoadError> loadDetailed(Matrix& destination,
                                      const std::filesystem::path& filename,
                                      const LoadOptions& options);

// On failure, destination is unchanged. An empty file represents a zero matrix.
bool load(Matrix& destination, const std::string& filename, const LoadOptions& options);
bool load(Matrix& destination, const std::filesystem::path& filename, const LoadOptions& options);
bool loadOrReset(Matrix& destination,
                 const std::filesystem::path& filename,
                 const LoadOptions& options);

bool writeBuffer(const std::string& filename, const std::string& data);
void logSerializationError(const std::string& filename, const std::string& message);

template<class Predicate = std::identity>
void saveToBuffer(const Matrix& matrix,
                  std::string& data,
                  unsigned int precision = 6,
                  bool printDimensions = false,
                  Predicate predicate = {},
                  bool saveEvenIfAllZero = false)
{
    if (!printDimensions && !saveEvenIfAllZero && matrix.containsOnlyZero(predicate))
    {
        data.clear();
        return;
    }
    std::string serialized;
    serialized.reserve(static_cast<std::size_t>(matrix.width()) * matrix.height() * 6);
    if (printDimensions)
    {
        serialized += "size:" + std::to_string(matrix.width()) + 'x'
                      + std::to_string(matrix.height()) + '\n';
    }
    matrixToBuffer(matrix, serialized, predicate, precision);
    data = std::move(serialized);
}

template<class Predicate = std::identity>
bool save(const Matrix& matrix,
          const std::string& filename,
          unsigned int precision = 6,
          bool printDimensions = false,
          Predicate predicate = {},
          bool saveEvenIfAllZero = false)
{
    std::string data;
    try
    {
        if (!matrix.empty())
        {
            saveToBuffer(matrix, data, precision, printDimensions, predicate, saveEvenIfAllZero);
        }
    }
    catch (const MatrixSerializationError& error)
    {
        logSerializationError(filename, error.what());
        return false;
    }
    return writeBuffer(filename, data);
}
} // namespace Antares::MatrixIO

#endif
