#define BOOST_TEST_MODULE test-lib-antares-matrix tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include "tests-matrix-load.h"

#include <iostream>
#include <stdio.h>

namespace utf = boost::unit_test;

using namespace Yuni;

/*
All loadFromCSVFile(...) entries (some directions to test this big method):
	0. Global JIT::enabled state : on or off
	1. file to be loaded (= buffer) :
		a. empty
		b. contains the head banner 'size:nxm' or not
		c. contains digits (which precision) or integers.
		d. full of zeros,
		e. full of numbers (> 0 or < 0)
		f. strange cases
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
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(mtx.containsOnlyZero());
}

// 1.b.
BOOST_AUTO_TEST_CASE(fake_file_with_banner__target_mtx_empty___mtx_gets_file_dimensions)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.4444, 3.66666, 0, 8.559, -5.5555 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(1);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

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
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 1, 3, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

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
	fake_buffer_factory<int, int> buffer_factory_ii;
	buffer_factory_ii.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_ii.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_ii.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 4, 1, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

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
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 3, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(mtx.containsOnlyZero());
}

// 1.e.
BOOST_AUTO_TEST_CASE(fake_file_not_empty__target_mtx_empty___mtx_gets_file_dimensions)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.44444, 3.66666, 0, 8.559, -5.5555 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(2);
	buffer_factory_dd.print_dimensions(false);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.44);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.67);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 0);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 8.56);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], -5.56);
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_alphabetic_char___load_fails_with_warning)
{
	Clob* fake_buffer = new Clob;
	fake_buffer->append("1.3\tHello\n");

	Matrix_mock_load_to_buffer<double, double> mtx;
	logs.warning().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 2, 1, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(logs.warning().contains("Invalid numeric value"));
}

// 1.f.
BOOST_AUTO_TEST_CASE(binary_file___detect_encoding_when_loading_buffer_is_KO_and_is_probably_dead_code)
{
	// CAUTION :
	//	In load from buffer function, the code in charge to detect encoding (ex : little endian utf-16, ...)
	//	is dead code. It is be used, but does not seem to detect anything.
	//	Anyway, binary file are not used in Antares.

	// Creating an utf-16 binary file -----------------------------------------------------------
	// ... UTF-16le data (if host system is little-endian)
	char16_t utf16le[4] = { 0x007a,				// latin small letter 'z' U+007a
							0x6c34,				// CJK ideograph "water"  U+6c34
							0xd834, 0xdd0b };	// musical sign segno U+1d10b
	// ... store in a file
	std::ofstream fout("text.txt");
	fout.write(reinterpret_cast<char*>(utf16le), sizeof utf16le);
	fout.close();
	// ------------------------------------------------------------------------------------------

	Clob* fake_buffer = new Clob;

	Matrix<double, double> mtx;
	BOOST_CHECK(not mtx.loadFromCSVFile("text.txt", 2, 2, Matrix<>::optNone, fake_buffer));
	BOOST_CHECK(not fake_buffer->empty());

	delete fake_buffer;

	remove("text.txt");
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_only_charriot_return__load_fails_with_warning)
{
	Clob* fake_buffer = new Clob;
	fake_buffer->append("\n\n");

	Matrix_mock_load_to_buffer<double, double> mtx;
	logs.warning().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 2, 1, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(logs.warning().contains("Invalid format: The file seems empty"));
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_only_tabs__option_no_failure___load_fails_with_warning)
{
	Clob* fake_buffer = new Clob;
	fake_buffer->append("\t\t\t\t");

	Matrix_mock_load_to_buffer<double, double> mtx;
	logs.warning().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 2, 1, Matrix<>::optNeverFails, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(logs.warning().contains("Invalid format: The file seems empty"));
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_no_charriot_return__option_no_failure___load_succeeds_with_warning__column_resized)
{
	Clob* fake_buffer = new Clob;
	fake_buffer->append("1.1\t2.2\t3.3\t4.4\t");

	Matrix_mock_load_to_buffer<double, double> mtx;
	logs.warning().clear();
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<>::optNeverFails, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(logs.warning().content() == "path/to/a/file: not enough rows (expected 2, got 1)");

	BOOST_REQUIRE_EQUAL(mtx.width, 4);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.1);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], 2.2);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.3);
	BOOST_REQUIRE_EQUAL(mtx.entry[3][0], 4.4);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 0.);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 0.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], 0.);
	BOOST_REQUIRE_EQUAL(mtx.entry[3][1], 0.);
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_rows_of_different_size___load_succeeds__column_resized__0_on_missing_coef)
{
	Clob* fake_buffer = new Clob;
	// 5.2  6.1   -
	// 1.3  4.5  9.7
	fake_buffer->append("5.2\t6.1\n1.3\t4.5\t9.7\n");

	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<>::optNeverFails, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 5.2);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], 6.1);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 0.);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 1.3);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 4.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], 9.7);
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_columns_of_different_size___load_succeeds__row_not_resized)
{
	Clob* fake_buffer = new Clob;
	// 5.2  6.1
	// 1.3  4.5
	// 9.7   -
	fake_buffer->append("5.2\t6.1\n1.3\t4.5\n9.7\n");

	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<>::optNeverFails, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 2);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);

	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 5.2);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], 6.1);

	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 1.3);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 4.5);
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_has_invalid_header__option_do_not_fail____load_succeeds__column_resized_to_1__0s_everywhere)
{
	Clob* fake_buffer = new Clob;
	fake_buffer->append("size:0x0");

	Matrix_mock_load_to_buffer<double, double> mtx;
	logs.warning().clear();
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 2, 5, Matrix<>::optNeverFails, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(logs.warning().contains("Invalid header"));

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 5);
	BOOST_CHECK(mtx.containsOnlyZero());
}

// 2.

// 3.a.
BOOST_AUTO_TEST_CASE(fake_file_empty__mtx_resized_to_0x2___mtx_cleared)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Clob* fake_buffer = new Clob;	// Empty buffer

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 0, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(mtx.empty());
}

// 3.a.
BOOST_AUTO_TEST_CASE(file_size_3x2__mtx_resized_to_5x7___mtx_still_5x7_but_contains_only_0s__load_fails)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.44444, 3.66666, 0.9, 8.559, -5.5555 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 5, 7, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 5);
	BOOST_REQUIRE_EQUAL(mtx.height, 7);
	BOOST_CHECK(mtx.containsOnlyZero());
}

// 3.a.
BOOST_AUTO_TEST_CASE(file_size_3x3__mtx_resized_to_1x2___mtx_column_resized_to_3__coefs_preserved)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(3, 3, { 1., -2., 3., 0., 8., -5., 6., -7., 12. });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

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
BOOST_AUTO_TEST_CASE(file_bigger_than_mtx__mtx_has_a_fixed_size___mtx_keeps_size_but_is_filled_with_0s__load_fails)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(4, 4,	{	1., -2., 3., -15.,
														8., -5., 6., -7.,
														12., 10., -20., 30.,
														-150., 80., -50., 60.});
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	logs.warning().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 3, 3, Matrix<>::optFixedSize, fake_buffer));

	delete fake_buffer;

	BOOST_CHECK(logs.warning().contains("Invalid format: Too many entry for the row"));

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 3);
	BOOST_CHECK(mtx.containsOnlyZero());
}

// 3.a. // 3.b.
BOOST_AUTO_TEST_CASE(file_bigger_than_mtx__mtx_fixed_size__load_should_never_fail___load_succeeds__column_resized)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(3, 3, { 1., -2., 3., 8., -5., 6., 12., 10., -20.});
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<>::optFixedSize | Matrix<>::optNeverFails, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 8.);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], -5.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], 6.);
}

// 3.b.
BOOST_AUTO_TEST_CASE(loading_option_to_none___target_mtx_not_loaded_but_pointed_to_by_jit)
{
	global_JIT_manager global_JIT_to(true);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(1, 3, { 1., -2., 3. });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

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
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(2);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optImmediate, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.55);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.11);
	BOOST_CHECK(not mtx.jit);
}

// 3.b.
BOOST_AUTO_TEST_CASE(loading_option_to_immediate_and_fixed_size___target_mtx_loaded_and_pointed_to_by_jit)
{
	global_JIT_manager global_JIT_to(true);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(1, 3, { 1.55, -2., 3.11 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(2);
	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optImmediate | Matrix<>::optFixedSize, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.55);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.);
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
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));
	BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: not found: 'path/to/a/file'");

	// option : quiet
	logs.error().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 2, 5, Matrix<>::optQuiet, fake_buffer));
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");

	delete fake_buffer;
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
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 3, 7, Matrix<>::optNone, fake_buffer));
	string logs_to_get = "path/to/a/file: The file is too large (>" + to_string(filesizeHardLimit / 1024 / 1024) + "Mo)";
	BOOST_REQUIRE_EQUAL(logs.error().content(), logs_to_get);

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 7);
	BOOST_CHECK(mtx.containsOnlyZero());

	// option : quiet
	logs.error().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optQuiet, fake_buffer));
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");

	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.height, 1);
	BOOST_CHECK(mtx.containsOnlyZero());

	delete fake_buffer;
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
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 3, 7, Matrix<>::optNone, fake_buffer));
	BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: failed to load 'path/to/a/file'");

	// option : quiet
	logs.error().clear();
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<>::optQuiet, fake_buffer));
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");

	delete fake_buffer;
}

BOOST_AUTO_TEST_SUITE_END()


// ============================
// ====  Matrix<int, int>  ====
// ============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__load_from_int)

// 1.c.
BOOST_AUTO_TEST_CASE(file_contains_digits___loading_to_target_matrix_rounds_each_coef_to_floor)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 1, { 1.5252, -2.1111 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<int, int> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], -2);
}

BOOST_AUTO_TEST_CASE(file_contains_int___loaded_coefs_are_int)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<int, int> mtx_0(2, 1, { 102, -54 });
	fake_buffer_factory<int, int> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<int, int> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 102);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], -54);
}

BOOST_AUTO_TEST_SUITE_END()


// ===============================
// ====  Matrix<int, double>  ====
// ===============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__load_from_digits)

// 1.c.
BOOST_AUTO_TEST_CASE(file_contains_digits___loading_to_target_matrix_rounds_each_coef_to_floor)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 1, { 1.5252, -2.1111 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<int, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], -2);
}

BOOST_AUTO_TEST_SUITE_END()

// ===============================
// ====  Matrix<double, int>  ====
// ===============================
BOOST_AUTO_TEST_SUITE(coeffs_are_digits__load_from_int)

// 1.c.
BOOST_AUTO_TEST_CASE(file_contains_digits___loaded_coefs_are_rounded_to_floor_but_stored_as_digits)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 1, { 12.9, -23.2 });
	fake_buffer_factory<double, double> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory_dd.set_precision(4);
	buffer_factory_dd.print_dimensions(true);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<int, double> mtx;
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 12.);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], -23.);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(JIT_management)

BOOST_AUTO_TEST_CASE(mtx_is_marked_modified__load_is_done___mtx_no_more_modified)
{
	global_JIT_manager global_JIT_to(true);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<int, int> mtx_0(2, 1, { 12, -23 });
	fake_buffer_factory<int, int> buffer_factory_dd;
	buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);

	Clob* fake_buffer = buffer_factory_dd.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<int, int> mtx;
	mtx.jit = new JIT::Informations();	// Giving matrix a defaut jit information
	mtx.markAsModified();
	BOOST_CHECK(mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<>::optImmediate | Matrix<>::optMarkAsModified, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_CHECK(not mtx.jit->modified);
}

BOOST_AUTO_TEST_CASE(load_fails_so_jit_not_created_by_effective_loading___jit_created_after_load_because_size_fixed)
{
	global_JIT_manager global_JIT_to(true);

	Clob* fake_buffer = nullptr;

	// Testing load
	Matrix_mock_load_to_buffer<int, int> mtx;
	mtx.error_when_loading_from_file(IO::errNotFound);
	BOOST_CHECK(not mtx.loadFromCSVFile("path/to/a/file", 1, 2, Matrix<>::optImmediate | Matrix<>::optFixedSize, fake_buffer));

	delete fake_buffer;

	BOOST_REQUIRE_EQUAL(mtx.width, 1);
	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_CHECK(mtx.jit);
}


BOOST_AUTO_TEST_SUITE_END()