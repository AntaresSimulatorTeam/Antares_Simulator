// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_HXX
#define ANTARES_ARRAY_MATRIX_IO_HXX

#include <cassert>
#include <charconv>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <string>
#include <string_view>

#include <antares/io/CsvReader.h>
#include <antares/io/statistics.h>
#include <antares/logs/logs.h>

#include "matrix-to-buffer.h"
#include "matrix.h"

namespace Antares::MatrixIO
{
namespace
{
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
} // namespace

inline bool load(Matrix& matrix,
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

    const auto handleInvalidNumericValue =
      [&](unsigned int cellX, unsigned int cellY, const std::string& cell)
    {
        result = false;
        if (errorCount > 0)
        {
            logs.warning() << '`' << filename << "`: Invalid numeric value (x:" << cellX
                           << ",y:" << cellY << ", text: `" << cell << '`';
            if (!(--errorCount))
            {
                logs.warning() << " ... (skipped)";
            }
        }
        matrix[cellX][cellY] = 0.;
    };

    try
    {
        const auto rows = IO::readCSVFile(filename, "\t;,");
        for (const auto& row: rows)
        {
            if (rowCount >= matrix.height())
            {
                break;
            }

            // Time-series files use one as the minimum width and store their actual width in
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

                if (cell.empty())
                {
                    matrix[column][rowCount] = 0.;
                }
                else
                {
                    double value = 0.;
                    if (!parseDouble(cell, value))
                    {
                        handleInvalidNumericValue(column, rowCount, cell);
                    }
                    else
                    {
                        matrix[column][rowCount] = value;
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
        }
    }
    catch (const std::exception& exception)
    {
        logs.error() << "I/O Error: failed to load '" << filename << "': " << exception.what();
        matrix.reset(minWidth, height);
        return false;
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

inline bool load(Matrix& matrix,
                 const std::filesystem::path& filename,
                 unsigned int minWidth,
                 unsigned int height)
{
    return load(matrix, filename.string(), minWidth, height);
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
