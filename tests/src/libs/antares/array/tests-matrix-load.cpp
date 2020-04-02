#define BOOST_TEST_MODULE test-lib-antares-matrix tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include<tests-matrix-load.h>

#include <iostream> 

namespace utf = boost::unit_test;

using namespace Yuni;


// ================================
// ===  Matrix<double, double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__load_from_double)


BOOST_AUTO_TEST_CASE(fake_file_is_empty___loaded_matrix_has_only_0s)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	fake_fuffer_factory<double, double> buffer_factory;
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
	fake_fuffer_factory<double, double> buffer_factory;
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

// gp : tester lorsque le buffer contient size:nxm
// gp : tester les histoires de jit : mise à jour du jit quand on charge la matrice
//		Mais pour ça il faut mettre JIT::enabled à true

BOOST_AUTO_TEST_CASE(test_1)
{
	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	fake_fuffer_factory<double, double> buffer_factory;
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


