// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - lib - antares - matrix tests

#define WIN32_LEAN_AND_MEAN

#include "tests-matrix-load.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

using BufferType = MatrixIO::BufferType;
using std::string;

/*
All loadFromCSVFile(...) entries (some directions to test this big method):
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
        3. loadFromBuffer(...) method arguments :
                a. desired final matrix size (minWidth & maxHeight)
                b. options :
                        + optNone = 0,				//! None
                        + optFixedSize = 1,			//! The matrix can not see its size
modified
                        + optQuiet = 2,				//! Do not produce warnings/errors
                        + optNoWarnIfEmpty = 16,	//! Do not warn if the file is empty
                        + optNeverFails = 32,		//! The loading never fails
        4. Error type returned by loadFromFileToBuffer(...)
*/

// ================================
// ===  Matrix<double>  ===
// ================================
BOOST_AUTO_TEST_SUITE(coeffs_are_double__load_from_double)

// 1.a.
BOOST_AUTO_TEST_CASE(fake_file_is_empty___target_matrix_has_only_0s)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    BufferType* fake_buffer = new BufferType; // Empty buffer

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx(2, 2);
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 2, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(mtx.containsOnlyZero());
}

// 1.b.
BOOST_AUTO_TEST_CASE(fake_file_with_banner__target_mtx_empty___mtx_gets_file_dimensions)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 3, {1.5, -2.4444, 3.66666, 0, 8.559, -5.5555});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(1);
    buffer_factory_dd.print_dimensions(true);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.5);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2.4);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.7);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 0);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 8.6);
    BOOST_REQUIRE_EQUAL(mtx[2][1], -5.6);
}

// 1.c.
BOOST_AUTO_TEST_CASE(fake_file_precision_is_4___matrix_precision_gets_4)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(3, 1, {1.5554, -2.4444, 3.66666});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(4);
    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 1, 3, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 3);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.5554);
    BOOST_REQUIRE_EQUAL(mtx[0][1], -2.4444);
    BOOST_REQUIRE_EQUAL(mtx[0][2], 3.6667);
}

// 1.c.
BOOST_AUTO_TEST_CASE(fake_file_contains_int___matrix_precision_is_0)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<int> mtx_0(1, 4, {1, -2, 3, -4});
    fake_buffer_factory<int> buffer_factory_ii;
    buffer_factory_ii.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_ii.print_dimensions(true);

    BufferType* fake_buffer = buffer_factory_ii.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 4, 1, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.height(), 1);
    BOOST_REQUIRE_EQUAL(mtx.width(), 4);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2.);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.);
    BOOST_REQUIRE_EQUAL(mtx[3][0], -4.);
}

// 1.d.
BOOST_AUTO_TEST_CASE(fake_file_full_0s__load_mtx___mtx_contains_only_0s)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 3, {0, 0, 0, 0, 0, 0});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 3, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(mtx.containsOnlyZero());
}

// 1.e.
BOOST_AUTO_TEST_CASE(fake_file_not_empty__target_mtx_empty___mtx_gets_file_dimensions)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 3, {1.5, -2.44444, 3.66666, 0, 8.559, -5.5555});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(2);
    buffer_factory_dd.print_dimensions(false);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.5);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2.44);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.67);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 0);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 8.56);
    BOOST_REQUIRE_EQUAL(mtx[2][1], -5.56);
}

// Specific tests for renewable TS
// Expected behavior : read 4 digits
BOOST_AUTO_TEST_CASE(fake_file_double_renewable)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 3, {100.5111, -2.44444, 3.66666, 0, 8.559, -5.5555});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(4);
    buffer_factory_dd.print_dimensions(false);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 100.5111);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2.4444);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.6667);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 0);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 8.559);
    BOOST_REQUIRE_EQUAL(mtx[2][1], -5.5555);
}

// Specific tests for thermal TS
// Expected behavior : read 0 digits, i.e round input to closest integer value
BOOST_AUTO_TEST_CASE(fake_file_double_thermal)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 3, {1.50001, -2.44444, 3.66666, 0, 8.559, -5.55555});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(0); // default precision is 0
    buffer_factory_dd.print_dimensions(false);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 2);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 4);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 0);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 9);
    BOOST_REQUIRE_EQUAL(mtx[2][1], -6);
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_alphabetic_char___load_fails_with_warning)
{
    BufferType* fake_buffer = new BufferType;
    fake_buffer->append("1.3\tHello\n");

    Matrix_mock_load_to_buffer<double> mtx;
    logs.warning().clear();
    BOOST_CHECK(not MatrixIO::load(mtx, "path/to/a/file", 2, 1, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(logs.warning().contains("Invalid numeric value"));
}

// 1.f.
BOOST_AUTO_TEST_CASE(
  binary_file___detect_encoding_when_loading_buffer_is_KO_and_is_probably_dead_code)
{
    // CAUTION :
    //	In load from buffer function, the code in charge to detect encoding (ex : little endian
    // utf-16, ...) 	is dead code. It is be used, but does not seem to detect anything.
    // Anyway, binary file are not used in Antares.

    // Creating an utf-16 binary file -----------------------------------------------------------
    // ... UTF-16le data (if host system is little-endian)
    char16_t utf16le[4] = {0x007a, // latin small letter 'z' U+007a
                           0x6c34, // CJK ideograph "water"  U+6c34
                           0xd834,
                           0xdd0b}; // musical sign segno U+1d10b
    // ... store in a file
    std::ofstream fout("text.txt");
    fout.write(reinterpret_cast<char*>(utf16le), sizeof utf16le);
    fout.close();
    // ------------------------------------------------------------------------------------------

    BufferType* fake_buffer = new BufferType;

    Matrix<double> mtx;
    BOOST_CHECK(not MatrixIO::load(mtx, "text.txt", 2, 2, Matrix<>::optNone));

    delete fake_buffer;

    remove("text.txt");
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_only_charriot_return__load_fails_with_warning)
{
    BufferType* fake_buffer = new BufferType;
    fake_buffer->append("\n\n");

    Matrix_mock_load_to_buffer<double> mtx;
    logs.warning().clear();
    BOOST_CHECK(not MatrixIO::load(mtx, "path/to/a/file", 2, 1, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(logs.warning().contains("Invalid format: The file seems empty"));
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_only_tabs__option_no_failure___load_fails_with_warning)
{
    BufferType* fake_buffer = new BufferType;
    fake_buffer->append("\t\t\t\t");

    Matrix_mock_load_to_buffer<double> mtx;
    logs.warning().clear();
    BOOST_CHECK(
      not MatrixIO::load(mtx, "path/to/a/file", 2, 1, Matrix<>::optNeverFails, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(logs.warning().contains("Invalid format: The file seems empty"));
}

// 1.f.
BOOST_AUTO_TEST_CASE(
  file_with_no_charriot_return__option_no_failure___load_succeeds_with_warning__column_resized)
{
    BufferType* fake_buffer = new BufferType;
    fake_buffer->append("1.1\t2.2\t3.3\t4.4\t");

    Matrix_mock_load_to_buffer<double> mtx;
    logs.warning().clear();
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 2, 2, Matrix<>::optNeverFails, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(logs.warning().content() == "path/to/a/file: not enough rows (expected 2, got 1)");

    BOOST_REQUIRE_EQUAL(mtx.width(), 4);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.1);
    BOOST_REQUIRE_EQUAL(mtx[1][0], 2.2);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.3);
    BOOST_REQUIRE_EQUAL(mtx[3][0], 4.4);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 0.);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 0.);
    BOOST_REQUIRE_EQUAL(mtx[2][1], 0.);
    BOOST_REQUIRE_EQUAL(mtx[3][1], 0.);
}

// 1.f.
BOOST_AUTO_TEST_CASE(
  file_with_rows_of_different_size___load_succeeds__column_resized__0_on_missing_coef)
{
    BufferType* fake_buffer = new BufferType;
    // 5.2  6.1   -
    // 1.3  4.5  9.7
    fake_buffer->append("5.2\t6.1\n1.3\t4.5\t9.7\n");

    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 2, 2, Matrix<>::optNeverFails, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 5.2);
    BOOST_REQUIRE_EQUAL(mtx[1][0], 6.1);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 0.);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 1.3);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 4.5);
    BOOST_REQUIRE_EQUAL(mtx[2][1], 9.7);
}

// 1.f.
BOOST_AUTO_TEST_CASE(file_with_columns_of_different_size___load_succeeds__row_not_resized)
{
    BufferType* fake_buffer = new BufferType;
    // 5.2  6.1
    // 1.3  4.5
    // 9.7   -
    fake_buffer->append("5.2\t6.1\n1.3\t4.5\n9.7\n");

    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 2, 2, Matrix<>::optNeverFails, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 2);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);

    BOOST_REQUIRE_EQUAL(mtx[0][0], 5.2);
    BOOST_REQUIRE_EQUAL(mtx[1][0], 6.1);

    BOOST_REQUIRE_EQUAL(mtx[0][1], 1.3);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 4.5);
}

// 1.f.
BOOST_AUTO_TEST_CASE(
  file_has_invalid_header__option_do_not_fail____load_succeeds__column_resized_to_1__0s_everywhere)
{
    BufferType* fake_buffer = new BufferType;
    fake_buffer->append("size:0x0");

    Matrix_mock_load_to_buffer<double> mtx;
    logs.warning().clear();
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 2, 5, Matrix<>::optNeverFails, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(logs.warning().contains("Invalid header"));

    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 5);
    BOOST_CHECK(mtx.containsOnlyZero());
}

// 2.

// 3.a.
BOOST_AUTO_TEST_CASE(fake_file_empty__mtx_resized_to_0x2___mtx_cleared)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    BufferType* fake_buffer = new BufferType; // Empty buffer

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 0, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(mtx.empty());
}

// 3.a.
BOOST_AUTO_TEST_CASE(
  file_size_3x2__mtx_resized_to_5x7___mtx_still_5x7_but_contains_only_0s__load_fails)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 3, {1.5, -2.44444, 3.66666, 0.9, 8.559, -5.5555});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(not MatrixIO::load(mtx, "path/to/a/file", 5, 7, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 5);
    BOOST_REQUIRE_EQUAL(mtx.height(), 7);
    BOOST_CHECK(mtx.containsOnlyZero());
}

// 3.a.
BOOST_AUTO_TEST_CASE(file_size_3x3__mtx_resized_to_1x2___mtx_column_resized_to_3__coefs_preserved)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(3, 3, {1., -2., 3., 0., 8., -5., 6., -7., 12.});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 0.);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2.);
    BOOST_REQUIRE_EQUAL(mtx[1][1], 8.);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.);
    BOOST_REQUIRE_EQUAL(mtx[2][1], -5);
}

// 3.a. // 3.b.
BOOST_AUTO_TEST_CASE(
  file_bigger_than_mtx__mtx_has_a_fixed_size___mtx_keeps_size_but_is_filled_with_0s__load_fails)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(
      4,
      4,
      {1., -2., 3., -15., 8., -5., 6., -7., 12., 10., -20., 30., -150., 80., -50., 60.});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    logs.warning().clear();
    BOOST_CHECK(
      not MatrixIO::load(mtx, "path/to/a/file", 3, 3, Matrix<>::optFixedSize, fake_buffer));

    delete fake_buffer;

    BOOST_CHECK(logs.warning().contains("Invalid format: Too many entry for the row"));

    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 3);
    BOOST_CHECK(mtx.containsOnlyZero());
}

// 3.a. // 3.b.
BOOST_AUTO_TEST_CASE(
  file_bigger_than_mtx__mtx_fixed_size__load_should_never_fail___load_succeeds__column_resized)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(3, 3, {1., -2., 3., 8., -5., 6., 12., 10., -20.});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx,
                               "path/to/a/file",
                               2,
                               2,
                               Matrix<>::optFixedSize | Matrix<>::optNeverFails,
                               fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 8.);
    BOOST_REQUIRE_EQUAL(mtx[1][0], -2.);
    BOOST_REQUIRE_EQUAL(mtx[1][1], -5.);
    BOOST_REQUIRE_EQUAL(mtx[2][0], 3.);
    BOOST_REQUIRE_EQUAL(mtx[2][1], 6.);
}

// 4.
BOOST_AUTO_TEST_CASE(err_not_found_when_loading___log_is_ok)
{
    BufferType* fake_buffer = new BufferType;

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    const MatrixIO::FileLoader fileLoader = [](MatrixIO::BufferType&, const std::string&)
    { return MatrixIO::FileLoadError::notFound; };

    // option : none
    logs.error().clear();
    BOOST_CHECK(
      not MatrixIO::load(mtx, "path/to/a/file", 0, 0, Matrix<>::optNone, fake_buffer, fileLoader));
    BOOST_REQUIRE_EQUAL(logs.error().content(), "I/O Error: not found: 'path/to/a/file'");

    // option : quiet
    logs.error().clear();
    BOOST_CHECK(
      not MatrixIO::load(mtx, "path/to/a/file", 2, 5, Matrix<>::optQuiet, fake_buffer, fileLoader));
    BOOST_REQUIRE_EQUAL(logs.error().content(), "");

    delete fake_buffer;
}

// 4.
BOOST_AUTO_TEST_CASE(err_too_large_when_loading___log_is_ok)
{
    const auto filename = std::filesystem::path("matrix-too-large.txt");
    std::ofstream file(filename);
    BOOST_REQUIRE(file);
    file.close();

    std::error_code ec;
    std::filesystem::resize_file(filename, 1536ULL * 1024ULL * 1024ULL + 1, ec);
    BOOST_REQUIRE(!ec);

    Matrix_mock_load_to_buffer<double> mtx;

    logs.error().clear();
    BOOST_CHECK(not MatrixIO::load(mtx, filename.string(), 3, 7));
    BOOST_REQUIRE_EQUAL(logs.error().content(),
                        "matrix-too-large.txt: The file is too large (>1536Mo)");

    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 7);
    BOOST_CHECK(mtx.containsOnlyZero());

    logs.error().clear();
    BOOST_CHECK(not MatrixIO::load(mtx, filename.string(), 3, 1, Matrix<>::optQuiet));
    BOOST_REQUIRE_EQUAL(logs.error().content(), "");

    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 1);
    BOOST_CHECK(mtx.containsOnlyZero());

    std::filesystem::remove(filename);
}

// 4.
BOOST_AUTO_TEST_CASE(file_loader_successfully_replaces_input_buffer)
{
    BufferType buffer = "ignored";
    Matrix_mock_load_to_buffer<double> mtx;
    bool loaderCalled = false;
    const MatrixIO::FileLoader fileLoader =
      [&loaderCalled](MatrixIO::BufferType& input, const std::string& filename)
    {
        loaderCalled = filename == "path/to/a/file";
        input = "1.5\n2.5\n";
        return MatrixIO::FileLoadError::none;
    };

    BOOST_CHECK(
      MatrixIO::load(mtx, "path/to/a/file", 1, 2, Matrix<>::optNone, &buffer, fileLoader));
    BOOST_CHECK(loaderCalled);
    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1.5);
    BOOST_REQUIRE_EQUAL(mtx[0][1], 2.5);
}

BOOST_AUTO_TEST_CASE(input_buffer_is_preserved_when_loading_fails)
{
    BufferType buffer("\xFF\xFE", 2);
    const BufferType original = buffer;
    Matrix_mock_load_to_buffer<double> mtx;

    BOOST_CHECK(!MatrixIO::load(mtx, "path/to/a/file", 1, 1, Matrix<>::optNone, &buffer));
    BOOST_CHECK_EQUAL(buffer, original);
}

BOOST_AUTO_TEST_SUITE_END()

// ===============================
// ====  Matrix<std::string>  ====
// ===============================
BOOST_AUTO_TEST_SUITE(strings_are_loaded_without_numeric_conversion)

BOOST_AUTO_TEST_CASE(file_contains_text___loaded_text_is_preserved)
{
    Matrix<std::string> matrix;
    BufferType buffer = "alpha\tbeta gamma\n42\t-7\n";

    BOOST_REQUIRE(MatrixIO::load(matrix, "path/to/a/file", 0, 0, Matrix<>::optNone, &buffer));

    BOOST_REQUIRE_EQUAL(matrix.width(), 2);
    BOOST_REQUIRE_EQUAL(matrix.height(), 2);
    BOOST_CHECK_EQUAL(matrix[0][0], "alpha");
    BOOST_CHECK_EQUAL(matrix[1][0], "beta gamma");
    BOOST_CHECK_EQUAL(matrix[0][1], "42");
    BOOST_CHECK_EQUAL(matrix[1][1], "-7");
}

BOOST_AUTO_TEST_SUITE_END()

// ============================
// ====  Matrix<int>  ====
// ============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__load_from_int)

// 1.c.
BOOST_AUTO_TEST_CASE(file_contains_digits___loading_to_target_matrix_rounds_each_coef_to_floor)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 1, {1.5252, -2.1111});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(4);
    buffer_factory_dd.print_dimensions(true);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<int> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1);
    BOOST_REQUIRE_EQUAL(mtx[0][1], -2);
}

BOOST_AUTO_TEST_CASE(out_of_range_and_non_finite_fallback_values_are_initialized)
{
    BufferType buffer = "1e100\t-1e100\tnan\n";
    Matrix_mock_load_to_buffer<int> mtx;

    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 3, 1, Matrix<>::optNeverFails, &buffer));
    BOOST_REQUIRE_EQUAL(mtx.width(), 3);
    BOOST_REQUIRE_EQUAL(mtx.height(), 1);
    BOOST_CHECK_EQUAL(mtx[0][0], 0);
    BOOST_CHECK_EQUAL(mtx[1][0], 0);
    BOOST_CHECK_EQUAL(mtx[2][0], 0);
}

BOOST_AUTO_TEST_CASE(file_contains_int___loaded_coefs_are_int)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<int> mtx_0(2, 1, {102, -54});
    fake_buffer_factory<int> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(4);
    buffer_factory_dd.print_dimensions(true);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<int> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 102);
    BOOST_REQUIRE_EQUAL(mtx[0][1], -54);
}

BOOST_AUTO_TEST_SUITE_END()

// ===============================
// ====  Matrix<int>  ====
// ===============================
BOOST_AUTO_TEST_SUITE(coeffs_are_int__load_from_digits)

// 1.c.
BOOST_AUTO_TEST_CASE(file_contains_digits___loading_to_target_matrix_rounds_each_coef_to_floor)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 1, {1.5252, -2.1111});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(4);
    buffer_factory_dd.print_dimensions(true);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<int> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 1);
    BOOST_REQUIRE_EQUAL(mtx[0][1], -2);
}

BOOST_AUTO_TEST_SUITE_END()

// ===============================
// ====  Matrix<double>  ====
// ===============================
BOOST_AUTO_TEST_SUITE(coeffs_are_digits__load_as_double)

// 1.c.
BOOST_AUTO_TEST_CASE(file_contains_digits___loaded_coefs_are_stored_as_digits)
{
    // Creating a buffer mocking the result of : IO::File::LoadFromFile(...)
    Matrix_easy_to_fill<double> mtx_0(2, 1, {12.9, -23.2});
    fake_buffer_factory<double> buffer_factory_dd;
    buffer_factory_dd.matrix_to_build_buffer_with(&mtx_0);
    buffer_factory_dd.set_precision(4);
    buffer_factory_dd.print_dimensions(true);

    BufferType* fake_buffer = buffer_factory_dd.build_buffer();

    // Testing load
    Matrix_mock_load_to_buffer<double> mtx;
    BOOST_CHECK(MatrixIO::load(mtx, "path/to/a/file", 1, 2, Matrix<>::optNone, fake_buffer));

    delete fake_buffer;

    BOOST_REQUIRE_EQUAL(mtx.width(), 1);
    BOOST_REQUIRE_EQUAL(mtx.height(), 2);
    BOOST_REQUIRE_EQUAL(mtx[0][0], 12.9);
    BOOST_REQUIRE_EQUAL(mtx[0][1], -23.2);
}

BOOST_AUTO_TEST_SUITE_END()
