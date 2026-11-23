// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - lib - antares - matrix tests

#define WIN32_LEAN_AND_MEAN

#include "tests-matrix-save.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

// ================================
// ===  Matrix  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__save_into_double)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and__no_print_dim___result_is_empty)
{
    Matrix_easy_to_fill mtx;
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(save_to_buffer_clears_stale_output_for_zero_matrix)
{
    Matrix_easy_to_fill mtx;
    mtx.reset(2, 2);
    mtx.data = "old data";

    MatrixIO::saveToBuffer(mtx, mtx.data);

    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

struct ThrowOnSerialization
{
    double operator()(double) const
    {
        throw std::runtime_error("serialization failed");
    }
};

std::filesystem::path temporaryMatrixPath()
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path()
           / ("antares-matrix-save-test-" + std::to_string(timestamp) + ".txt");
}

BOOST_AUTO_TEST_CASE(save_does_not_open_file_when_serialization_throws)
{
    const auto filename = temporaryMatrixPath();
    {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        BOOST_REQUIRE(file);
        file << "old data";
    }

    Matrix mtx(1, 1);
    mtx[0][0] = 1.0;

    BOOST_CHECK_THROW(
      MatrixIO::save(mtx, filename.string(), 6, false, ThrowOnSerialization{}, true),
      std::runtime_error);

    {
        std::ifstream file(filename, std::ios::binary);
        BOOST_REQUIRE(file);
        const std::string contents((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        BOOST_CHECK_EQUAL(contents, "old data");
    }
    std::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE(save_to_buffer_preserves_output_when_serialization_throws)
{
    Matrix_easy_to_fill mtx(1, 1, {1.0});
    std::string output = "old data";

    BOOST_CHECK_THROW(MatrixIO::saveToBuffer(mtx, output, 6, false, ThrowOnSerialization{}, true),
                      std::runtime_error);
    BOOST_CHECK_EQUAL(output, "old data");
}

BOOST_AUTO_TEST_CASE(save_replaces_existing_file)
{
    const auto filename = temporaryMatrixPath();
    {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        BOOST_REQUIRE(file);
        file << "old data";
    }

    Matrix mtx(1, 1);
    mtx[0][0] = 2.5;

    BOOST_REQUIRE(MatrixIO::save(mtx, filename.string()));

    {
        std::ifstream file(filename, std::ios::binary);
        BOOST_REQUIRE(file);
        const std::string contents((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        BOOST_CHECK_EQUAL(contents, "2.500000\n");
    }
    std::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE(nonfinite_save_returns_false_and_preserves_existing_file)
{
    const auto filename = temporaryMatrixPath();
    {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        BOOST_REQUIRE(file);
        file << "old data";
    }

    Matrix matrix(1, 1);
    for (double value: {std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::infinity(),
                        -std::numeric_limits<double>::infinity()})
    {
        matrix[0][0] = value;
        BOOST_CHECK(!MatrixIO::save(matrix, filename.string()));
        std::ifstream file(filename, std::ios::binary);
        BOOST_REQUIRE(file);
        const std::string contents((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        BOOST_CHECK_EQUAL(contents, "old data");
    }
    std::filesystem::remove(filename);
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___get_only_a_title_and_the_0s)
{
    Matrix_easy_to_fill mtx;
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data, 0, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x2\n0\t0\n0\t0\n");
}

BOOST_AUTO_TEST_CASE(coeffs_have_int_values___no_zeros_after_decimal_point)
{
    Matrix_easy_to_fill mtx(2, 2, {1, 0, 0, -2});
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "1\t0\n0\t-2\n");
}

BOOST_AUTO_TEST_CASE(precision_is_3___get_3_nbs_after_decimal_point)
{
    Matrix_easy_to_fill mtx(2, 2, {1.5, -2.44444, 3.66666, 0});
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "1.500\t-2.444\n3.667\t0\n");
}

BOOST_AUTO_TEST_CASE(precision_above_supported_range_is_clamped)
{
    Matrix_easy_to_fill mtx(1, 1, {1.1234567890123456});

    MatrixIO::saveToBuffer(mtx, mtx.data, 17);

    BOOST_REQUIRE_EQUAL(mtx.data, "1.1234567890123457\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_integer_values)
{
    // Any whole number is printed without decimal point
    Matrix_easy_to_fill mtx(2, 2, {1, 5, 0, 3});
    MatrixIO::saveToBuffer(mtx, mtx.data, 4, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "1\t5\n0\t3\n");
}

BOOST_AUTO_TEST_CASE(add_identity_predicate___each_coeff_is_unchanged)
{
    Matrix_easy_to_fill mtx(2, 2, {1, 0, 0, 2});
    PredicateIdentity predicate;
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, false, predicate);
    BOOST_REQUIRE_EQUAL(mtx.data, "1\t0\n0\t2\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows)
{
    Matrix_easy_to_fill mtx(3, 1, {1.5, -3.552, 0.66});
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1.50\n-3.55\n0.66\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows_thermal)
{
    Matrix_easy_to_fill mtx(3, 1, {1., -3., 2.});
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1\n-3\n2\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows_renw)
{
    Matrix_easy_to_fill mtx(3, 1, {1.3333, -3.66666, 2.});
    MatrixIO::saveToBuffer(mtx, mtx.data, 4, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1.3333\n-3.6667\n2\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(matrix_operations)

BOOST_AUTO_TEST_CASE(small_multiplier_is_not_treated_as_zero)
{
    Matrix matrix(1, 1);
    matrix[0][0] = 1e9;
    matrix.multiplyAllEntriesBy(1e-7);
    BOOST_CHECK_CLOSE(matrix[0][0], 100., 1e-10);
}

BOOST_AUTO_TEST_CASE(multiplication_by_zero_obeys_floating_point_arithmetic)
{
    Matrix matrix(1, 1);
    matrix[0][0] = std::numeric_limits<double>::infinity();
    matrix.multiplyAllEntriesBy(0.);
    BOOST_CHECK(std::isnan(matrix[0][0]));
}

BOOST_AUTO_TEST_CASE(column_views_and_self_assignment_preserve_shape)
{
    Matrix matrix(1, 2);
    auto column = matrix[0];
    column[1] = 7.;
    const auto assign = [](Matrix& destination, const Matrix& source) { destination = source; };
    assign(matrix, matrix);
    BOOST_CHECK_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][1], 7.);
    BOOST_CHECK_THROW(matrix.pasteToColumn(0, std::span<const double>(column.data(), 1)),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(typed_storage_preserves_shape_after_copy_and_move)
{
    ColumnMatrix<uint32_t> numbers;
    numbers.reset(2, 3);
    numbers[1][2] = 9;
    auto copy = numbers;
    ColumnMatrix<uint32_t> moved(std::move(copy));
    BOOST_CHECK_EQUAL(moved.width(), 2);
    BOOST_CHECK_EQUAL(moved.height(), 3);
    BOOST_CHECK_EQUAL(moved[1][2], 9);
    BOOST_CHECK_EQUAL(copy.width(), 0);
    ColumnMatrix<float> correlations;
    correlations.reset(2, 2);
    correlations[0][1] = 0.5f;
    BOOST_CHECK_EQUAL(correlations[0][1], 0.5f);
}

BOOST_AUTO_TEST_CASE(same_shape_copy_reuses_numeric_columns)
{
    ColumnMatrix<double> source;
    source.reset(2, 3);
    source[0][1] = 4.;
    source[1][2] = 9.;
    ColumnMatrix<double> destination;
    destination.reset(2, 3);
    const auto* first = destination[0].data();
    const auto* second = destination[1].data();
    destination = source;
    BOOST_CHECK(destination[0].data() == first);
    BOOST_CHECK(destination[1].data() == second);
    BOOST_CHECK_EQUAL(destination[0][1], 4.);
    BOOST_CHECK_EQUAL(destination[1][2], 9.);
    const auto assign = [](auto& lhs, const auto& rhs) { lhs = rhs; };
    assign(destination, destination);
    BOOST_CHECK(destination[0].data() == first);
    BOOST_CHECK_EQUAL(destination[1][2], 9.);
}

struct ThrowingCopy
{
    int value = 0;
    static inline bool throwOnCopy = false;
    ThrowingCopy() = default;

    ThrowingCopy(const ThrowingCopy& rhs):
        value(rhs.value)
    {
        if (throwOnCopy)
        {
            throw std::runtime_error("copy failed");
        }
    }

    ThrowingCopy& operator=(const ThrowingCopy& rhs)
    {
        value = rhs.value;
        return *this;
    }
};

BOOST_AUTO_TEST_CASE(throwing_element_copy_preserves_destination_on_failure)
{
    ColumnMatrix<ThrowingCopy> source;
    source.reset(1, 1);
    source[0][0].value = 7;
    ColumnMatrix<ThrowingCopy> destination;
    destination.reset(1, 1);
    destination[0][0].value = 3;
    ThrowingCopy::throwOnCopy = true;
    BOOST_CHECK_THROW(destination = source, std::runtime_error);
    ThrowingCopy::throwOnCopy = false;
    BOOST_CHECK_EQUAL(destination[0][0].value, 3);
    destination = source;
    BOOST_CHECK_EQUAL(destination[0][0].value, 7);
}

BOOST_AUTO_TEST_CASE(shared_storage_resize_reset_swap_and_move)
{
    ColumnMatrix<int> values;
    values.reset(2, 2);
    values[0][0] = 4;
    values.resize(2, 2);
    BOOST_CHECK_EQUAL(values[0][0], 4);
    values.resizeWithoutDataLost(3, 3, 7);
    BOOST_CHECK_EQUAL(values[0][0], 4);
    BOOST_CHECK_EQUAL(values[2][2], 7);
    values.resizeWithoutDataLost(1, 2);
    BOOST_CHECK_EQUAL(values[0][0], 4);
    values.reset(1, 2);
    BOOST_CHECK_EQUAL(values[0][0], 0);
    values[0][1] = 9;
    ColumnMatrix<int> other;
    other.swap(values);
    BOOST_CHECK_EQUAL(values.width(), 0);
    BOOST_CHECK_EQUAL(other[0][1], 9);
    values = std::move(other);
    BOOST_CHECK_EQUAL(other.width(), 0);
    BOOST_CHECK_EQUAL(values[0][1], 9);
    values.resize(0, 3);
    BOOST_CHECK_EQUAL(values.width(), 0);
    BOOST_CHECK_EQUAL(values.height(), 0);

    Matrix matrix(2, 1);
    matrix[0][0] = 2.;
    matrix[1][0] = 4.;
    matrix.averageTimeseries(false);
    BOOST_CHECK_EQUAL(matrix.width(), 1);
    BOOST_CHECK_EQUAL(matrix[0][0], 3.);
    Matrix moved;
    moved = std::move(matrix);
    BOOST_CHECK(matrix.empty());
    BOOST_CHECK_EQUAL(moved[0][0], 3.);
}

BOOST_AUTO_TEST_CASE(fill_unit_handles_rectangular_matrices)
{
    Matrix matrix(3, 1);
    matrix.fillUnit();

    BOOST_CHECK_EQUAL(matrix[0][0], 1);
    BOOST_CHECK_EQUAL(matrix[1][0], 0);
    BOOST_CHECK_EQUAL(matrix[2][0], 0);
}

BOOST_AUTO_TEST_CASE(fill_unit_handles_tall_matrices)
{
    Matrix matrix(1, 3);
    matrix.fillUnit();

    BOOST_CHECK_EQUAL(matrix[0][0], 1);
    BOOST_CHECK_EQUAL(matrix[0][1], 0);
    BOOST_CHECK_EQUAL(matrix[0][2], 0);
}

BOOST_AUTO_TEST_CASE(contains_only_zero_accepts_a_predicate)
{
    Matrix matrix(2, 1);
    matrix[0][0] = 1;
    matrix[1][0] = -1;
    auto absoluteValue = [](double value) { return std::abs(value); };

    BOOST_CHECK(matrix.containsOnlyZero(absoluteValue) == false);

    matrix.zero();
    BOOST_CHECK(matrix.containsOnlyZero(absoluteValue));
}

BOOST_AUTO_TEST_CASE(resize_without_data_lost_fills_new_values)
{
    Matrix matrix(1, 1);
    matrix[0][0] = 7;

    matrix.resizeWithoutDataLost(2, 2, 3);

    BOOST_CHECK_EQUAL(matrix[0][0], 7);
    BOOST_CHECK_EQUAL(matrix[0][1], 3);
    BOOST_CHECK_EQUAL(matrix[1][0], 3);
    BOOST_CHECK_EQUAL(matrix[1][1], 3);
}

BOOST_AUTO_TEST_CASE(resize_without_data_lost_preserves_values_during_mixed_resize)
{
    Matrix matrix(2, 3);
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[1][0] = 3;

    matrix.resizeWithoutDataLost(1, 5, 9);
    BOOST_REQUIRE_EQUAL(matrix.width(), 1);
    BOOST_REQUIRE_EQUAL(matrix.height(), 5);
    BOOST_CHECK_EQUAL(matrix[0][0], 1);
    BOOST_CHECK_EQUAL(matrix[0][1], 2);
    BOOST_CHECK_EQUAL(matrix[0][2], 0);
    BOOST_CHECK_EQUAL(matrix[0][3], 9);
    BOOST_CHECK_EQUAL(matrix[0][4], 9);

    matrix.resizeWithoutDataLost(4, 2, 8);
    BOOST_REQUIRE_EQUAL(matrix.width(), 4);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], 1);
    BOOST_CHECK_EQUAL(matrix[0][1], 2);
    BOOST_CHECK_EQUAL(matrix[1][0], 8);
    BOOST_CHECK_EQUAL(matrix[3][1], 8);
}

BOOST_AUTO_TEST_CASE(resize_rejects_excessive_dimensions)
{
    Matrix matrix;

    BOOST_CHECK_THROW(matrix.resize(Matrix::kMaxDimension + 1, 1), std::length_error);
    BOOST_CHECK_THROW(matrix.resizeWithoutDataLost(Matrix::kMaxDimension + 1, 1),
                      std::length_error);
}

BOOST_AUTO_TEST_CASE(zero_sized_constructor_keeps_matrix_empty)
{
    Matrix matrix(0, 3);

    BOOST_CHECK(matrix.empty());
    BOOST_CHECK_EQUAL(matrix.width(), 0);
    BOOST_CHECK_EQUAL(matrix.height(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

// =============================
// ===  Matrix  ===
// =============================
BOOST_AUTO_TEST_SUITE(matrix_serialization)

BOOST_AUTO_TEST_CASE(serializes_small_and_near_integer_values_at_requested_precision)
{
    Matrix matrix(2, 1);
    matrix[0][0] = 0.0000005;
    matrix[1][0] = 1.0000005;
    std::string result;
    MatrixIO::saveToBuffer(matrix, result, 7);
    BOOST_CHECK_EQUAL(result, "0.0000005\t1.0000005\n");
}

BOOST_AUTO_TEST_CASE(nonfinite_values_are_not_silently_serialized)
{
    Matrix matrix(1, 1);
    matrix[0][0] = std::numeric_limits<double>::infinity();
    std::string result;
    BOOST_CHECK_THROW(MatrixIO::saveToBuffer(matrix, result), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(large_finite_values_are_serialized)
{
    Matrix matrix(1, 1);
    matrix[0][0] = std::numeric_limits<double>::max();
    std::string result;
    MatrixIO::saveToBuffer(matrix, result);
    BOOST_CHECK_EQUAL(result.size(), 310);
    BOOST_CHECK(result.find("ERR") == std::string::npos);
}

BOOST_AUTO_TEST_CASE(coeffs_only_0s__no_print_dim___result_empty)
{
    Matrix_easy_to_fill mtx(2, 2);
    mtx.zero();
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(coeffs_only_0s__print_dim___get_dim_and_0s_in_output)
{
    Matrix_easy_to_fill mtx(1, 3);
    mtx.zero();
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n0\n0\n0\n");
}

BOOST_AUTO_TEST_CASE(precision_controls_fractional_digits)
{
    Matrix_easy_to_fill mtx(2, 2, {1.99, 2.44, -3.999, -1.51});
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "1.990\t2.440\n-3.999\t-1.510\n");
}

BOOST_AUTO_TEST_CASE(precision_preserves_fractional_values)
{
    Matrix_easy_to_fill mtx(3, 2, {1.99, -2.49, 3, -4.99, -5, 6.99});
    MatrixIO::saveToBuffer(mtx, mtx.data, 5, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1.99000\t-2.49000\n3\t-4.99000\n-5\t6.99000\n");
}

BOOST_AUTO_TEST_SUITE_END()
