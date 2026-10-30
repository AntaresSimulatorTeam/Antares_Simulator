// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - lib - antares - matrix load

#include "tests-matrix-load.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include <boost/test/unit_test.hpp>

namespace
{
struct TemporaryFile final
{
    explicit TemporaryFile(std::filesystem::path filename):
        path(std::move(filename))
    {
    }

    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;

    TemporaryFile(TemporaryFile&& other) noexcept:
        path(std::move(other.path))
    {
    }

    ~TemporaryFile()
    {
        std::error_code error;
        std::filesystem::remove(path, error);
    }

    std::filesystem::path path;
};

TemporaryFile writeFile(std::string_view content)
{
    static unsigned int counter = 0;
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    TemporaryFile file(std::filesystem::temp_directory_path()
                       / ("antares-matrix-load-" + std::to_string(stamp) + '-'
                          + std::to_string(counter++) + ".txt"));

    std::ofstream output(file.path, std::ios::binary | std::ios::trunc);
    if (!output)
    {
        throw std::runtime_error("Unable to create matrix test file");
    }
    output.write(content.data(), static_cast<std::streamsize>(content.size()));
    if (!output)
    {
        throw std::runtime_error("Unable to write matrix test file");
    }
    return file;
}
} // namespace

BOOST_AUTO_TEST_CASE(loads_numeric_values_and_infers_variable_width)
{
    const auto file = writeFile("1.5\t-2\t3\n4\t5\t6\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(Antares::MatrixIO::load(matrix, file.path, 1, 2));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.5);
    BOOST_CHECK_EQUAL(matrix[1][0], -2.);
    BOOST_CHECK_EQUAL(matrix[2][0], 3.);
    BOOST_CHECK_EQUAL(matrix[0][1], 4.);
    BOOST_CHECK_EQUAL(matrix[2][1], 6.);
}

BOOST_AUTO_TEST_CASE(loads_comma_separated_values_and_empty_cells)
{
    const auto file = writeFile("1,,3\r\n4,5,6\r\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(Antares::MatrixIO::load(matrix, file.path, 1, 2));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[2][0], 3.);
    BOOST_CHECK_EQUAL(matrix[1][1], 5.);
}

BOOST_AUTO_TEST_CASE(reloading_overwrites_previous_values)
{
    const auto file = writeFile("1\t2\n3\t4\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(Antares::MatrixIO::load(matrix, file.path, 2, 2));

    {
        std::ofstream output(file.path, std::ios::binary | std::ios::trunc);
        output << "5\t\n\t8\n";
    }

    BOOST_REQUIRE(Antares::MatrixIO::load(matrix, file.path, 2, 2));
    BOOST_CHECK_EQUAL(matrix[0][0], 5.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[0][1], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 8.);
}

BOOST_AUTO_TEST_CASE(minimum_width_is_preserved)
{
    const auto file = writeFile("1\t2\t3\n4\t5\t6\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::load(matrix, file.path, 5, 2));
    BOOST_REQUIRE_EQUAL(matrix.width(), 5);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[3][0], 0.);
    BOOST_CHECK_EQUAL(matrix[4][1], 0.);
}

BOOST_AUTO_TEST_CASE(invalid_numeric_values_fail_and_are_zeroed)
{
    const auto file = writeFile("1\tnot-a-number\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::load(matrix, file.path, 2, 1));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 1);
    BOOST_CHECK_EQUAL(matrix[0][0], 0.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
}

BOOST_AUTO_TEST_CASE(incomplete_rows_fail_and_reset_matrix)
{
    const auto file = writeFile("1\t2\n3\t4\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::load(matrix, file.path, 3, 3));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);
    BOOST_REQUIRE_EQUAL(matrix.height(), 3);
    BOOST_CHECK_EQUAL(matrix[0][0], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 0.);
    BOOST_CHECK_EQUAL(matrix[2][2], 0.);
}

BOOST_AUTO_TEST_CASE(empty_files_initialize_requested_dimensions)
{
    const auto file = writeFile("");
    Antares::Matrix matrix;

    BOOST_CHECK(Antares::MatrixIO::load(matrix, file.path, 2, 2));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(missing_files_fail)
{
    Antares::Matrix matrix;
    const auto path = std::filesystem::temp_directory_path()
                      / "antares-matrix-file-does-not-exist.txt";

    BOOST_CHECK(!Antares::MatrixIO::load(matrix, path, 2, 2));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}
