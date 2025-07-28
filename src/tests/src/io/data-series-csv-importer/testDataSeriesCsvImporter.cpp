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
#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

using namespace std;
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

    void writeFile(string filename, string content);
};

void CsvCreationFixture::writeFile(const string filename, const string content)
{
    auto filepath = temp_path / (filename);
    std::ofstream outfile(filepath);
    outfile << content;
    outfile.close();
}

BOOST_FIXTURE_TEST_SUITE(_DataSeriesImport_OneCsvFile_, CsvCreationFixture)

BOOST_AUTO_TEST_CASE(inconsistent_columns)
{
    writeFile("wrong.csv", "1;2\n3");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ";"),
                          std::invalid_argument,
                          checkMessage("wrong.csv: rows have inconsistent number of columns"));
}

BOOST_AUTO_TEST_CASE(inconsistent_rows)
{
    writeFile("wrong2.csv", "1;2\n;3");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ";"),
                          std::invalid_argument,
                          checkMessage("wrong2.csv: columns have inconsistent number of rows"));
}

BOOST_AUTO_TEST_CASE(not_a_number)
{
    writeFile("wrong.csv", "1;2\nXy;3");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ";"),
                          std::invalid_argument,
                          checkMessage("wrong.csv: \"Xy\" is not a number"));
}

BOOST_AUTO_TEST_CASE(empty_line)
{
    writeFile("wrong.csv", "1;2\n\n3;4");
    BOOST_CHECK_EXCEPTION(DataSeriesRepoImporter::importFromDirectory(temp_path, ";"),
                          std::invalid_argument,
                          checkMessage("wrong.csv: empty line in the middle of the file"));
}

BOOST_AUTO_TEST_CASE(empty_file)
{
    writeFile("empty.csv", "");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("empty").name(), "empty");
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("empty").getData(0, 0),
                          TimeSeriesSet::Empty,
                          checkMessage(
                            "TS set 'empty' : empty, requesting a value makes no sense"));
}

BOOST_AUTO_TEST_CASE(one_line_one_column)
{
    writeFile("one.tsv", "138.583");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").name(), "one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").getData(0, 0), 138.583);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("one").getData(1, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'one' : rank 1 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("one").getData(0, 1),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'one' : hour 1 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(one_line_two_columns)
{
    writeFile("one_by_two.csv", "123,456.789\n");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path, ",");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one_by_two").name(), "one_by_two");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one_by_two").getData(0, 0), 123);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one_by_two").getData(1, 0), 456.789);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("one_by_two").getData(2, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'one_by_two' : rank 2 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("one_by_two").getData(0, 1),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'one_by_two' : hour 1 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(two_lines_one_column)
{
    writeFile("two_by_one.tsv", "123\n20");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_one").name(), "two_by_one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_one").getData(0, 0), 123);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_one").getData(0, 1), 20);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("two_by_one").getData(1, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'two_by_one' : rank 1 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("two_by_one").getData(0, 2),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'two_by_one' : hour 2 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(two_lines_two_columns)
{
    writeFile("two_by_two.csv", "1\t2\n3\t4");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").name(), "two_by_two");
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(0, 0), 1);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(0, 1), 3);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(1, 0), 2);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two_by_two").getData(1, 1), 4);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("two_by_two").getData(2, 1),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'two_by_two' : rank 2 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("two_by_two").getData(1, 2),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'two_by_two' : hour 2 exceeds TS set's height"));
}

BOOST_AUTO_TEST_CASE(two_lines_three_columns_three_separators)
{
    writeFile("2x3.csv", "1\t2;3\n4,5\t6");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path, "\t,;");
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").name(), "2x3");
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").getData(0, 0), 1);
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").getData(0, 1), 4);
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").getData(1, 0), 2);
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").getData(1, 1), 5);
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").getData(2, 0), 3);
    BOOST_CHECK_EQUAL(repo.getDataSeries("2x3").getData(2, 1), 6);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("2x3").getData(3, 1),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set '2x3' : rank 3 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("2x3").getData(2, 2),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set '2x3' : hour 2 exceeds TS set's height"));
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
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").getData(0, 0), 123);
}

BOOST_AUTO_TEST_CASE(two_simple_files)
{
    writeFile("one.csv", "123");
    writeFile("two.tsv", "456");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").name(), "one");
    BOOST_CHECK_EQUAL(repo.getDataSeries("two").name(), "two");
    BOOST_CHECK_EQUAL(repo.getDataSeries("one").getData(0, 0), 123);
    BOOST_CHECK_EQUAL(repo.getDataSeries("two").getData(0, 0), 456);
}

BOOST_AUTO_TEST_CASE(three_small_files)
{
    writeFile("gen1_p_max.csv", "1\t2\t3\t4\n5\t6\t7\t8\n9\t10\t11\t12\n13\t14\t15\t16.17");
    writeFile("gen2_p_max.tsv", "10\n20\n30\n40");
    writeFile("node1_load.tsv", "20\t30\n21\t31\n22\t32\n23\t33");
    auto repo = DataSeriesRepoImporter::importFromDirectory(temp_path);

    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").name(), "gen1_p_max");
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(0, 0), 1);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 0), 2);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 0), 3);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 0), 4);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(0, 1), 5);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 1), 6);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 1), 7);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 1), 8);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(0, 2), 9);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 2), 10);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 2), 11);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 2), 12);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(0, 3), 13);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(1, 3), 14);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(2, 3), 15);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen1_p_max").getData(3, 3), 16.17);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("gen1_p_max").getData(4, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'gen1_p_max' : rank 4 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("gen1_p_max").getData(0, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'gen1_p_max' : hour 4 exceeds TS set's height"));

    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").name(), "gen2_p_max");
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(0, 0), 10);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(0, 1), 20);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(0, 2), 30);
    BOOST_CHECK_EQUAL(repo.getDataSeries("gen2_p_max").getData(0, 3), 40);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("gen2_p_max").getData(1, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'gen2_p_max' : rank 1 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("gen2_p_max").getData(0, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'gen2_p_max' : hour 4 exceeds TS set's height"));

    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").name(), "node1_load");
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(0, 0), 20);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(0, 1), 21);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(0, 2), 22);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(0, 3), 23);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 0), 30);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 1), 31);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 2), 32);
    BOOST_CHECK_EQUAL(repo.getDataSeries("node1_load").getData(1, 3), 33);
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("node1_load").getData(2, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage("TS set 'node1_load' : rank 2 exceeds TS set's width"));
    BOOST_CHECK_EXCEPTION(repo.getDataSeries("node1_load").getData(0, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage("TS set 'node1_load' : hour 4 exceeds TS set's height"));
}

BOOST_AUTO_TEST_SUITE_END()
