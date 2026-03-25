// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/InputError.h>
#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

using namespace std;
using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::DataSeriesCsvImporter;
using namespace Antares::Optimisation::LinearProblemDataImpl;

struct CsvCreationFixture
{
    filesystem::path temp_path;

    CsvCreationFixture()
    {
        temp_path = filesystem::temp_directory_path() / std::tmpnam(nullptr);
        filesystem::create_directories(temp_path);
    }

    std::filesystem::path writeFile(string filename, string content);

    ~CsvCreationFixture()
    {
        if (filesystem::exists(temp_path))
        {
            filesystem::remove_all(temp_path);
        }
    }
};

std::filesystem::path CsvCreationFixture::writeFile(const string filename, const string content)
{
    auto filepath = temp_path / (filename);
    std::ofstream outfile(filepath);
    outfile << content;
    outfile.close();
    return filepath;
}

BOOST_FIXTURE_TEST_SUITE(_DataSeriesImport_OneCsvFile_, CsvCreationFixture)

BOOST_AUTO_TEST_CASE(row_two_has_less_columns)
{
    const auto filePath = writeFile("wrong.csv", "1;2\n3");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ';'),
                          InputError,
                          checkMessage(filePath.string()
                                       + ": row (1) has less columns (1) than the expected (2)."));
}

BOOST_AUTO_TEST_CASE(row_two_has_more_columns)
{
    const auto filePath = writeFile("wrong.csv", "1;2\n3;4;5");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ';'),
                          InputError,
                          checkMessage(filePath.string()
                                       + ": row (1) has more columns (3) than the expected (2)."));
}

BOOST_AUTO_TEST_CASE(not_a_number)
{
    const auto filePath = writeFile("wrong.csv", "1;2\nXy;3");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ';'),
                          InputError,
                          checkMessage(filePath.string() + ": \"X\" is not a number"));
}

BOOST_AUTO_TEST_CASE(one_line_one_column)
{
    writeFile("one.tsv", "138.583");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").name(), "one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").getData(1, 0), 138.583);
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("one").getData(2, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage(
                            "TS set 'one' : TS number 2 exceeds TS set's number of columns (1)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("one").getData(1, 1),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'one' : hour 1 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(one_line_two_columns)
{
    writeFile("one_by_two.csv", "123,456.789\n");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path, ',');
    BOOST_CHECK_EQUAL(repo.getDataSeries("one_by_two").name(), "one_by_two");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one_by_two").getData(1, 0), 123);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one_by_two").getData(2, 0), 456.789);
    BOOST_CHECK_EXCEPTION(
      (void)repo.getDataSeries("one_by_two").getData(3, 0),
      TimeSeriesSet::RankTooBig,
      checkMessage("TS set 'one_by_two' : TS number 3 exceeds TS set's number of columns (2)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("one_by_two").getData(2, 1),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'one_by_two' : hour 1 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(two_lines_one_column)
{
    writeFile("two_by_one.tsv", "123\n20");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_one").name(), "two_by_one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_one").getData(1, 0), 123);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_one").getData(1, 1), 20);
    BOOST_CHECK_EXCEPTION(
      (void)repo.getDataSeries("two_by_one").getData(2, 0),
      TimeSeriesSet::RankTooBig,
      checkMessage("TS set 'two_by_one' : TS number 2 exceeds TS set's number of columns (1)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("two_by_one").getData(1, 2),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'two_by_one' : hour 2 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(two_lines_two_columns)
{
    writeFile("two_by_two.csv", "1\t2\n3\t4");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").name(), "two_by_two");
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(1, 0), 1);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(1, 1), 3);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(2, 0), 2);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(2, 1), 4);
    BOOST_CHECK_EXCEPTION(
      (void)repo.getDataSeries("two_by_two").getData(3, 1),
      TimeSeriesSet::RankTooBig,
      checkMessage("TS set 'two_by_two' : TS number 3 exceeds TS set's number of columns (2)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("two_by_two").getData(2, 2),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'two_by_two' : hour 2 exceeds TS set's height"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(_DataSeriesImport_AllCsvFiles_, CsvCreationFixture)

BOOST_AUTO_TEST_CASE(empty_dir)
{
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("any"),
                          DataSeriesRepository::Empty,
                          checkMessage(
                            "Data series repo is empty, and somebody requests data from it"));
}

BOOST_AUTO_TEST_CASE(two_files_with_same_name)
{
    writeFile("data.tsv", "123");
    writeFile("data.csv", "527");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path),
                          DataSeriesRepository::DataSeriesAlreadyExists,
                          checkMessage("Data series repo : data series 'data' already exists"));
}

BOOST_AUTO_TEST_CASE(one_simple_file)
{
    writeFile("one.tsv", "123");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").name(), "one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").getData(1, 0), 123);
}

BOOST_AUTO_TEST_CASE(two_simple_files)
{
    writeFile("one.csv", "123");
    writeFile("two.tsv", "456");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").name(), "one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("two").name(), "two");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").getData(1, 0), 123);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two").getData(1, 0), 456);
}

BOOST_AUTO_TEST_CASE(three_small_files)
{
    writeFile("gen1_p_max.csv", "1\t2\t3\t4\n5\t6\t7\t8\n9\t10\t11\t12\n13\t14\t15\t16.17");
    writeFile("gen2_p_max.tsv", "10\n20\n30\n40");
    writeFile("node1_load.tsv", "20\t30\n21\t31\n22\t32\n23\t33");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);

    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").name(), "gen1_p_max");
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 0), 1);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 0), 2);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 0), 3);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(4, 0), 4);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 1), 5);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 1), 6);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 1), 7);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(4, 1), 8);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 2), 9);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 2), 10);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 2), 11);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(4, 2), 12);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 3), 13);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 3), 14);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 3), 15);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(4, 3), 16.17);
    BOOST_CHECK_EXCEPTION(
      (void)repo.getDataSeries("gen1_p_max").getData(5, 0),
      TimeSeriesSet::RankTooBig,
      checkMessage("TS set 'gen1_p_max' : TS number 5 exceeds TS set's number of columns (4)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("gen1_p_max").getData(1, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'gen1_p_max' : hour 4 exceeds TS set's height"));

    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").name(), "gen2_p_max");
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(1, 0), 10);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(1, 1), 20);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(1, 2), 30);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(1, 3), 40);
    BOOST_CHECK_EXCEPTION(
      (void)repo.getDataSeries("gen2_p_max").getData(2, 0),
      TimeSeriesSet::RankTooBig,
      checkMessage("TS set 'gen2_p_max' : TS number 2 exceeds TS set's number of columns (1)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("gen2_p_max").getData(1, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'gen2_p_max' : hour 4 exceeds TS set's height"));

    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").name(), "node1_load");
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 0), 20);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 1), 21);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 2), 22);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 3), 23);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(2, 0), 30);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(2, 1), 31);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(2, 2), 32);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(2, 3), 33);
    BOOST_CHECK_EXCEPTION(
      (void)repo.getDataSeries("node1_load").getData(3, 0),
      TimeSeriesSet::RankTooBig,
      checkMessage("TS set 'node1_load' : TS number 3 exceeds TS set's number of columns (2)"));
    BOOST_CHECK_EXCEPTION((void)repo.getDataSeries("node1_load").getData(1, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'node1_load' : hour 4 exceeds TS set's height"));
}

BOOST_AUTO_TEST_SUITE_END()
