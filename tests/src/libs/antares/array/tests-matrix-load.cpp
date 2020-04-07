#define BOOST_TEST_MODULE test-lib-antares-matrix tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include<array/tests-matrix-load.h>

#include <iostream> 

namespace utf = boost::unit_test;

using namespace Yuni;

/*
All loadFromCSVFile(...) entries :
	0. Global JIT::enabled state : on or off
	1. file to be loaded (= buffer) :
		a. empty
		b. contains the head banner 'size:nxm' or not
		c. contains digits (which precision) or integers.
		d. full of zeros,
		e. full of numbers (> 0 or < 0)
	2. Initial matrix state (matrix state when loadFromCSVFile(...) is called) :
		a. empty
		b. sized n x m
		c. state of the jit
	3. loadFromBuffer(...) method arguments :
		a. desired final matrix size (minWidth & maxHeight)
		b. options :
			+ optNone = 0,				//! None
			+ optFixedSize = 1,			//! The matrix can not see its size modified
			+ optQuiet = 2,				//! Do not produce warnings/errors
			+ optImmediate = 4,			//! Do not postpone the loading
			+ optMarkAsModified = 8,	//! mark the matrix as modified after loading
			+ optNoWarnIfEmpty = 16,	//! Do not warn if the file is empty
			+ optNeverFails = 32,		//! The loading never fails
	4. Error type returned by loadFromFileToBuffer(...)

Testing strategy :
	test effect on results of each previous element
	- 2.a and 2.b sould not have any effect (to be tested)

Tests ideas :
	- test of loadFromCSVFile(...) return value
	- 
*/


// ================================
// ===  Matrix<double, double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__load_from_double)

// 1.a.
BOOST_AUTO_TEST_CASE(fake_file_is_empty___target_matrix_has_only_0s)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Clob* fake_buffer = new Clob;	// Empty buffer

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx(2, 2);
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<>::optNone, fake_buffer);

	BOOST_CHECK(mtx.containsOnlyZero());
}

// 1.b.
BOOST_AUTO_TEST_CASE(fake_file_with_banner__target_mtx_empty___mtx_gets_file_dimensions)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.4444, 3.66666, 0, 8.559, -5.5555 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(1);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.4);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.7);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 0);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 8.6);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], -5.6);
}

// 1.c.
BOOST_AUTO_TEST_CASE(fake_file_precision_is_4___matrix_precision_gets_4)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(3, 1, { 1.5554, -2.4444, 3.66666 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 1, 3, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 3);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5554);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], -2.4444);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][2], 3.6667);
}

// 1.c.
BOOST_AUTO_TEST_CASE(fake_file_contains_int___matrix_precision_is_0)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<int, int> mtx_0(1, 4, { 1, -2, 3, -4 });
	fake_fuffer_factory<int, int> buffer_factory_ii;
	buffer_factory_ii.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_ii.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_ii.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 4, 1, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.height, 1);
	BOOST_REQUIRE_EQUAL(mtx.width, 4);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.);
	BOOST_REQUIRE_EQUAL(mtx.entry[3][0], -4.);
}

// 1.d.
BOOST_AUTO_TEST_CASE(fake_file_full_0s__load_mtx___mtx_contains_only_0s)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, {0,0,0,0,0,0});
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 3, 2, Matrix<>::optNone, fake_buffer);

	BOOST_CHECK(mtx.containsOnlyZero());
}

// 1.e.
BOOST_AUTO_TEST_CASE(fake_file_not_empty__target_mtx_empty___mtx_gets_file_dimensions)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.44444, 3.66666, 0, 8.559, -5.5555 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(2);
	buffer_factory_dd.print_dimensions(false);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.44);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.67);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 0);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 8.56);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], -5.56);
}

// 2.

// 3.a.
BOOST_AUTO_TEST_CASE(fake_file_empty__mtx_resized_to_0x2___mtx_cleared)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Clob* fake_buffer = new Clob;	// Empty buffer

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 0, 2, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.empty(), true);
}

// 3.a.
BOOST_AUTO_TEST_CASE(file_size_3x2__mtx_resized_to_5x7___mtx_still_5x7_but_contains_only_0s)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.44444, 3.66666, 0.9, 8.559, -5.5555 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 5, 7, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 5);
	BOOST_REQUIRE_EQUAL(mtx.height, 7);
	BOOST_CHECK(mtx.containsOnlyZero());
}

// 3.a.
BOOST_AUTO_TEST_CASE(file_size_3x3__mtx_resized_to_2x2___mtx_only_column_resized__coefs_preserved)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(3, 3, { 1., -2., 3., 0., 8., -5., 6., -7., 12. });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 0.);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 8.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], -5);
}

// 3.a. // 3.b.
BOOST_AUTO_TEST_CASE(file_bigger_than_mtx__mtx_has_a_fixed_size___mtx_keeps_size_but_is_filled_with_0s)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(4, 4,	{	1., -2., 3., -15.,
														8., -5., 6., -7.,
														12., 10., -20., 30.,
														-150., 80., -50., 60.});
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 3, 3, Matrix<>::optFixedSize, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 3);
	BOOST_CHECK(mtx.containsOnlyZero());
}


// 3.b.
BOOST_AUTO_TEST_CASE(loading_option_to_none___target_mtx_not_loaded_but_pointed_to_by_jit)
{
	global_JIT_manager global_JIT_to(true);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(1, 3, { 1., -2., 3. });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 0);
	BOOST_REQUIRE_EQUAL(mtx.height, 0);
	BOOST_CHECK((bool)(mtx.jit));
	BOOST_REQUIRE_EQUAL(mtx.jit->sourceFilename, "path/to/a/file");
}


// 3.b.
BOOST_AUTO_TEST_CASE(loading_option_to_immediate___target_mtx_loaded_but_not_pointed_to_by_jit)
{
	global_JIT_manager global_JIT_to(true);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(1, 3, { 1.55, -2., 3.11 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(2);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optImmediate, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.55);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.11);
	BOOST_CHECK(not mtx.jit);
}

// 3.b.
BOOST_AUTO_TEST_CASE(loading_option_to_immediate_and_fixed_size___target_mtx_loaded_and_pointed_to_by_jit)
{
	global_JIT_manager global_JIT_to(true);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(1, 3, { 1.55, -2., 3.11 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(2);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optImmediate | Matrix<>::optFixedSize, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.55);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.11);
	BOOST_CHECK(mtx.jit);
	BOOST_REQUIRE_EQUAL(mtx.jit->sourceFilename, "path/to/a/file");
}

// 4.
BOOST_AUTO_TEST_CASE(err_not_found_when_loading___log_is_ok)
{
	Clob* fake_buffer = new Clob;

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNotFound);

	// option : none
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: not found: 'path/to/a/file'");

	// option : quiet
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 2, 5, Matrix<>::optQuiet, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");
}

// 4.
BOOST_AUTO_TEST_CASE(err_memory_limit_when_loading___log_is_ok)
{
	Clob* fake_buffer = new Clob;

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errMemoryLimit);

	// option : none
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 7, Matrix<>::optNone, fake_buffer);
	string logs_to_get = "path/to/a/file: The file is too large (>" + to_string(filesizeHardLimit / 1024 / 1024) + "Mo)";
	BOOST_REQUIRE_EQUAL(logs.error().content(), logs_to_get);

	// option : quiet
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optQuiet, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");
}

// 4.
BOOST_AUTO_TEST_CASE(err_unknown_when_loading___log_is_ok)
{
	Clob* fake_buffer = new Clob;

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errUnknown);

	// option : none
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 7, Matrix<>::optNone, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: failed to load 'path/to/a/file'");

	// option : quiet
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optQuiet, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");
}

BOOST_AUTO_TEST_SUITE_END()


// ============================
// ====  Matrix<int, int>  ====
// ============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__load_from_int)

// 1.c.
BOOST_AUTO_TEST_CASE(fake_file_contain_doubles___loading_to_target_matrix_rounds_each_coef_to_floor)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(4, 1, { 1.5252, -2.1111, 3.9999, -4.5101 });
	fake_fuffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<int, int> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 4, 1, Matrix<int, int>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.height, 4);
	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], -2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][2], 3);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][3], -4);
}

BOOST_AUTO_TEST_SUITE_END()