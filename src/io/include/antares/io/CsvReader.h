// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <cstddef>
#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Antares::IO
{
struct CSVCell final
{
    std::string_view value;
};

using CSVRow = std::span<const CSVCell>;
using CSVRowCallback = std::function<bool(std::size_t, CSVRow)>;
using CSVTable = std::vector<std::vector<std::string>>;

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
 * Cell values are copied into the returned table, so they remain valid after
 * the file is closed and unmapped.
 */
CSVTable readCSVFile(const std::filesystem::path& filename, std::string_view separators);
} // namespace Antares::IO
