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

struct LoadState
{
    Matrix matrix;
    unsigned int rowCount = 0;
    unsigned int targetWidth = 0;
    unsigned int targetHeight = 0;
    bool initialized = false;
    bool headerControlsHeight = false;
};

LoadError makeError(const std::filesystem::path& filename,
                    std::string message,
                    std::optional<std::size_t> row = {},
                    std::optional<std::size_t> column = {})
{
    return LoadError{filename, row, column, std::move(message)};
}

bool headerMatchesOptions(unsigned int headerWidth,
                          unsigned int headerHeight,
                          const LoadOptions& options)
{
    if (options.widthPolicy == WidthPolicy::Exact && headerWidth != options.minWidth)
    {
        return false;
    }
    if (options.widthPolicy == WidthPolicy::AtLeast && headerWidth < options.minWidth)
    {
        return false;
    }
    switch (options.rowPolicy)
    {
    case RowPolicy::Exact:
        return headerHeight == options.height;
    case RowPolicy::Prefix:
        return headerHeight >= options.height;
    case RowPolicy::HeaderOrPrefix:
        return headerHeight >= options.minHeaderHeight && headerHeight <= options.height;
    case RowPolicy::HeaderHeightOrPrefix:
        return true;
    }
    return false;
}

std::optional<LoadError> initializeFromHeader(LoadState& state,
                                              const std::filesystem::path& filename,
                                              const LoadOptions& options,
                                              std::size_t fileRow,
                                              IO::CSVRow row)
{
    const auto firstCell = row.front().value;
    row = trimTabPadding(row, 1);
    unsigned int headerWidth = 0;
    unsigned int headerHeight = 0;
    if (row.size() != 1 || !parseHeader(firstCell, headerWidth, headerHeight) || !headerWidth
        || !headerHeight || !validDimensions(headerWidth, headerHeight))
    {
        return makeError(filename, "invalid matrix header", fileRow + 1);
    }
    if (!headerMatchesOptions(headerWidth, headerHeight, options))
    {
        return makeError(filename, "matrix header dimensions do not match request", fileRow + 1);
    }
    state.targetWidth = headerWidth;
    if (options.rowPolicy == RowPolicy::HeaderOrPrefix
        || options.rowPolicy == RowPolicy::HeaderHeightOrPrefix)
    {
        state.targetHeight = headerHeight;
        state.headerControlsHeight = options.rowPolicy == RowPolicy::HeaderOrPrefix;
    }
    state.matrix.resize(state.targetWidth, state.targetHeight);
    state.initialized = true;
    return {};
}

std::optional<LoadError> parseDataRow(LoadState& state,
                                      const std::filesystem::path& filename,
                                      std::size_t fileRow,
                                      IO::CSVRow row)
{
    row = trimTabPadding(row, state.targetWidth);
    if (row.size() != state.targetWidth)
    {
        return makeError(filename,
                         "expected " + std::to_string(state.targetWidth) + " columns, got "
                           + std::to_string(row.size()),
                         fileRow + 1);
    }
    for (std::size_t column = 0; column < row.size(); ++column)
    {
        const auto cell = row[column].value;
        if (cell.find_first_not_of(" \t") == std::string_view::npos)
        {
            continue;
        }
        double value = 0.;
        if (!IO::parseCSVDouble(cell, value, IO::CSVDoublePolicy::AllowInfinity))
        {
            return makeError(filename, "invalid numeric value", fileRow + 1, column + 1);
        }
        state.matrix[column][state.rowCount] = value;
    }
    ++state.rowCount;
    return {};
}
} // namespace

std::optional<LoadError> loadDetailed(Matrix& destination,
                                      const std::filesystem::path& filename,
                                      const LoadOptions& options)
{
    if (!validDimensions(options.minWidth, options.height)
        || options.minHeaderHeight > options.height)
    {
        return makeError(filename, "matrix dimensions exceed the supported limit");
    }
    std::error_code error;
    const auto size = std::filesystem::file_size(filename, error);
    if (error)
    {
        return makeError(filename, "cannot read file: " + error.message());
    }
    if (size > kMaxMatrixFileSize)
    {
        return makeError(filename, "matrix file is too large (maximum 1536 MiB)");
    }

    try
    {
        if (size == 0)
        {
            Matrix matrix;
            matrix.reset(options.minWidth, options.height);
            destination.swap(matrix);
            return {};
        }
        if (!options.minWidth || !options.height)
        {
            return makeError(filename, "matrix dimensions are required for a non-empty file");
        }

        Statistics::HasReadFromDisk(size);
        LoadState state;
        state.targetHeight = options.height;
        std::optional<LoadError> result;

        IO::readCSVFile(
          filename,
          "\t;,",
          [&state, &filename, &options, &result](std::size_t fileRow, IO::CSVRow row)
          {
              if (state.initialized && state.rowCount >= state.targetHeight)
              {
                  result = makeError(filename, "too many rows", fileRow + 1);
                  return false;
              }
              if (!state.initialized)
              {
                  const auto firstCell = row.front().value;
                  const auto first = firstCell.find_first_not_of(" \t");
                  const bool hasHeader = first != std::string_view::npos
                                         && firstCell.substr(first).starts_with("size:");
                  if (hasHeader)
                  {
                      result = initializeFromHeader(state, filename, options, fileRow, row);
                      return !result.has_value();
                  }
                  if (row.size() > Matrix::kMaxDimension)
                  {
                      result = makeError(filename,
                                         "matrix width exceeds the supported limit",
                                         fileRow + 1);
                      return false;
                  }
                  row = trimTabPadding(row, options.minWidth);
                  state.targetWidth = options.widthPolicy == WidthPolicy::Exact
                                        ? options.minWidth
                                        : std::max<unsigned int>(options.minWidth, row.size());
                  state.matrix.resize(state.targetWidth, options.height);
                  state.initialized = true;
              }
              result = parseDataRow(state, filename, fileRow, row);
              if (result)
              {
                  return false;
              }
              return options.rowPolicy == RowPolicy::Exact || state.headerControlsHeight
                     || state.rowCount < state.targetHeight;
          },
          Matrix::kMaxDimension + 1);

        if (result)
        {
            return result;
        }
        if (!state.initialized || state.rowCount < state.targetHeight)
        {
            return makeError(filename,
                             "expected " + std::to_string(state.targetHeight) + " rows, got "
                               + std::to_string(state.rowCount));
        }
        destination.swap(state.matrix);
        return {};
    }
    catch (const std::exception& exception)
    {
        return makeError(filename, exception.what());
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
