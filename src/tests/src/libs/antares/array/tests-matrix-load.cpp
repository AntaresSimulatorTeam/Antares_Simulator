// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - lib - antares - matrix load

#include "tests-matrix-load.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <unit_test_utils.h>
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

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.5);
    BOOST_CHECK_EQUAL(matrix[1][0], -2.);
    BOOST_CHECK_EQUAL(matrix[2][0], 3.);
    BOOST_CHECK_EQUAL(matrix[0][1], 4.);
    BOOST_CHECK_EQUAL(matrix[2][1], 6.);
}

BOOST_AUTO_TEST_CASE(loads_infinite_binding_constraint_bounds)
{
    const auto file = writeFile("0\t-inf\t0\ninf\t+infinity\t-INFINITY\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 3, .height = 2})));
    BOOST_CHECK(std::isinf(matrix[1][0]) && std::signbit(matrix[1][0]));
    BOOST_CHECK(std::isinf(matrix[0][1]) && !std::signbit(matrix[0][1]));
    BOOST_CHECK(std::isinf(matrix[1][1]) && !std::signbit(matrix[1][1]));
    BOOST_CHECK(std::isinf(matrix[2][1]) && std::signbit(matrix[2][1]));
}

BOOST_AUTO_TEST_CASE(rejects_nan_and_invalid_infinity_values)
{
    for (const auto value: {"nan", "-nan", "infjunk", "1e309"})
    {
        const auto file = writeFile(std::string(value) + "\n");
        Antares::Matrix matrix;
        BOOST_CHECK(
          !Antares::MatrixIO::load(matrix,
                                   file.path,
                                   (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 1})));
    }
}

BOOST_AUTO_TEST_CASE(exact_width_rejects_extra_columns)
{
    const auto file = writeFile("1\t2\t3\n4\t5\t6\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(
      matrix,
      file.path,
      (Antares::MatrixIO::LoadOptions{.minWidth = 2,
                                      .height = 2,
                                      .widthPolicy = Antares::MatrixIO::WidthPolicy::Exact})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(exact_width_rejects_extra_columns_on_later_rows)
{
    const auto file = writeFile("1\t2\n3\t4\t5\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(
      matrix,
      file.path,
      (Antares::MatrixIO::LoadOptions{.minWidth = 2,
                                      .height = 2,
                                      .widthPolicy = Antares::MatrixIO::WidthPolicy::Exact})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(loads_utf8_bom)
{
    const auto file = writeFile("\xEF\xBB\xBF"
                                "1.5\t2\n3\t4\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 2, .height = 2})));
    BOOST_CHECK_EQUAL(matrix[0][0], 1.5);
    BOOST_CHECK_EQUAL(matrix[1][1], 4.);
}

BOOST_AUTO_TEST_CASE(loads_dimension_header)
{
    Antares::Matrix source(2, 2);
    source[0][0] = 1.5;
    source[1][0] = 2.5;
    source[0][1] = 3.5;
    source[1][1] = 4.5;

    std::string contents;
    Antares::MatrixIO::saveToBuffer(source, contents, 1, true);
    const auto file = writeFile(contents);
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.5);
    BOOST_CHECK_EQUAL(matrix[1][1], 4.5);
}

BOOST_AUTO_TEST_CASE(loads_dimension_header_with_trailing_whitespace)
{
    for (const auto header: {"size:2x2 \n", "size:2x2\t\t \n"})
    {
        const auto file = writeFile(std::string(header) + "1\t2\n3\t4\n");
        Antares::Matrix matrix;

        BOOST_REQUIRE(
          Antares::MatrixIO::load(matrix,
                                  file.path,
                                  (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
        BOOST_REQUIRE_EQUAL(matrix.width(), 2);
        BOOST_REQUIRE_EQUAL(matrix.height(), 2);
        BOOST_CHECK_EQUAL(matrix[0][0], 1.);
        BOOST_CHECK_EQUAL(matrix[1][1], 4.);
    }
}

BOOST_AUTO_TEST_CASE(dimension_header_must_match_requested_height)
{
    const auto file = writeFile("size:1x1\n7\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                file.path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 1,
                                                                                .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 1);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(exact_width_ignores_trailing_tab_padding)
{
    const auto file = writeFile("1\t\n2\t\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(Antares::MatrixIO::load(
      matrix,
      file.path,
      (Antares::MatrixIO::LoadOptions{.minWidth = 1,
                                      .height = 2,
                                      .widthPolicy = Antares::MatrixIO::WidthPolicy::Exact})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 1);
    BOOST_CHECK_EQUAL(matrix[0][1], 2.);
}

BOOST_AUTO_TEST_CASE(file_larger_than_the_matrix_file_limit_is_rejected)
{
    const auto file = writeFile("");
    std::filesystem::resize_file(file.path, 1536ULL * 1024ULL * 1024ULL + 1);
    Antares::Matrix matrix;

    const auto error = Antares::MatrixIO::loadDetailed(matrix,
                                                       file.path,
                                                       {.minWidth = 1, .height = 2});
    BOOST_REQUIRE(error);
    BOOST_CHECK(error->filename == file.path);
    BOOST_CHECK_EQUAL(error->message, "matrix file is too large (maximum 1536 MiB)");
    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                file.path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 1,
                                                                                .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 1);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(huge_dimension_header_is_rejected_before_allocation)
{
    const auto file = writeFile("size:50001x1\n");
    Antares::Matrix matrix;

    const auto error = Antares::MatrixIO::loadDetailed(matrix,
                                                       file.path,
                                                       {.minWidth = 1, .height = 1});
    BOOST_REQUIRE(error);
    BOOST_CHECK_EQUAL(error->message, "invalid matrix header");
    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                file.path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 1,
                                                                                .height = 1})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 1);
    BOOST_REQUIRE_EQUAL(matrix.height(), 1);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(dynamic_width_load_reuses_requested_height)
{
    const auto file = writeFile("1\t2\t3\n4\t5\t6\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);

    {
        std::ofstream output(file.path, std::ios::binary | std::ios::trunc);
        output << "7\t8\n9\t10\n";
    }

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 7.);
    BOOST_CHECK_EQUAL(matrix[1][1], 10.);
}

BOOST_AUTO_TEST_CASE(loads_comma_separated_values_and_empty_cells)
{
    const auto file = writeFile("1,,3\r\n4,5,6\r\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[2][0], 3.);
    BOOST_CHECK_EQUAL(matrix[1][1], 5.);
}

BOOST_AUTO_TEST_CASE(variable_width_counts_final_empty_comma_cells)
{
    const auto file = writeFile("1,\n2,3\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 3.);
}

BOOST_AUTO_TEST_CASE(variable_width_ignores_trailing_tabs)
{
    const auto file = writeFile("1\t2\t\t\n3\t4\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.);
    BOOST_CHECK_EQUAL(matrix[1][1], 4.);
}

BOOST_AUTO_TEST_CASE(trailing_tabs_preserve_required_empty_columns)
{
    const auto file = writeFile("1\t\t\t\n2\t\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 2, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 0.);
}

BOOST_AUTO_TEST_CASE(header_width_preserves_empty_columns_and_ignores_padding)
{
    const auto file = writeFile("size:2x2\t\n1\t\t\t\n2\t\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.);
    BOOST_CHECK_EQUAL(matrix[0][1], 2.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 0.);
}

BOOST_AUTO_TEST_CASE(blank_single_column_row_is_zero)
{
    const auto file = writeFile("1\n\n2\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 3})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 1);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.);
    BOOST_CHECK_EQUAL(matrix[0][1], 0.);
    BOOST_CHECK_EQUAL(matrix[0][2], 2.);
}

BOOST_AUTO_TEST_CASE(whitespace_only_cells_are_treated_as_zero)
{
    const auto file = writeFile("1\t \t3\n \t5\t \n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 3, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 3);
    BOOST_CHECK_EQUAL(matrix[0][0], 1.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[2][0], 3.);
    BOOST_CHECK_EQUAL(matrix[0][1], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 5.);
    BOOST_CHECK_EQUAL(matrix[2][1], 0.);
}

BOOST_AUTO_TEST_CASE(dimension_checks_allow_large_decoded_matrices)
{
    Antares::Matrix matrix;
    const auto file = writeFile("");
    BOOST_CHECK(
      !Antares::MatrixIO::load(matrix,
                               file.path,
                               (Antares::MatrixIO::LoadOptions{.minWidth = 50001, .height = 1})));
    BOOST_CHECK(
      !Antares::MatrixIO::load(matrix,
                               file.path,
                               (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 50001})));
}

BOOST_AUTO_TEST_CASE(reloading_overwrites_previous_values)
{
    const auto file = writeFile("1\t2\n3\t4\n");
    Antares::Matrix matrix;

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 2, .height = 2})));

    {
        std::ofstream output(file.path, std::ios::binary | std::ios::trunc);
        output << "5\t\n\t8\n";
    }

    BOOST_REQUIRE(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 2, .height = 2})));
    BOOST_CHECK_EQUAL(matrix[0][0], 5.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
    BOOST_CHECK_EQUAL(matrix[0][1], 0.);
    BOOST_CHECK_EQUAL(matrix[1][1], 8.);
}

BOOST_AUTO_TEST_CASE(minimum_width_is_preserved)
{
    const auto file = writeFile("1\t2\t3\n4\t5\t6\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                file.path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 5,
                                                                                .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 5);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[3][0], 0.);
    BOOST_CHECK_EQUAL(matrix[4][1], 0.);
}

BOOST_AUTO_TEST_CASE(invalid_numeric_values_fail_and_are_zeroed)
{
    const auto file = writeFile("1\tnot-a-number\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                file.path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 2,
                                                                                .height = 1})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 1);
    BOOST_CHECK_EQUAL(matrix[0][0], 0.);
    BOOST_CHECK_EQUAL(matrix[1][0], 0.);
}

BOOST_AUTO_TEST_CASE(incomplete_rows_fail_and_reset_matrix)
{
    const auto file = writeFile("1\t2\n3\t4\n");
    Antares::Matrix matrix;

    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                file.path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 3,
                                                                                .height = 3})));
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

    BOOST_CHECK(
      Antares::MatrixIO::load(matrix,
                              file.path,
                              (Antares::MatrixIO::LoadOptions{.minWidth = 2, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(missing_files_fail)
{
    Antares::Matrix matrix;
    const auto temporary = writeFile("");
    const auto path = temporary.path.string() + ".missing";

    const auto error = Antares::MatrixIO::loadDetailed(matrix, path, {.minWidth = 2, .height = 2});
    BOOST_REQUIRE(error);
    BOOST_CHECK(error->filename == path);
    BOOST_CHECK(error->message.starts_with("cannot read file:"));
    Antares::UnitTests::CaptureAntaresLogs logs;
    BOOST_CHECK(!Antares::MatrixIO::loadOrReset(matrix,
                                                path,
                                                (Antares::MatrixIO::LoadOptions{.minWidth = 2,
                                                                                .height = 2})));
    BOOST_REQUIRE_EQUAL(logs.getErrors().size(), 1);
    BOOST_CHECK(logs.getErrors().begin()->find(path) != std::string::npos);
    BOOST_CHECK(logs.getErrors().begin()->find("cannot read file:") != std::string::npos);
    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK(matrix.containsOnlyZero());
}

BOOST_AUTO_TEST_CASE(failed_load_preserves_existing_matrix)
{
    const auto file = writeFile("1\ninvalid\n");
    Antares::Matrix matrix(1, 1);
    matrix[0][0] = 42.;
    BOOST_CHECK(
      !Antares::MatrixIO::load(matrix,
                               file.path,
                               (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE_EQUAL(matrix.height(), 1);
    BOOST_CHECK_EQUAL(matrix[0][0], 42.);
}

BOOST_AUTO_TEST_CASE(detailed_error_identifies_invalid_cell)
{
    const auto file = writeFile("1\t2\n3\tbad\n");
    Antares::Matrix matrix;
    const auto error = Antares::MatrixIO::loadDetailed(matrix,
                                                       file.path,
                                                       {.minWidth = 2, .height = 2});
    BOOST_REQUIRE(error);
    BOOST_REQUIRE(error->row);
    BOOST_REQUIRE(error->column);
    BOOST_CHECK_EQUAL(*error->row, 2);
    BOOST_CHECK_EQUAL(*error->column, 2);
    BOOST_CHECK_EQUAL(error->message, "invalid numeric value");
    BOOST_CHECK(matrix.empty());
}

BOOST_AUTO_TEST_CASE(detailed_error_identifies_column_mismatch)
{
    const auto file = writeFile("1\t2\n3\n");
    Antares::Matrix matrix;
    const auto error = Antares::MatrixIO::loadDetailed(matrix,
                                                       file.path,
                                                       {.minWidth = 2, .height = 2});
    BOOST_REQUIRE(error);
    BOOST_CHECK(error->filename == file.path);
    BOOST_REQUIRE(error->row);
    BOOST_CHECK_EQUAL(*error->row, 2);
    BOOST_CHECK_EQUAL(error->message, "expected 2 columns, got 1");
}

BOOST_AUTO_TEST_CASE(extra_rows_require_explicit_prefix_policy)
{
    const auto file = writeFile("1\n2\n3\n");
    Antares::Matrix matrix;
    BOOST_CHECK(
      !Antares::MatrixIO::load(matrix,
                               file.path,
                               (Antares::MatrixIO::LoadOptions{.minWidth = 1, .height = 2})));
    BOOST_REQUIRE(Antares::MatrixIO::load(
      matrix,
      file.path,
      (Antares::MatrixIO::LoadOptions{.minWidth = 1,
                                      .height = 2,
                                      .rowPolicy = Antares::MatrixIO::RowPolicy::Prefix})));
    BOOST_CHECK_EQUAL(matrix[0][1], 2.);
}

BOOST_AUTO_TEST_CASE(header_or_prefix_loads_only_complete_bounded_matrices)
{
    using namespace Antares::MatrixIO;
    Antares::Matrix matrix(1, 1);
    matrix[0][0] = 42.;
    const auto load = [&](std::string_view content)
    {
        const auto file = writeFile(content);
        return Antares::MatrixIO::load(matrix,
                                       file.path,
                                       {.minWidth = 1,
                                        .height = 3,
                                        .rowPolicy = RowPolicy::HeaderOrPrefix,
                                        .minHeaderHeight = 2});
    };
    BOOST_REQUIRE(load("size:2x2\n1\t2\n3\tinf\n"));
    BOOST_CHECK_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][1], 3.);
    BOOST_CHECK(std::isinf(matrix[1][1]));
    BOOST_REQUIRE(load("size:1x3\n1\n2\n3\n"));
    BOOST_CHECK_EQUAL(matrix.height(), 3);
    BOOST_REQUIRE(load("1\n2\n3\nignored\n"));
    BOOST_CHECK_EQUAL(matrix[0][2], 3.);
    for (const auto invalid: {"size:1x1\n1\n",
                              "size:1x4\n1\n2\n3\n4\n",
                              "size:1x2\n1\n",
                              "size:1x2\n1\n2\n3\n",
                              "1\nbad\n3\n4\n"})
    {
        BOOST_CHECK(!load(invalid));
        BOOST_CHECK_EQUAL(matrix.height(), 3);
        BOOST_CHECK_EQUAL(matrix[0][2], 3.);
    }
}

BOOST_AUTO_TEST_CASE(header_height_or_prefix_preserves_declared_height)
{
    using namespace Antares::MatrixIO;
    Antares::Matrix matrix;
    const auto load = [&](std::string_view content)
    {
        const auto file = writeFile(content);
        return Antares::MatrixIO::load(matrix,
                                       file.path,
                                       {.minWidth = 1,
                                        .height = 2,
                                        .rowPolicy = RowPolicy::HeaderHeightOrPrefix});
    };
    BOOST_REQUIRE(load("size:1x1\n7\nignored\n"));
    BOOST_CHECK_EQUAL(matrix.height(), 1);
    BOOST_CHECK_EQUAL(matrix[0][0], 7.);
    BOOST_REQUIRE(load("size:1x3\n1\n2\n3\nignored\n"));
    BOOST_CHECK_EQUAL(matrix.height(), 3);
    BOOST_CHECK_EQUAL(matrix[0][2], 3.);
    BOOST_REQUIRE(load("1\n2\nignored\n"));
    BOOST_CHECK_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][1], 2.);
    BOOST_CHECK(!load("size:1x3\n1\n2\n"));
    BOOST_CHECK_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][1], 2.);
}
