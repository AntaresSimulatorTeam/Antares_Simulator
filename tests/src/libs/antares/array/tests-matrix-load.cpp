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
BOOST_AUTO_TEST_SUITE(test_suite_1)

BOOST_AUTO_TEST_CASE(test_1)
{
	
	Matrix_mock_load_to_buffer<double, double> mtx(2, 2);

	// Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
	mtx.fake_matrix_to_load(2, 2, { 1.5, -2.44444, 3.66666, 0 });
	mtx.fake_mtx_to_be_loaded__set_precision(2);
	mtx.fake_mtx_to_be_loaded__print_dimensions(false);
	mtx.error_when_loading_fake_mtx(IO::errNone);

	Clob* buffer = new Clob;
	mtx.loadFromCSVFile("path/to/a/file", 2, 2, Matrix<double, double>::optNone, buffer);

	BOOST_REQUIRE_EQUAL(mtx.entry[0][0], 1.5);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][0], -2.44);
	BOOST_REQUIRE_EQUAL(mtx.entry[0][1], 3.67);
	BOOST_REQUIRE_EQUAL(mtx.entry[1][1], 0);
}

BOOST_AUTO_TEST_SUITE_END()


