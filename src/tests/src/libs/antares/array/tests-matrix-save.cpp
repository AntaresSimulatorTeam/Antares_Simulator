// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - lib - antares - matrix tests

#define WIN32_LEAN_AND_MEAN

#include "tests-matrix-save.h"

#include <cstdlib>

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

// ================================
// ===  Matrix<double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__save_into_double)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and__no_print_dim___result_is_empty)
{
    Matrix_easy_to_fill<double> mtx;
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___get_only_a_title_and_the_0s)
{
    Matrix_easy_to_fill<double> mtx;
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data, 0, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x2\n0\t0\n0\t0\n");
}

BOOST_AUTO_TEST_CASE(coeffs_have_int_values___no_zeros_after_decimal_point)
{
    Matrix_easy_to_fill<double> mtx(2, 2, {1, 0, 0, -2});
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "1\t0\n0\t-2\n");
}

BOOST_AUTO_TEST_CASE(precision_is_3___get_3_nbs_after_decimal_point)
{
    Matrix_easy_to_fill<double> mtx(2, 2, {1.5, -2.44444, 3.66666, 0});
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "1.500\t-2.444\n3.667\t0\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_integer_values)
{
    // Any whole number is printed without decimal point
    Matrix_easy_to_fill<double> mtx(2, 2, {1, 5, 0, 3});
    MatrixIO::saveToBuffer(mtx, mtx.data, 4, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "1\t5\n0\t3\n");
}

BOOST_AUTO_TEST_CASE(add_identity_predicate___each_coeff_is_unchanged)
{
    Matrix_easy_to_fill<double> mtx(2, 2, {1, 0, 0, 2});
    PredicateIdentity predicate;
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, false, predicate);
    BOOST_REQUIRE_EQUAL(mtx.data, "1\t0\n0\t2\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows)
{
    Matrix_easy_to_fill<double> mtx(3, 1, {1.5, -3.552, 0.66});
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1.50\n-3.55\n0.66\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows_thermal)
{
    Matrix_easy_to_fill<double> mtx(3, 1, {1., -3., 2.});
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1\n-3\n2\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows_renw)
{
    Matrix_easy_to_fill<double> mtx(3, 1, {1.3333, -3.66666, 2.});
    MatrixIO::saveToBuffer(mtx, mtx.data, 4, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1.3333\n-3.6667\n2\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(matrix_operations)

BOOST_AUTO_TEST_CASE(fill_unit_handles_rectangular_matrices)
{
    Matrix<int> matrix(3, 1);
    matrix.fillUnit();

    BOOST_CHECK_EQUAL(matrix[0][0], 1);
    BOOST_CHECK_EQUAL(matrix[1][0], 0);
    BOOST_CHECK_EQUAL(matrix[2][0], 0);
}

BOOST_AUTO_TEST_CASE(fill_unit_handles_tall_matrices)
{
    Matrix<int> matrix(1, 3);
    matrix.fillUnit();

    BOOST_CHECK_EQUAL(matrix[0][0], 1);
    BOOST_CHECK_EQUAL(matrix[0][1], 0);
    BOOST_CHECK_EQUAL(matrix[0][2], 0);
}

BOOST_AUTO_TEST_CASE(contains_only_zero_accepts_a_predicate)
{
    Matrix<int> matrix(2, 1);
    matrix[0][0] = 1;
    matrix[1][0] = -1;
    auto absoluteValue = [](int value) { return std::abs(value); };

    BOOST_CHECK(matrix.containsOnlyZero(absoluteValue) == false);

    matrix.zero();
    BOOST_CHECK(matrix.containsOnlyZero(absoluteValue));
}

BOOST_AUTO_TEST_CASE(resize_without_data_lost_fills_new_values)
{
    Matrix<int> matrix(1, 1);
    matrix[0][0] = 7;

    matrix.resizeWithoutDataLost(2, 2, 3);

    BOOST_CHECK_EQUAL(matrix[0][0], 7);
    BOOST_CHECK_EQUAL(matrix[0][1], 3);
    BOOST_CHECK_EQUAL(matrix[1][0], 3);
    BOOST_CHECK_EQUAL(matrix[1][1], 3);
}

BOOST_AUTO_TEST_SUITE_END()

// =============================
// ===  Matrix<int>  ===
// =============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__save_into_double)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and__no_print_dim___result_is_empty)
{
    Matrix_easy_to_fill<int> mtx(2, 2);
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___get_only_a_title_and_the_0s)
{
    Matrix_easy_to_fill<int> mtx;
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data, 0, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x2\n0\t0\n0\t0\n");
}

BOOST_AUTO_TEST_CASE(any_whole_number_is_printed_without_decimal_point)
{
    Matrix_easy_to_fill<int> mtx(2, 2, {10, 500, 0, 3});
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "10\t500\n0\t3\n");
}

BOOST_AUTO_TEST_CASE(negative_int___printed_correctly)
{
    Matrix_easy_to_fill<int> mtx(2, 3, {1, -2, 3, -4, -5, 6});
    MatrixIO::saveToBuffer(mtx, mtx.data, 2, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:3x2\n1\t-2\t3\n-4\t-5\t6\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_int_coeffs)
{
    Matrix_easy_to_fill<int> mtx(3, 2, {1, -2, 3, -4, -5, 6});
    MatrixIO::saveToBuffer(mtx, mtx.data, 5, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1\t-2\n3\t-4\n-5\t6\n");
}

BOOST_AUTO_TEST_SUITE_END()

// ==========================
// ===  Matrix<int>  ===
// ==========================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__save_into_int)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and_no_print_dim___result_is_empty)
{
    Matrix_easy_to_fill<int> mtx(2, 2);
    mtx.reset(2, 2);
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___dim_and_0s_are_printed)
{
    Matrix_easy_to_fill<int> mtx(3, 1); // Normal Matrix constuctor : 3 columns x 1 row
    mtx.zero();
    MatrixIO::saveToBuffer(mtx, mtx.data, 0, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:3x1\n0\t0\t0\n");
}

BOOST_AUTO_TEST_CASE(first_matrix___int_to_int)
{
    Matrix_easy_to_fill<int> mtx(2, 2, {1000, -5000, 0, 3000});
    MatrixIO::saveToBuffer(mtx, mtx.data);
    BOOST_REQUIRE_EQUAL(mtx.data, "1000\t-5000\n0\t3000\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_int_coeffs)
{
    Matrix_easy_to_fill<int> mtx(3, 2, {1, -2, 3, -4, -5, 6});
    MatrixIO::saveToBuffer(mtx, mtx.data, 5, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1\t-2\n3\t-4\n-5\t6\n");
}

BOOST_AUTO_TEST_SUITE_END()

// =============================
// ===  Matrix<double>  ===
// =============================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__save_into_int)

BOOST_AUTO_TEST_CASE(coeffs_only_0s__no_print_dim___result_empty)
{
    Matrix_easy_to_fill<double> mtx(2, 2);
    mtx.zero();
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(coeffs_only_0s__print_dim___get_dim_and_0s_in_output)
{
    Matrix_easy_to_fill<double> mtx(1, 3);
    mtx.zero();
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n0\n0\n0\n");
}

BOOST_AUTO_TEST_CASE(precision_is_3__get_coeffs_floor_integers)
{
    Matrix_easy_to_fill<double> mtx(2, 2, {1.99, 2.44, -3.999, -1.51});
    MatrixIO::saveToBuffer(mtx, mtx.data, 3, false);
    BOOST_REQUIRE_EQUAL(mtx.data, "1.990\t2.440\n-3.999\t-1.510\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_int_coeffs)
{
    Matrix_easy_to_fill<double> mtx(3, 2, {1.99, -2.49, 3, -4.99, -5, 6.99});
    MatrixIO::saveToBuffer(mtx, mtx.data, 5, true);
    BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1.99000\t-2.49000\n3\t-4.99000\n-5\t6.99000\n");
}

BOOST_AUTO_TEST_SUITE_END()
