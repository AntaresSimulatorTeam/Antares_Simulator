// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_HXX
#define ANTARES_ARRAY_MATRIX_IO_HXX

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <antares/io/CsvReader.h>
#include <antares/io/statistics.h>
#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

#include "matrix-to-buffer.h"

namespace Antares::MatrixIO
{
using BufferType = std::string;

enum class FileLoadError
{
    none,
    notFound,
    tooLarge,
    failed,
};

using FileLoader = std::function<FileLoadError(BufferType&, const std::string&)>;

template<class T>
using MatrixType = Matrix<T>;

namespace // anonymous
{
constexpr std::uintmax_t matrixFileSizeLimit = 1536ULL * 1024ULL * 1024ULL;

template<class T>
class MatrixStringConverter final
{
public:
    static bool Do(const std::string& str, T& out)
    {
        std::istringstream stream(str);
        stream >> out;
        stream >> std::ws;
        return !stream.fail() && stream.eof();
    }
};

template<>
class MatrixStringConverter<double> final
{
public:
    static bool Do(const std::string& str, double& out)
    {
        char* end = nullptr;
        out = ::strtod(str.c_str(), &end);
        return end != nullptr && *end == '\0';
    }
};

template<>
class MatrixStringConverter<float> final
{
public:
    static bool Do(const std::string& str, float& out)
    {
        char* end = nullptr;
        out = static_cast<float>(::strtod(str.c_str(), &end));
        return end != nullptr && *end == '\0';
    }
};

template<>
class MatrixStringConverter<std::string> final
{
public:
    static bool Do(const std::string& str, std::string& out)
    {
        out.assign(str);
        return true;
    }
};

bool detectEncoding(const std::string& filename, const std::string& data, size_t& offset)
{
    if (data.size() > 1)
    {
        if (static_cast<unsigned char>(data[0]) == 0xFE
            && static_cast<unsigned char>(data[1]) == 0xFF)
        {
            if (data.size() > 3 && data[2] == 0 && data[3] == 0)
            {
                logs.error() << '`' << filename
                             << "`: UTF-32 Little Endian encoding detected. ASCII/UTF-8 required.";
            }
            else
            {
                logs.error() << '`' << filename
                             << "`: UTF-16 Big Endian encoding detected. ASCII/UTF-8 required.";
            }
            return false;
        }

        if (static_cast<unsigned char>(data[0]) == 0xFF
            && static_cast<unsigned char>(data[1]) == 0xFE)
        {
            logs.error() << '`' << filename
                         << "`: UTF-16 Little Endian encoding detected. ASCII/UTF-8 required.";
            return false;
        }

        if (data.size() > 2 && static_cast<unsigned char>(data[0]) == 0xEF
            && static_cast<unsigned char>(data[1]) == 0xBB
            && static_cast<unsigned char>(data[2]) == 0xBF)
        {
            offset = 3;
        }

        if (data.size() > 3 && data[0] == 0 && data[1] == 0
            && static_cast<unsigned char>(data[2]) == 0xFE
            && static_cast<unsigned char>(data[3]) == 0xFF)
        {
            logs.error() << '`' << filename
                         << "`: UTF-32 Big Endian encoding detected. ASCII/UTF-8 required.";
            return false;
        }
    }
    return true;
}

std::size_t matrixWidth(IO::CSVRow row)
{
    auto width = row.size();
    while (width > 0 && row[width - 1].value.empty())
    {
        if (width == 1)
        {
            if (row[0].separator != '\t')
            {
                break;
            }
        }
        else if (row[width - 2].separator != '\t')
        {
            break;
        }
        --width;
    }
    return width;
}

template<class T>
bool loadFromCSV(Matrix<T>& matrix,
                 const std::string& filename,
                 const std::string& data,
                 unsigned int minWidth,
                 unsigned int maxHeight,
                 bool fixedSize,
                 unsigned int options)
{
    logs.debug() << "  :: loading `" << filename << "`";

    size_t bom = 0;
    if (!detectEncoding(filename, data, bom))
    {
        matrix.reset(minWidth > 0 ? minWidth : 1, maxHeight);
        return false;
    }

    const std::string_view csvData(data.data() + bom, data.size() - bom);
    constexpr std::string_view separators = "\t;,";
    std::string_view dataToRead = csvData;
    std::size_t dataOffset = bom;
    bool hasHeader = false;

    if (fixedSize)
    {
        matrix.reset(minWidth, maxHeight);
    }
    else
    {
        const auto newlinePosition = csvData.find_first_of("\r\n");
        const auto firstLineEnd = newlinePosition == std::string_view::npos ? csvData.size()
                                                                            : newlinePosition;
        if (!csvData.empty())
        {
            auto headerEnd = firstLineEnd;
            while (headerEnd > 0
                   && (csvData[headerEnd - 1] == '\r' || csvData[headerEnd - 1] == '\t'
                       || csvData[headerEnd - 1] == ' '))
            {
                --headerEnd;
            }

            hasHeader = headerEnd > 7 && csvData.compare(0, 5, "size:") == 0;
            if (hasHeader)
            {
                const std::string header(csvData.substr(0, headerEnd));
                int headerWidth = 0;
                int headerHeight = 0;
#ifdef _MSC_VER
                const int parsed = sscanf_s(header.c_str(),
                                            "size:%dx%d",
                                            &headerWidth,
                                            &headerHeight);
#else
                const int parsed = sscanf(header.c_str(),
                                          "size:%dx%d",
                                          &headerWidth,
                                          &headerHeight);
#endif
                if (parsed != 2)
                {
                    headerWidth = 0;
                    headerHeight = 0;
                }

                if (headerWidth < 1)
                {
                    if (!(options & Matrix<T>::optQuiet))
                    {
                        logs.warning() << '`' << filename << "`: Invalid header";
                    }
                    headerWidth = 1;
                }
                if (headerHeight < 1)
                {
                    if (!(options & Matrix<T>::optQuiet))
                    {
                        logs.warning() << '`' << filename << "`: Invalid header";
                    }

                    if (!maxHeight)
                    {
                        unsigned int rows = 0;
                        const auto bodyStart = newlinePosition == std::string_view::npos
                                                 ? csvData.size()
                                                 : firstLineEnd + 1
                                                     + ((csvData[firstLineEnd] == '\r'
                                                         && firstLineEnd + 1 < csvData.size()
                                                         && csvData[firstLineEnd + 1] == '\n')
                                                          ? 1
                                                          : 0);
                        IO::readCSV(csvData.substr(bodyStart),
                                    separators,
                                    [&rows](std::size_t, IO::CSVRow)
                                    {
                                        ++rows;
                                        return true;
                                    });
                        maxHeight = rows;
                    }
                    headerHeight = static_cast<int>(maxHeight);
                }
                maxHeight = static_cast<unsigned int>(headerHeight);
                matrix.resize(static_cast<unsigned int>(headerWidth), maxHeight);

                auto bodyStart = newlinePosition == std::string_view::npos ? csvData.size()
                                                                           : firstLineEnd + 1;
                if (newlinePosition != std::string_view::npos && csvData[firstLineEnd] == '\r'
                    && bodyStart < csvData.size() && csvData[bodyStart] == '\n')
                {
                    ++bodyStart;
                }
                dataToRead = csvData.substr(bodyStart);
                dataOffset += bodyStart;
            }
        }

        if (!hasHeader)
        {
            unsigned int width = 0;
            unsigned int rows = 0;
            IO::readCSV(dataToRead,
                        separators,
                        [&width, &rows, maxHeight](std::size_t rowIndex, IO::CSVRow row)
                        {
                            if (rowIndex == 0)
                            {
                                width = static_cast<unsigned int>(matrixWidth(row));
                            }
                            ++rows;
                            return maxHeight == 0;
                        });

            if (!maxHeight)
            {
                maxHeight = rows;
            }

            matrix.resize(std::max(width, minWidth), maxHeight);
            if (!width)
            {
                if (!(options & Matrix<T>::optQuiet) && !(options & Matrix<T>::optNoWarnIfEmpty))
                {
                    logs.warning() << '`' << filename << "`: Invalid format: The file seems empty";
                }
                matrix.zero();
                return false;
            }
        }
    }

    unsigned int y = 0;
    int errorCount = 6;
    std::string converter;
    T cellValue{};
    bool result = true;

    const auto handleInvalidNumericValue =
      [&](unsigned int cellX, unsigned int cellY, const IO::CSVCell& cell)
    {
        result = false;
        if (!(options & Matrix<T>::optQuiet) && errorCount)
        {
            logs.warning() << '`' << filename << "`: Invalid numeric value (x:" << cellX
                           << ",y:" << cellY << ", offset: " << dataOffset + cell.offset
                           << "byte), text: `" << std::string(cell.value)
                           << " read:" << matrix[cellX][cellY] << '`';
            if (!(--errorCount))
            {
                logs.warning() << " ... (skipped)";
            }
        }
        matrix[cellX][cellY] = T{};
    };

    const bool parsed = IO::readCSV(
      dataToRead,
      separators,
      [&](std::size_t, IO::CSVRow row)
      {
          if (y >= matrix.height())
          {
              return true;
          }

          if (options & Matrix<T>::optNeverFails)
          {
              const auto requiredWidth = matrixWidth(row);
              if (requiredWidth > matrix.width())
              {
                  matrix.resizeWithoutDataLost(static_cast<unsigned int>(requiredWidth),
                                               matrix.height());
              }
          }

          unsigned int x = 0;
          for (const auto& cell: row)
          {
              if (x >= matrix.width())
              {
                  if (cell.value.empty())
                  {
                      ++x;
                      continue;
                  }

                  if (options & Matrix<T>::optNeverFails)
                  {
                      assert(x < matrix.width());
                  }
                  else
                  {
                      result = false;
                      if (!(options & Matrix<T>::optQuiet) && errorCount > 0)
                      {
                          logs.warning() << '`' << filename
                                         << "`: Invalid format: Too many columns_ for the row " << y
                                         << " (offset: " << dataOffset + cell.offset << "byte)";
                          if (!(--errorCount))
                          {
                              logs.warning() << " ... (skipped)";
                          }
                      }
                      break;
                  }
              }

              if (cell.value.empty())
              {
                  matrix[x][y] = T{};
                  if (!(options & Matrix<T>::optQuiet))
                  {
                      logs.debug() << "  empty value at " << (x + 1) << 'x' << (y + 1);
                  }
              }
              else if constexpr (std::is_same_v<T, std::string>)
              {
                  matrix[x][y].assign(cell.value);
              }
              else
              {
                  converter.assign(cell.value);
                  if (!MatrixStringConverter<T>::Do(converter, cellValue))
                  {
                      double fallback = 0;
                      if (!MatrixStringConverter<double>::Do(converter, fallback))
                      {
                          handleInvalidNumericValue(x, y, cell);
                      }
                      else
                      {
                          if constexpr (std::is_integral_v<T>)
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
                                  handleInvalidNumericValue(x, y, cell);
                              }
                              else
                              {
                                  matrix[x][y] = static_cast<T>(std::trunc(fallback));
                              }
                          }
                          else
                          {
                              matrix[x][y] = static_cast<T>(fallback);
                          }
                      }
                  }
                  else
                  {
                      matrix[x][y] = cellValue;
                  }
              }
              ++x;
          }

          if (x < matrix.width())
          {
              if (!(options & Matrix<T>::optNeverFails))
              {
                  result = false;
                  if (!(options & Matrix<T>::optQuiet) && errorCount)
                  {
                      logs.warning()
                        << filename << ": at line " << (y + 1) << ", not enough columns (expected "
                        << matrix.width() << ", got " << x << ')';
                      if (!(--errorCount))
                      {
                          logs.warning() << " ... (skipped)";
                      }
                  }
              }
              while (x < matrix.width())
              {
                  matrix[x][y] = T{};
                  ++x;
              }
          }

          ++y;
          return true;
      });

    if (!parsed)
    {
        result = false;
    }

    if (y < matrix.height())
    {
        result = false;
        if (!(options & Matrix<T>::optQuiet))
        {
            logs.warning() << filename << ": not enough rows (expected " << matrix.height()
                           << ", got " << y << ')';
        }
        while (y < matrix.height())
        {
            for (unsigned int x = 0; x < matrix.width(); ++x)
            {
                matrix[x][y] = T{};
            }
            ++y;
        }
    }

    return (options & Matrix<T>::optNeverFails) ? true : result;
}

} // anonymous namespace

template<class T>
bool load(MatrixType<T>& matrix,
          const std::string& filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    assert(!filename.empty());

    BufferType owned;
    BufferType* input = buffer ? buffer : &owned;
    const bool readFromDisk = buffer == nullptr || static_cast<bool>(fileLoader);

    if (readFromDisk)
    {
        const auto error = fileLoader ? fileLoader(*input, filename) : [&input, &filename]
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file)
            {
                return FileLoadError::notFound;
            }

            std::error_code ec;
            const auto size = std::filesystem::file_size(filename, ec);
            if (ec)
            {
                return FileLoadError::failed;
            }

            if (size > matrixFileSizeLimit)
            {
                return FileLoadError::tooLarge;
            }

            input->resize(static_cast<std::size_t>(size));
            file.read(input->data(), static_cast<std::streamsize>(size));
            return file ? FileLoadError::none : FileLoadError::failed;
        }();
        if (error != FileLoadError::none)
        {
            if (!(options & MatrixType<T>::optQuiet))
            {
                if (error == FileLoadError::notFound)
                {
                    logs.error() << "I/O Error: not found: '" << filename << "'";
                }
                else if (error == FileLoadError::tooLarge)
                {
                    logs.error() << filename << ": The file is too large (>"
                                 << (matrixFileSizeLimit / 1024 / 1024) << "Mo)";
                }
                else
                {
                    logs.error() << "I/O Error: failed to load '" << filename << "'";
                }
            }
            matrix.reset(minWidth, maxHeight);
            return false;
        }
    }
    if (input->size() > matrixFileSizeLimit)
    {
        if (!(options & MatrixType<T>::optQuiet))
        {
            logs.error() << filename << ": The file is too large (>"
                         << (matrixFileSizeLimit / 1024 / 1024) << "Mo)";
        }
        matrix.reset(minWidth, maxHeight);
        return false;
    }

    if (input->empty())
    {
        if (minWidth && maxHeight)
        {
            matrix.reset(minWidth, maxHeight);
        }
        else
        {
            matrix.clear();
        }
        return true;
    }

    if (readFromDisk)
    {
        Statistics::HasReadFromDisk(input->size());
    }

    const bool result = loadFromCSV(matrix,
                                    filename,
                                    *input,
                                    minWidth,
                                    maxHeight,
                                    (options & MatrixType<T>::optFixedSize) != 0,
                                    options);
    if (!result)
    {
        matrix.reset(minWidth, maxHeight);
    }
    return result;
}

template<class T>
bool load(MatrixType<T>& matrix,
          const std::filesystem::path& filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, filename.string(), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T>
bool load(MatrixType<T>& matrix,
          const char* filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    if (!filename)
    {
        if (!(options & MatrixType<T>::optQuiet))
        {
            logs.error() << "I/O Error: null filename";
        }
        matrix.reset(minWidth, maxHeight);
        return false;
    }
    return load(matrix, std::string(filename), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T, class Predicate = std::identity>
void saveToBuffer(const MatrixType<T>& matrix,
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
bool save(const MatrixType<T>& matrix,
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

template<class T>
bool saveToCSVFile(const MatrixType<T>& matrix,
                   const std::string& filename,
                   unsigned int precision = 6,
                   bool printDimensions = false,
                   bool saveEvenIfAllZero = false)
{
    return save(matrix, filename, precision, printDimensions, std::identity{}, saveEvenIfAllZero);
}
} // namespace Antares::MatrixIO

#endif
