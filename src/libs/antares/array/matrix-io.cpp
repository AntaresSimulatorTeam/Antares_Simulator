// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <charconv>
#include <fstream>
#include <limits>
#include <string_view>

#include <antares/array/matrix-io.h>
#include <antares/io/CsvReader.h>
#include <antares/io/statistics.h>
#include <antares/logs/logs.h>

namespace Antares::MatrixIO
{
namespace
{
constexpr std::uintmax_t kMaxMatrixFileSize = 1536ULL * 1024ULL * 1024ULL;

bool validDimensions(unsigned int width, unsigned int height)
{
    return width <= Matrix::kMaxDimension && height <= Matrix::kMaxDimension;
}

bool parseHeader(std::string_view text, unsigned int& width, unsigned int& height)
{
    const auto first = text.find_first_not_of(" \t");
    if (first == std::string_view::npos)
    {
        return false;
    }
    text = text.substr(first, text.find_last_not_of(" \t") - first + 1);
    if (!text.starts_with("size:"))
    {
        return false;
    }
    text.remove_prefix(5);
    const auto separator = text.find('x');
    if (separator == std::string_view::npos)
    {
        return false;
    }
    const auto parseUnsigned = [](std::string_view input, unsigned int& output)
    {
        const auto [end, error] = std::from_chars(input.data(),
                                                  input.data() + input.size(),
                                                  output);
        return error == std::errc{} && end == input.data() + input.size();
    };
    return parseUnsigned(text.substr(0, separator), width)
           && parseUnsigned(text.substr(separator + 1), height);
}

// Trailing tabs historically acted as padding rather than extra columns.
// Keep cells required by the requested or already established width.
IO::CSVRow trimTabPadding(IO::CSVRow row, std::size_t minimumWidth)
{
    while (row.size() > minimumWidth && row.size() > 1
           && row.back().value.find_first_not_of(" \t") == std::string_view::npos
           && row[row.size() - 2].separator == '\t')
    {
        row = row.first(row.size() - 1);
    }
    return row;
}
} // namespace

std::optional<LoadError> loadDetailed(Matrix& destination,
                                      const std::filesystem::path& filename,
                                      const LoadOptions& options)
{
    const auto& [minWidth, height, widthPolicy, rowPolicy, minHeaderHeight] = options;
    auto failure = [&](std::string message,
                       std::optional<std::size_t> row = {},
                       std::optional<std::size_t> column = {}) -> std::optional<LoadError>
    { return LoadError{filename, row, column, std::move(message)}; };
    if (!validDimensions(minWidth, height) || minHeaderHeight > height)
    {
        return failure("matrix dimensions exceed the supported limit");
    }
    std::error_code error;
    const auto size = std::filesystem::file_size(filename, error);
    if (error)
    {
        return failure("cannot read file: " + error.message());
    }
    if (size > kMaxMatrixFileSize)
    {
        return failure("matrix file is too large (maximum 1536 MiB)");
    }

    Matrix matrix;
    try
    {
        if (size == 0)
        {
            matrix.reset(minWidth, height);
            destination.swap(matrix);
            return {};
        }
        if (!minWidth || !height)
        {
            return failure("matrix dimensions are required for a non-empty file");
        }

        Statistics::HasReadFromDisk(size);
        unsigned int rowCount = 0;
        unsigned int targetWidth = 0;
        unsigned int targetHeight = height;
        bool initialized = false;
        bool headerControlsHeight = false;
        std::optional<LoadError> result;

        IO::readCSVFile(
          filename,
          "\t;,",
          [&](std::size_t fileRow, IO::CSVRow row)
          {
              if (initialized && rowCount >= targetHeight)
              {
                  result = failure("too many rows", fileRow + 1);
                  return false;
              }
              if (!initialized)
              {
                  auto firstCell = row.front().value;
                  const auto first = firstCell.find_first_not_of(" \t");
                  const bool hasHeader = first != std::string_view::npos
                                         && firstCell.substr(first).starts_with("size:");
                  if (hasHeader)
                  {
                      row = trimTabPadding(row, 1);
                      unsigned int headerWidth = 0;
                      unsigned int headerHeight = 0;
                      if (row.size() != 1 || !parseHeader(firstCell, headerWidth, headerHeight)
                          || !headerWidth || !headerHeight
                          || !validDimensions(headerWidth, headerHeight))
                      {
                          result = failure("invalid matrix header", fileRow + 1);
                          return false;
                      }
                      if ((widthPolicy == WidthPolicy::Exact && headerWidth != minWidth)
                          || (widthPolicy == WidthPolicy::AtLeast && headerWidth < minWidth)
                          || (rowPolicy == RowPolicy::Exact && headerHeight != height)
                          || (rowPolicy == RowPolicy::Prefix && headerHeight < height)
                          || (rowPolicy == RowPolicy::HeaderOrPrefix
                              && (headerHeight < minHeaderHeight || headerHeight > height)))
                      {
                          result = failure("matrix header dimensions do not match request",
                                           fileRow + 1);
                          return false;
                      }
                      targetWidth = headerWidth;
                      if (rowPolicy == RowPolicy::HeaderOrPrefix
                          || rowPolicy == RowPolicy::HeaderHeightOrPrefix)
                      {
                          targetHeight = headerHeight;
                          headerControlsHeight = rowPolicy == RowPolicy::HeaderOrPrefix;
                      }
                      matrix.resize(targetWidth, targetHeight);
                      initialized = true;
                      return true;
                  }
                  if (row.size() > Matrix::kMaxDimension)
                  {
                      result = failure("matrix width exceeds the supported limit", fileRow + 1);
                      return false;
                  }
                  row = trimTabPadding(row, minWidth);
                  targetWidth = widthPolicy == WidthPolicy::Exact
                                  ? minWidth
                                  : std::max<unsigned int>(minWidth, row.size());
                  matrix.resize(targetWidth, height);
                  initialized = true;
              }
              row = trimTabPadding(row, targetWidth);
              if (row.size() != targetWidth)
              {
                  result = failure("expected " + std::to_string(targetWidth) + " columns, got "
                                     + std::to_string(row.size()),
                                   fileRow + 1);
                  return false;
              }
              for (std::size_t column = 0; column < row.size(); ++column)
              {
                  if (!row[column].value.empty())
                  {
                      double value = 0.;
                      if (!IO::parseCSVDouble(row[column].value,
                                              value,
                                              IO::CSVDoublePolicy::AllowInfinity))
                      {
                          result = failure("invalid numeric value", fileRow + 1, column + 1);
                          return false;
                      }
                      matrix[column][rowCount] = value;
                  }
              }
              ++rowCount;
              return rowPolicy == RowPolicy::Exact || headerControlsHeight
                     || rowCount < targetHeight;
          },
          Matrix::kMaxDimension + 1);

        if (result)
        {
            return result;
        }
        if (!initialized || rowCount < targetHeight)
        {
            return failure("expected " + std::to_string(targetHeight) + " rows, got "
                           + std::to_string(rowCount));
        }
        destination.swap(matrix);
        return {};
    }
    catch (const std::exception& exception)
    {
        return failure(exception.what());
    }
}

bool load(Matrix& matrix, const std::filesystem::path& filename, const LoadOptions& options)
{
    const auto error = loadDetailed(matrix, filename, options);
    if (!error)
    {
        return true;
    }
    auto message = error->filename.string();
    if (error->row)
    {
        message += ":" + std::to_string(*error->row);
    }
    if (error->column)
    {
        message += ":" + std::to_string(*error->column);
    }
    logs.error() << message << ": " << error->message;
    return false;
}

bool load(Matrix& matrix, const std::string& filename, const LoadOptions& options)
{
    return load(matrix, std::filesystem::path(filename), options);
}

bool loadOrReset(Matrix& matrix, const std::filesystem::path& filename, const LoadOptions& options)
{
    if (load(matrix, filename, options))
    {
        return true;
    }
    matrix.reset(options.minWidth, options.height);
    return false;
}

void logSerializationError(const std::string& filename, const std::string& message)
{
    logs.error() << "I/O error: " << filename << ": " << message;
}

bool writeBuffer(const std::string& filename, const std::string& data)
{
    if (data.size() > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
    {
        logs.error() << "I/O error: " << filename << ": Failed to write the file";
        return false;
    }
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
    {
        logs.error() << "I/O error: " << filename << ": Failed to open the file";
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
    return true;
}
} // namespace Antares::MatrixIO
