#define BOOST_TEST_MODULE test-lib-antares-matrix tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include<matrix.h>
#include<yuni/core/system/stdint.h>

#include<string>
#include<vector>
using namespace std;
using namespace Yuni;
using namespace Antares;

namespace utf = boost::unit_test;

using namespace std;



struct TSNumbersPredicate
{
	uint32 operator () (uint32 value) const
	{
		return value + 1;
	}
};

template<class T = double, class ReadWriteT = T>
class mtx_fixture
{
public:
	mtx_fixture(uint height, uint width, const vector<T>& vec)
	{
		BOOST_REQUIRE_EQUAL(height * width, vec.size());
		mtx.reset(height, width, true);
		uint count = 0;
		for (uint j = 0; j < height; j++)
			for (uint i = 0; i < width; i++)
			{
				mtx.entry[i][j] = vec[count];
				count++;
			}
	}
	mtx_fixture(uint height, uint width)
	{
		mtx.reset(height, width, true);
	}

public:
	Matrix<T, ReadWriteT> mtx;
};



// ================================
// ===  Matrix<double, double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__save_into_double)
BOOST_AUTO_TEST_CASE(matrix_only_0s_and_no_addHint___result_is_empty)
{
	mtx_fixture<double, double> mf(2, 2);
	mf.mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "");
}

BOOST_AUTO_TEST_CASE(matrix_only_0s_addHint__get_only_a_title_and_the_0s)
{
	mtx_fixture<double, double> mf(2, 2);
	mf.mtx.saveToCSVFile("path/to/an/output/file", 0, true);
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "size:2x2\n0\t0\n0\t0\n");
}

BOOST_AUTO_TEST_CASE(coeffs_have_int_values__no_zeros_after_decimal_point)
{
	mtx_fixture<double, double> mf(2, 2, {1, 0, 0, 2});
	mf.mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "1\t0\n0\t2\n");
}

BOOST_AUTO_TEST_CASE(precision_is_3__get_3_nbs_after_decimal_point)
{
	mtx_fixture<double, double> mf(2, 2, {1.5, 2.44444, 3.66666, 0});
	mf.mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "1.500\t2.444\n3.667\t0\n");
}

BOOST_AUTO_TEST_CASE(precision_has_no_effect_on_integer_values)
{
	// Any whole number is printed without decimal point
	mtx_fixture<double, double> mf(2, 2, {1, 5, 0, 3});
	mf.mtx.saveToCSVFile("path/to/an/output/file", 4, false);
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "1\t5\n0\t3\n");
}

BOOST_AUTO_TEST_CASE(add_predicate__each_coeff_is_one_incremented)
{
	mtx_fixture<double, double> mf(2, 2, {1, 0, 0, 2});
	TSNumbersPredicate predicate;
	mf.mtx.saveToCSVFile("path/to/an/output/file", 2, false, predicate);
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "2\t1\n1\t3\n");
}

BOOST_AUTO_TEST_CASE(TS_nb_predicate_suits_int_matrices__It_converts_double_into_unsigned_ints)
{
	// Negative numbers are turned into strange positive numbers because the TS number
	// predicate converted them into unsigned int
	mtx_fixture<double, double> mf(2, 2, {1.5, -3.552, 0.66, 2.3});
	TSNumbersPredicate predicate;
	mf.mtx.saveToCSVFile("path/to/an/output/file", 2, false, predicate);
	string res = mf.mtx.data.to<string>();
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "2\t4294967294\n1\t3\n");
}
BOOST_AUTO_TEST_SUITE_END()



// =============================
// ===  Matrix<int, double>  ===
// =============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__save_into_double)
BOOST_AUTO_TEST_CASE(matrix_only_0s_and_no_addHint___result_is_empty)
{
	mtx_fixture<int, double> mf(2, 2);
	mf.mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "");
}

BOOST_AUTO_TEST_CASE(any_whole_number_is_printed_without_decimal_point)
{
	mtx_fixture<int, double> mf(2, 2, {10, 500, 0, 3});
	mf.mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "10\t500\n0\t3\n");
}
BOOST_AUTO_TEST_SUITE_END()



// ==========================
// ===  Matrix<int, int>  ===
// ==========================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__save_into_int)
BOOST_AUTO_TEST_CASE(matrix_only_0s_and_no_addHint___result_is_empty)
{
	mtx_fixture<int, int> mf(2, 2);
	mf.mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "");
}

BOOST_AUTO_TEST_CASE(first_matrix__int_to_int)
{
	mtx_fixture<int, int> mf(2, 2, {1000, -5000, 0, 3000});
	mf.mtx.saveToCSVFile("path/to/an/output/file");
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "1000\t-5000\n0\t3000\n");
}
BOOST_AUTO_TEST_SUITE_END()


// =============================
// ===  Matrix<double, int>  ===
// =============================
BOOST_AUTO_TEST_CASE(precision_is_3__get_coeffs_floor_integers)
{
	mtx_fixture<double, int> mf(2, 2, {1.99, 2.44, -3.999, -1.51});
	mf.mtx.saveToCSVFile("path/to/an/output/file", 3, false);
	BOOST_REQUIRE_EQUAL(mf.mtx.data.to<string>(), "1\t2\n-3\t-1\n");
}