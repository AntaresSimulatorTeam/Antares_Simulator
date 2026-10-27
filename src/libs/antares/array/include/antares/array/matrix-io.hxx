// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_HXX
#define ANTARES_ARRAY_MATRIX_IO_HXX

#include <cassert>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <antares/io/CsvReader.h>
#include <antares/io/statistics.h>
#include <antares/logs/logs.h>

#include "matrix-to-buffer.h"
#include "matrix.h"

namespace Antares::MatrixIO
{
namespace // anonymous
{
template<class T>
concept MatrixFastIntegral = std::is_integral_v<T> && !std::is_same_v<T, bool>
                             && !std::is_same_v<T, char> && !std::is_same_v<T, signed char>
                             && !std::is_same_v<T, unsigned char> && !std::is_same_v<T, wchar_t>
                             && !std::is_same_v<T, char8_t> && !std::is_same_v<T, char16_t>
                             && !std::is_same_v<T, char32_t>
                             && requires(const char* first, const char* last, T& value) {
                                    std::from_chars(first, last, value);
                                };

template<class T>
class MatrixStringConverter final
{
public:
    static bool Do(std::string_view str, T& out)
    {
        std::istringstream stream{std::string(str)};
        stream >> out;
        stream >> std::ws;
        return !stream.fail() && stream.eof();
    }
};

template<class T>
requires MatrixFastIntegral<T>
class MatrixStringConverter<T> final
{
public:
    static bool Do(std::string_view str, T& out)
    {
        const auto first = str.data();
        const auto last = first + str.size();
        const auto [end, error] = std::from_chars(first, last, out);
        if (error == std::errc{} && end == last)
        {
            return true;
        }

        // Keep the legacy behavior for signs, whitespace, and other values accepted by streams.
        std::istringstream stream{std::string(str)};
        stream >> out;
        stream >> std::ws;
        return !stream.fail() && stream.eof();
    }
};

template<>
class MatrixStringConverter<double> final
{
public:
    static bool Do(std::string_view str, double& out)
    {
        const auto first = str.data();
        const auto last = first + str.size();
        const auto [end, error] = std::from_chars(first, last, out);
        if (error == std::errc{} && end == last)
        {
            return true;
        }

        return DoLegacy(str, out);
    }

private:
    static bool DoLegacy(std::string_view str, double& out)
    {
        const std::string value(str);
        char* end = nullptr;
        out = ::strtod(value.c_str(), &end);
        return end != nullptr && *end == '\0';
    }
};

template<>
class MatrixStringConverter<float> final
{
public:
    static bool Do(std::string_view str, float& out)
    {
        const auto first = str.data();
        const auto last = first + str.size();
        const auto [end, error] = std::from_chars(first, last, out);
        if (error == std::errc{} && end == last)
        {
            return true;
        }

        return DoLegacy(str, out);
    }

private:
    static bool DoLegacy(std::string_view str, float& out)
    {
        const std::string value(str);
        char* end = nullptr;
        out = static_cast<float>(::strtod(value.c_str(), &end));
        return end != nullptr && *end == '\0';
    }
};

template<>
class MatrixStringConverter<std::string> final
{
public:
    static bool Do(std::string_view str, std::string& out)
    {
        out.assign(str);
        return true;
    }
};
} // anonymous namespace

template<class T>
bool load(Matrix<T>& matrix,
          const std::string& filename,
          unsigned int minWidth,
          unsigned int height)
{
    assert(!filename.empty());

    std::error_code error;
    const auto size = std::filesystem::file_size(filename, error);
    if (error)
    {
        logs.error() << "I/O Error: not found: '" << filename << "'";
        matrix.reset(minWidth, height);
        return false;
    }

    matrix.reset(minWidth, height);
    if (size == 0)
    {
        return true;
    }

    if (!minWidth || !height)
    {
        logs.error() << filename << ": matrix dimensions are required for a non-empty file";
        return false;
    }

    Statistics::HasReadFromDisk(size);
    logs.debug() << "  :: loading `" << filename << "`";

    unsigned int rowCount = 0;
    bool result = true;
    int errorCount = 6;
    T cellValue{};

    const auto handleInvalidNumericValue =
      [&](unsigned int cellX, unsigned int cellY, const IO::CSVCell& cell)
    {
        result = false;
        if (errorCount > 0)
        {
            logs.warning() << '`' << filename << "`: Invalid numeric value (x:" << cellX
                           << ",y:" << cellY << ", text: `" << std::string(cell.value) << '`';
            if (!(--errorCount))
            {
                logs.warning() << " ... (skipped)";
            }
        }
        matrix[cellX][cellY] = T{};
    };

    bool parsed = false;
    try
    {
        parsed = IO::readCSVFile(
          filename,
          "\t;,",
          [&](std::size_t, IO::CSVRow row)
          {
              if (rowCount >= matrix.height())
              {
                  return true;
              }

              // Time series files use one as the minimum width and store their actual width in
              // the first row. Resize once before storing that row.
              if (rowCount == 0 && row.size() > matrix.width())
              {
                  matrix.resize(static_cast<unsigned int>(row.size()), matrix.height());
              }

              unsigned int column = 0;
              for (const auto& cell: row)
              {
                  if (column >= matrix.width())
                  {
                      result = false;
                      logs.warning() << filename << ": too many columns on row " << rowCount;
                      break;
                  }

                  if (cell.value.empty())
                  {
                      matrix[column][rowCount] = T{};
                  }
                  else if constexpr (std::is_same_v<T, std::string>)
                  {
                      matrix[column][rowCount].assign(cell.value);
                  }
                  else
                  {
                      if (!MatrixStringConverter<T>::Do(cell.value, cellValue))
                      {
                          double fallback = 0;
                          if (!MatrixStringConverter<double>::Do(cell.value, fallback))
                          {
                              handleInvalidNumericValue(column, rowCount, cell);
                          }
                          else if constexpr (std::is_integral_v<T>)
                          {
                              if (!std::isfinite(fallback)
                                  || fallback < static_cast<double>(std::numeric_limits<T>::min())
                                  || (std::numeric_limits<T>::digits
                                          > std::numeric_limits<double>::digits
                                        ? fallback
                                            >= static_cast<double>(std::numeric_limits<T>::max())
                                        : fallback
                                            > static_cast<double>(std::numeric_limits<T>::max())))
                              {
                                  handleInvalidNumericValue(column, rowCount, cell);
                              }
                              else
                              {
                                  matrix[column][rowCount] = static_cast<T>(std::trunc(fallback));
                              }
                          }
                          else
                          {
                              matrix[column][rowCount] = static_cast<T>(fallback);
                          }
                      }
                      else
                      {
                          matrix[column][rowCount] = cellValue;
                      }
                  }
                  ++column;
              }

              if (column < matrix.width())
              {
                  result = false;
                  logs.warning() << filename << ": not enough columns on row " << rowCount
                                 << " (expected " << matrix.width() << ", got " << column << ')';
              }

              ++rowCount;
              return true;
          });
    }
    catch (const std::exception& exception)
    {
        logs.error() << "I/O Error: failed to load '" << filename << "': " << exception.what();
        matrix.reset(minWidth, height);
        return false;
    }

    if (!parsed)
    {
        result = false;
    }

    if (rowCount < matrix.height())
    {
        result = false;
        logs.warning() << filename << ": not enough rows (expected " << matrix.height() << ", got "
                       << rowCount << ')';
    }

    if (!result)
    {
        matrix.reset(minWidth, height);
    }

    return result;
}

template<class T>
bool load(Matrix<T>& matrix,
          const std::filesystem::path& filename,
          unsigned int minWidth,
          unsigned int height)
{
    return load(matrix, filename.string(), minWidth, height);
}

template<class T, class Predicate = std::identity>
void saveToBuffer(const Matrix<T>& matrix,
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
    const bool isDecimal = std::is_floating_point_v<T>;

    serialized.reserve(matrix.width() * matrix.height() * 6);
    if (printDimensions)
    {
        serialized += "size:" + std::to_string(matrix.width()) + 'x'
                      + std::to_string(matrix.height()) + '\n';
    }
    matrixToBuffer(matrix, serialized, predicate, isDecimal, precision);
    data = std::move(serialized);
}

template<class T, class Predicate = std::identity>
bool save(const Matrix<T>& matrix,
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
