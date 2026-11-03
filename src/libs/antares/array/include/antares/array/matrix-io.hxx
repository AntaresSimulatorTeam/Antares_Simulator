// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_HXX
#define ANTARES_ARRAY_MATRIX_IO_HXX

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <new>
#include <string>
#include <string_view>

#include <antares/io/CsvReader.h>
#include <antares/io/statistics.h>
#include <antares/logs/logs.h>

#include "matrix-to-buffer.h"
#include "matrix.h"

namespace Antares::MatrixIO
{
enum class WidthPolicy
{
    AtLeast,
    Exact,
};

namespace
{
constexpr std::uintmax_t kMaxMatrixFileSize = Matrix::kMaxAllocationBytes;

bool parseDouble(std::string_view text, double& value)
{
    const auto first = text.data();
    const auto last = first + text.size();
    const auto [end, error] = std::from_chars(first, last, value);
    if (error == std::errc{} && end == last)
    {
        return true;
    }

    const std::string stringValue(text);
    char* legacyEnd = nullptr;
    value = std::strtod(stringValue.c_str(), &legacyEnd);
    return legacyEnd != nullptr && *legacyEnd == '\0';
}

bool startsWithDimensionHeader(std::string_view text)
{
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t'))
    {
        text.remove_prefix(1);
    }
    return text.starts_with("size:");
}

std::string_view trimDimensionHeader(std::string_view text)
{
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t'))
    {
        text.remove_prefix(1);
    }
    while (!text.empty() && (text.back() == ' ' || text.back() == '\t'))
    {
        text.remove_suffix(1);
    }
    return text;
}

bool parseDimensionHeader(std::string_view text, unsigned int& width, unsigned int& height)
{
    text = trimDimensionHeader(text);
    constexpr std::string_view prefix = "size:";
    const auto separator = text.find('x', prefix.size());
    if (separator == std::string_view::npos || separator == prefix.size()
        || separator + 1 == text.size())
    {
        return false;
    }

    const auto parseUnsigned = [](std::string_view value, unsigned int& result)
    {
        const auto first = value.data();
        const auto last = first + value.size();
        const auto [end, error] = std::from_chars(first, last, result);
        return error == std::errc{} && end == last;
    };

    return parseUnsigned(text.substr(prefix.size(), separator - prefix.size()), width)
           && parseUnsigned(text.substr(separator + 1), height);
}

bool validDimensions(unsigned int width, unsigned int height)
{
    return width <= Matrix::kMaxDimension && height <= Matrix::kMaxDimension
           && static_cast<std::uintmax_t>(width) * height <= Matrix::kMaxElements;
}
} // namespace

inline bool load(Matrix& matrix,
                 const std::string& filename,
                 unsigned int minWidth,
                 unsigned int height,
                 WidthPolicy widthPolicy = WidthPolicy::AtLeast)
{
    assert(!filename.empty());

    const auto resetToRequestedDimensions = [&]()
    {
        if (validDimensions(minWidth, height))
        {
            matrix.reset(minWidth, height);
        }
        else
        {
            matrix.clear();
        }
    };

    if (!validDimensions(minWidth, height))
    {
        logs.error() << filename << ": matrix dimensions exceed the supported limit";
        matrix.clear();
        return false;
    }

    std::error_code error;
    const auto size = std::filesystem::file_size(filename, error);
    if (error)
    {
        logs.error() << "I/O Error: not found: '" << filename << "'";
        resetToRequestedDimensions();
        return false;
    }

    if (size > kMaxMatrixFileSize)
    {
        logs.error() << filename << ": matrix file is too large (maximum 1536 MiB)";
        resetToRequestedDimensions();
        return false;
    }

    if (size == 0)
    {
        resetToRequestedDimensions();
        return true;
    }

    if (!minWidth || !height)
    {
        logs.error() << filename << ": matrix dimensions are required for a non-empty file";
        resetToRequestedDimensions();
        return false;
    }

    Statistics::HasReadFromDisk(size);
    logs.debug() << "  :: loading `" << filename << "`";

    unsigned int rowCount = 0;
    unsigned int targetWidth = 0;
    unsigned int targetHeight = height;
    bool matrixInitialized = false;
    bool result = true;
    int errorCount = 6;

    const auto handleInvalidNumericValue =
      [&](unsigned int cellX, unsigned int cellY, std::string_view cell)
    {
        result = false;
        if (errorCount > 0)
        {
            logs.warning() << '`' << filename << "`: Invalid numeric value (x:" << cellX
                           << ",y:" << cellY << ", text: `" << std::string(cell) << '`';
            if (!(--errorCount))
            {
                logs.warning() << " ... (skipped)";
            }
        }
        matrix[cellX][cellY] = 0.;
    };

    try
    {
        IO::readCSVFile(
          filename,
          "\t;,",
          [&](std::size_t, IO::CSVRow row)
          {
              if (matrixInitialized && rowCount >= targetHeight)
              {
                  return false;
              }

              if (!matrixInitialized)
              {
                  const bool hasDimensionHeader = !row.empty()
                                                  && startsWithDimensionHeader(row.front().value)
                                                  && std::all_of(row.begin() + 1,
                                                                 row.end(),
                                                                 [](const IO::CSVCell& cell)
                                                                 { return cell.value.empty(); });
                  if (hasDimensionHeader)
                  {
                      unsigned int headerWidth = 0;
                      unsigned int headerHeight = 0;
                      if (!parseDimensionHeader(row.front().value, headerWidth, headerHeight)
                          || !headerWidth || !headerHeight
                          || !validDimensions(headerWidth, headerHeight))
                      {
                          logs.warning() << filename << ": invalid matrix header";
                          result = false;
                          return false;
                      }

                      if (widthPolicy == WidthPolicy::Exact
                          && (headerWidth != minWidth || headerHeight != height))
                      {
                          logs.warning() << filename
                                         << ": matrix dimensions do not match the "
                                            "requested dimensions";
                          result = false;
                          return false;
                      }

                      if (widthPolicy == WidthPolicy::AtLeast && headerWidth < minWidth)
                      {
                          logs.warning() << filename
                                         << ": matrix header width is smaller than "
                                            "the requested minimum";
                          result = false;
                          return false;
                      }

                      targetWidth = headerWidth;
                      targetHeight = headerHeight;
                      matrix.resize(targetWidth, targetHeight);
                      matrixInitialized = true;
                      return true;
                  }

                  if (row.size() > Matrix::kMaxDimension)
                  {
                      logs.warning() << filename
                                     << ": matrix width exceeds the "
                                        "supported limit";
                      result = false;
                      return false;
                  }

                  targetWidth = widthPolicy == WidthPolicy::Exact
                                  ? minWidth
                                  : std::max<unsigned int>(minWidth, row.size());
                  if (!validDimensions(targetWidth, targetHeight))
                  {
                      logs.warning() << filename
                                     << ": matrix dimensions exceed the "
                                        "supported limit";
                      result = false;
                      return false;
                  }

                  matrix.resize(targetWidth, targetHeight);
                  matrixInitialized = true;
              }

              unsigned int column = 0;
              for (const auto& cell: row)
              {
                  if (column >= targetWidth)
                  {
                      result = false;
                      logs.warning() << filename << ": too many columns on row " << rowCount;
                      break;
                  }

                  if (cell.value.empty())
                  {
                      matrix[column][rowCount] = 0.;
                  }
                  else
                  {
                      double value = 0.;
                      if (!parseDouble(cell.value, value))
                      {
                          handleInvalidNumericValue(column, rowCount, cell.value);
                      }
                      else
                      {
                          matrix[column][rowCount] = value;
                      }
                  }
                  ++column;
              }

              if (column < targetWidth)
              {
                  result = false;
                  logs.warning() << filename << ": not enough columns on row " << rowCount
                                 << " (expected " << targetWidth << ", got " << column << ')';
              }

              ++rowCount;
              return rowCount < targetHeight;
          });
    }
    catch (const std::bad_alloc& exception)
    {
        logs.error() << "I/O Error: failed to allocate matrix for '" << filename
                     << "': " << exception.what();
        matrix.clear();
        return false;
    }
    catch (const std::exception& exception)
    {
        logs.error() << "I/O Error: failed to load '" << filename << "': " << exception.what();
        resetToRequestedDimensions();
        return false;
    }

    if (!matrixInitialized || rowCount < targetHeight)
    {
        result = false;
        logs.warning() << filename << ": not enough rows (expected " << targetHeight << ", got "
                       << rowCount << ')';
    }

    if (!result)
    {
        resetToRequestedDimensions();
    }

    return result;
}

inline bool load(Matrix& matrix,
                 const std::filesystem::path& filename,
                 unsigned int minWidth,
                 unsigned int height,
                 WidthPolicy widthPolicy = WidthPolicy::AtLeast)
{
    return load(matrix, filename.string(), minWidth, height, widthPolicy);
}

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
    serialized.reserve(matrix.width() * matrix.height() * 6);
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
    logs.debug() << "  :: writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';

    std::string data;
    if (matrix.width() && matrix.height())
    {
        saveToBuffer(matrix, data, precision, printDimensions, predicate, saveEvenIfAllZero);
    }

    if (data.size() > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
    {
        logs.error() << "I/O error: " << filename << ": Failed to write the file";
        return false;
    }

    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
    {
        logs.error() << "I/O error: " << filename
                     << ": Impossible to write the file (not enough permission ?)";
        return false;
    }

    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    file.close();
    if (!file)
    {
        logs.error() << "I/O error: " << filename << ": Failed to write the file";
        return false;
    }

    Statistics::HasWrittenToDisk(data.size());
    logs.debug() << "  :: [end] writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';
    return true;
}
} // namespace Antares::MatrixIO

#endif
