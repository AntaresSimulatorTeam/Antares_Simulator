#define BOOST_TEST_MODULE test-lib-antares-matrix tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include<array/tests-matrix-load.h>

#include <iostream> 

namespace utf = boost::unit_test;

using namespace Yuni;


// To do :
// - test jit things : 
//		jit update when loading matrix.
//		But for this, JIT::enabled turned to true.
// - test depending on the fake file structure :
//		+ the file contains the head banner 'size:nxm'
//		+ the file contains digits (which precision), integers, zeros.

// ================================
// ===  Matrix<double, double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__load_from_double)

fake_fuffer_factory<double, double> buffer_factory;

BOOST_AUTO_TEST_CASE(fake_file_is_empty___loaded_matrix_has_only_0s)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0;
	buffer_factory.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory.set_precision(0);
	buffer_factory.print_dimensions(false);

	Clob* fake_buffer = buffer_factory.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx(2, 2);
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<double, double>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.containsOnlyZero(), true);
}

BOOST_AUTO_TEST_CASE(fake_file_not_empty__mtx_is_empty___mtx_get_file_sizes)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 3, { 1.5, -2.44444, 3.66666, 0, 8.559, -5.5555 });
	buffer_factory.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory.set_precision(2);
	buffer_factory.print_dimensions(false);

	Clob* fake_buffer = buffer_factory.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<double, double>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.height, 2);
	BOOST_REQUIRE_EQUAL(mtx.width, 3);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.44);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][0], 3.67);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 0);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 8.56);
	BOOST_REQUIRE_EQUAL(mtx.entry[2][1], -5.56);
}

BOOST_AUTO_TEST_CASE(err_not_found_when_loading___log_is_ok)
{
	Clob* fake_buffer = new Clob;

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errNotFound);

	// option : none
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 0, 0, Matrix<double, double>::optNone, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: not found: 'path/to/a/file'");

	// option : quiet
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 2, 5, Matrix<double, double>::optQuiet, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");
}

BOOST_AUTO_TEST_CASE(err_memory_limit_when_loading___log_is_ok)
{
	Clob* fake_buffer = new Clob;

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errMemoryLimit);

	// option : none
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 7, Matrix<double, double>::optNone, fake_buffer);
	string logs_to_get = "path/to/a/file: The file is too large (>" + to_string(filesizeHardLimit / 1024 / 1024) + "Mo)";
	BOOST_REQUIRE_EQUAL(logs.error().content(), logs_to_get);

	// option : quiet
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<double, double>::optQuiet, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");
}

BOOST_AUTO_TEST_CASE(err_unknown_when_loading___log_is_ok)
{
	Clob* fake_buffer = new Clob;

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx;
	mtx.error_when_loading_from_file(IO::errUnknown);

	// option : none
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 7, Matrix<double, double>::optNone, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: failed to load 'path/to/a/file'");

	// option : quiet
	logs.error().clear();
	mtx.loadFromCSVFile("path/to/a/file", 3, 1, Matrix<double, double>::optQuiet, fake_buffer);
	BOOST_REQUIRE_EQUAL(logs.error().content(), "");
}

// errUnknown

BOOST_AUTO_TEST_CASE(test_1)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	Matrix_easy_to_fill<double, double> mtx_0(2, 2, { 1.5, -2.44444, 3.66666, 0 });
	buffer_factory.matrix_to_build_buffer_with(&mtx_0);
	buffer_factory.set_precision(2);
	buffer_factory.print_dimensions(false);

	Clob* fake_buffer = buffer_factory.build_buffer();

	// Testing load
	Matrix_mock_load_to_buffer<double, double> mtx(2, 2);
	mtx.error_when_loading_from_file(IO::errNone);
	mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<double, double>::optNone, fake_buffer);

	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.44);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 3.67);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 0);
}

BOOST_AUTO_TEST_SUITE_END()


