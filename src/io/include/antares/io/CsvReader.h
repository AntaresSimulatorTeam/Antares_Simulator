// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <cstddef>
#include <filesystem>
#include <functional>
#include <span>
#include <string_view>

namespace Antares::IO
{
struct CSVCell final
{
    std::string_view value;
};

using CSVRow = std::span<const CSVCell>;
using CSVRowCallback = std::function<bool(std::size_t, CSVRow)>;

/**
 * Read CSV rows from an in-memory buffer.
 *
 * Separators are not included in cell values. Empty cells are preserved,
 * including cells created by trailing separators. Newline characters are
 * treated as row separators and CRLF is handled as one newline.
 *
 * The callback is invoked while the row storage is valid. Returning false
 * stops reading and makes this function return false.
 */
bool readCSV(std::string_view data, std::string_view separators, const CSVRowCallback& callback);

/**
 * Read CSV rows directly from a file.
 *
 * The file is memory-mapped and remains mapped for the duration of the
 * callback invocations.
 */
bool readCSVFile(const std::filesystem::path& filename,
                 std::string_view separators,
                 const CSVRowCallback& callback);
} // namespace Antares::IO
