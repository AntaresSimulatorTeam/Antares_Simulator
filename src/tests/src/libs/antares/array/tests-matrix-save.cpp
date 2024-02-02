/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE test-lib-antares-matrix tests
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "tests-matrix-save.h"

namespace utf = boost::unit_test;


// ================================
// ===  Matrix<double, double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__save_into_double)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and__no_print_dim___result_is_empty)
{
	Matrix_easy_to_fill<double, double> mtx;
	mtx.reset(2, 2, true);
	mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___get_only_a_title_and_the_0s)
{
	Matrix_easy_to_fill<double, double> mtx;
	mtx.reset(2, 2, true);
	mtx.saveToCSVFile("path/to/an/output/file", 0, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:2x2\n0\t0\n0\t0\n");
}

BOOST_AUTO_TEST_CASE(coeffs_have_int_values___no_zeros_after_decimal_point)
{
	Matrix_easy_to_fill<double, double> mtx(2, 2, {1, 0, 0, -2});
	mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mtx.data, "1\t0\n0\t-2\n");
}

BOOST_AUTO_TEST_CASE(precision_is_3___get_3_nbs_after_decimal_point)
{
	Matrix_easy_to_fill<double, double> mtx(2, 2, {1.5, -2.44444, 3.66666, 0});
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_REQUIRE_EQUAL(mtx.data, "1.500\t-2.444\n3.667\t0\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_integer_values)
{
	// Any whole number is printed without decimal point
	Matrix_easy_to_fill<double, double> mtx(2, 2, {1, 5, 0, 3});
	mtx.saveToCSVFile("path/to/an/output/file", 4, false);
	BOOST_REQUIRE_EQUAL(mtx.data, "1\t5\n0\t3\n");
}

BOOST_AUTO_TEST_CASE(add_identity_predicate___each_coeff_is_unchanged)
{
	Matrix_easy_to_fill<double, double> mtx(2, 2, {1, 0, 0, 2});
	PredicateIdentity predicate;
	mtx.saveToCSVFile("path/to/an/output/file", 2, false, predicate);
	BOOST_REQUIRE_EQUAL(mtx.data, "1\t0\n0\t2\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows)
{
	Matrix_easy_to_fill<double, double> mtx(3, 1, { 1.5, -3.552, 0.66 });
	mtx.saveToCSVFile("path/to/an/output/file", 2, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1.50\n-3.55\n0.66\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows_thermal)
{
	Matrix_easy_to_fill<double, double> mtx(3, 1, { 1., -3., 2. });
	mtx.saveToCSVFile("path/to/an/output/file", 2, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1\n-3\n2\n");
}

BOOST_AUTO_TEST_CASE(one_column__3_rows_renw)
{
	Matrix_easy_to_fill<double, double> mtx(3, 1, { 1.3333, -3.66666, 2. });
	mtx.saveToCSVFile("path/to/an/output/file", 4, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n1.3333\n-3.6667\n2\n");
}

BOOST_AUTO_TEST_SUITE_END()



// =============================
// ===  Matrix<int, double>  ===
// =============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__save_into_double)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and__no_print_dim___result_is_empty)
{
	Matrix_easy_to_fill<int, double> mtx(2, 2);
	mtx.reset(2, 2, true);
	mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___get_only_a_title_and_the_0s)
{
	Matrix_easy_to_fill<int, double> mtx;
	mtx.reset(2, 2, true);
	mtx.saveToCSVFile("path/to/an/output/file", 0, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:2x2\n0\t0\n0\t0\n");
}

BOOST_AUTO_TEST_CASE(any_whole_number_is_printed_without_decimal_point)
{
	Matrix_easy_to_fill<int, double> mtx(2, 2, {10, 500, 0, 3});
	mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mtx.data, "10\t500\n0\t3\n");
}

BOOST_AUTO_TEST_CASE(add_predicate__print_dim___each_coeff_is_one_incremented)
{
	Matrix_easy_to_fill<int, double> mtx(2, 3, { 1, 2, 3, 4, 5, 6 });
	TSNumbersPredicate predicate;
	mtx.saveToCSVFile("path/to/an/output/file", 2, true, predicate);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:3x2\n2\t3\t4\n5\t6\t7\n");
}

BOOST_AUTO_TEST_CASE(negative_int___printed_correctly)
{
	Matrix_easy_to_fill<int, double> mtx(2, 3, { 1, -2, 3, -4, -5, 6 });
	mtx.saveToCSVFile("path/to/an/output/file", 2, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:3x2\n1\t-2\t3\n-4\t-5\t6\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_int_coeffs)
{
	Matrix_easy_to_fill<int, double> mtx(3, 2, { 1, -2, 3, -4, -5, 6 });
	mtx.saveToCSVFile("path/to/an/output/file", 5, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1\t-2\n3\t-4\n-5\t6\n");
}

BOOST_AUTO_TEST_SUITE_END()



// ==========================
// ===  Matrix<int, int>  ===
// ==========================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__save_into_int)

BOOST_AUTO_TEST_CASE(matrix_only_0s_and_no_print_dim___result_is_empty)
{
	Matrix_easy_to_fill<int, int> mtx(2, 2);
	mtx.reset(2, 2, true);
	mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s__print_dim___dim_and_0s_are_printed)
{
	Matrix_easy_to_fill<int, int> mtx(3, 1); // Normal Matrix constuctor : 3 columns x 1 row
	mtx.zero();
	mtx.saveToCSVFile("path/to/an/output/file", 0, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:3x1\n0\t0\t0\n");
}

BOOST_AUTO_TEST_CASE(first_matrix___int_to_int)
{
	Matrix_easy_to_fill<int, int> mtx(2, 2, {1000, -5000, 0, 3000});
	mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mtx.data, "1000\t-5000\n0\t3000\n");
}

BOOST_AUTO_TEST_CASE(add_predicate__print_dim__negative_int___each_coeff_is_one_incremented)
{
	Matrix_easy_to_fill<int, int> mtx(2, 3, { -1, 2, -3, 4, -5, 6 });
	TSNumbersPredicate predicate;
	mtx.saveToCSVFile("path/to/an/output/file", 2, true, predicate);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:3x2\n0\t3\t-2\n5\t-4\t7\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_int_coeffs)
{
	Matrix_easy_to_fill<int, int> mtx(3, 2, { 1, -2, 3, -4, -5, 6 });
	mtx.saveToCSVFile("path/to/an/output/file", 5, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1\t-2\n3\t-4\n-5\t6\n");
}

BOOST_AUTO_TEST_SUITE_END()


// =============================
// ===  Matrix<double, int>  ===
// =============================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__save_into_int)

BOOST_AUTO_TEST_CASE(coeffs_only_0s__no_print_dim___result_empty)
{
	Matrix_easy_to_fill<double, int> mtx(2, 2);
	mtx.zero();
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_REQUIRE_EQUAL(mtx.data, "");
}

BOOST_AUTO_TEST_CASE(coeffs_only_0s__print_dim___get_dim_and_0s_in_output)
{
	Matrix_easy_to_fill<double, int> mtx(1, 3);
	mtx.zero();
	mtx.saveToCSVFile("path/to/an/output/file", 3, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:1x3\n0\n0\n0\n");
}

BOOST_AUTO_TEST_CASE(precision_is_3__get_coeffs_floor_integers)
{
	Matrix_easy_to_fill<double, int> mtx(2, 2, {1.99, 2.44, -3.999, -1.51});
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_REQUIRE_EQUAL(mtx.data, "1\t2\n-3\t-1\n");
}


BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_int_coeffs)
{
	Matrix_easy_to_fill<double, int> mtx(3, 2, { 1.99, -2.49, 3, -4.99, -5, 6.99 });
	mtx.saveToCSVFile("path/to/an/output/file", 5, true);
	BOOST_REQUIRE_EQUAL(mtx.data, "size:2x3\n1\t-2\n3\t-4\n-5\t6\n");
}

BOOST_AUTO_TEST_SUITE_END()


// ===================
// ===  About JIT  ===
// ===================

BOOST_AUTO_TEST_SUITE(jit)

BOOST_AUTO_TEST_CASE(JIT_enabler_built__jit_also_built)
{
	{
		global_JIT_manager global_JIT_to(true);
		BOOST_CHECK(JIT::enabled);
	}
	BOOST_CHECK(not JIT::enabled);
}

BOOST_AUTO_TEST_CASE(global_JIT_off___matrix_built_with_any_constructor_or_reset____jit_not_built)
{
	Matrix_load_bypass<double, double> mtx_1, mtx_2(2, 2);
	BOOST_CHECK(not mtx_1.jit);
	BOOST_CHECK(not mtx_2.jit);
	mtx_1.reset(2, 2, true);
	BOOST_CHECK(not mtx_1.jit);
}

BOOST_AUTO_TEST_CASE(global_JIT_on___matrix_built_with_any_constructor____jit_not_built)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx_1, mtx_2(2, 2);
	BOOST_CHECK(not mtx_1.jit);
	BOOST_CHECK(not mtx_2.jit);
	mtx_1.reset(2, 2, true);
	BOOST_CHECK(mtx_1.jit);
}

BOOST_AUTO_TEST_CASE(global_JIT_on___matrix_built_with_reset____jit_built)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx;
	mtx.reset(2, 2, true);
	BOOST_CHECK(mtx.jit);
}

BOOST_AUTO_TEST_CASE(global_JIT_ON__reset__save____no_load_done)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx;
	mtx.reset(2, 2, true);
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(not mtx.loadFromCSVFile_called);
	BOOST_CHECK(not mtx.jit->loadDataIfNotAlreadyDone);
	BOOST_CHECK(not mtx.jit->alreadyLoaded);
}

BOOST_AUTO_TEST_CASE(global_JIT_ON__reset__save__force_loadData_to_ON____no_load_done)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx;
	mtx.reset(2, 2, true);
	mtx.jit->loadDataIfNotAlreadyDone = true;
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(not mtx.loadFromCSVFile_called);
	BOOST_CHECK(not mtx.jit->loadDataIfNotAlreadyDone);
	BOOST_CHECK(not mtx.jit->alreadyLoaded);
}

BOOST_AUTO_TEST_CASE(global_JIT_ON__reset__save__force_loadData_to_ON__alreadyLoaded_to_OFF____load_done)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx;
	mtx.reset(2, 2, true);
	mtx.jit->loadDataIfNotAlreadyDone = true;
	mtx.jit->alreadyLoaded = false;
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(mtx.loadFromCSVFile_called);
	BOOST_CHECK(not mtx.jit->loadDataIfNotAlreadyDone);
}

BOOST_AUTO_TEST_CASE(matrix_not_empty_cleared_BEFORE_it_can_be_saved)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx(2, 2, { 1.99, 2.44, -3.999, -1.51 });
	mtx.jit = JIT::Reset(mtx.jit);
	mtx.jit->modified = false;
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(not mtx.loadFromCSVFile_called);
	BOOST_CHECK(mtx.empty());
}

BOOST_AUTO_TEST_CASE(global_JIT_ON___matrix_saved_and_cleared)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx(2, 2, { 1.99, 2.44, -3.999, -1.51 });
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(mtx.empty());
}

BOOST_AUTO_TEST_CASE(global_JIT_OFF_jit_off___matrix_never_cleared)
{
	global_JIT_manager global_JIT_to(false);
	Matrix_load_bypass<double, double> mtx(2, 2, { 1.99, 2.44, -3.999, -1.51 });
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(not mtx.empty());
}

BOOST_AUTO_TEST_CASE(global_JIT_ON_jit_off___matrix_cleared)
{
	global_JIT_manager global_JIT_to(true);
	Matrix_load_bypass<double, double> mtx(2, 2, { 1.99, 2.44, -3.999, -1.51 });
	delete mtx.jit; // Turning jit off 
	mtx.jit = nullptr;
	mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_CHECK(mtx.empty());
}

BOOST_AUTO_TEST_SUITE_END()