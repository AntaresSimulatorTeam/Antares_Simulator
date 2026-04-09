// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>

#define BOOST_TEST_MODULE test - writing parquet tables - mode standalone
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>

namespace fs = std::filesystem;

// Helper: throw on Arrow errors
#define ARROW_THROW_NOT_OK(expr)                      \
    do                                                \
    {                                                 \
        arrow::Status _st = (expr);                   \
        if (!_st.ok())                                \
            throw std::runtime_error(_st.ToString()); \
    } while (0)

#define ARROW_THROW_ASSIGN(lhs, expr)                           \
    do                                                          \
    {                                                           \
        auto _res = (expr);                                     \
        if (!_res.ok())                                         \
            throw std::runtime_error(_res.status().ToString()); \
        lhs = std::move(_res).ValueOrDie();                     \
    } while (0)

// =======================================================
// Building and writing a regular table to Parquet
// =======================================================
// "regular" table : as opposed to nullable table (containing std::optional<T>)

std::shared_ptr<arrow::Table> buildTable(const std::vector<int>& ids,
                                         const std::vector<std::string>& names,
                                         const std::vector<double>& scores)
{
    // --- 1. Fill builders ---
    arrow::Int32Builder id_builder;
    arrow::StringBuilder name_builder;
    arrow::DoubleBuilder score_builder;

    ARROW_THROW_NOT_OK(id_builder.AppendValues(ids));
    ARROW_THROW_NOT_OK(name_builder.AppendValues(names));
    ARROW_THROW_NOT_OK(score_builder.AppendValues(scores));

    // --- 2. Finalise arrays ---
    std::shared_ptr<arrow::Array> ids_, names_, scores_;

    ARROW_THROW_ASSIGN(ids_, id_builder.Finish());
    ARROW_THROW_ASSIGN(names_, name_builder.Finish());
    ARROW_THROW_ASSIGN(scores_, score_builder.Finish());

    // --- 3. Define schema ---
    auto schema = arrow::schema({
      arrow::field("id", arrow::int32()),
      arrow::field("name", arrow::utf8()),
      arrow::field("score", arrow::float64()),
    });

    // --- 4. Assemble table ---
    return arrow::Table::Make(schema, {ids_, names_, scores_});
}

void writeParquet(const std::shared_ptr<arrow::Table>& table, const fs::path& file_path)
{
    // --- 1. Open output file ---
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    ARROW_THROW_ASSIGN(outfile, arrow::io::FileOutputStream::Open(file_path.string()));

    // --- 2. Configure Parquet writer ---
    auto writer_props = parquet::WriterProperties::Builder()
                          .compression(arrow::Compression::SNAPPY) // or ZSTD, GZIP, UNCOMPRESSED
                          ->version(parquet::ParquetVersion::PARQUET_2_6)
                          ->build();

    auto arrow_props = parquet::ArrowWriterProperties::Builder()
                         .store_schema() // embeds Arrow schema in Parquet metadata
                         ->build();

    // --- 3. Write ---
    ARROW_THROW_NOT_OK(parquet::arrow::WriteTable(*table,
                                                  arrow::default_memory_pool(),
                                                  outfile,
                                                  /*chunk_size=*/1024, // rows per row group
                                                  writer_props,
                                                  arrow_props));
}

// =======================================================
// Reading a Parquet file into an Arrow table
// =======================================================
std::shared_ptr<arrow::Table> readParquet(const fs::path& file_path)
{
    std::shared_ptr<arrow::io::ReadableFile> infile;
    ARROW_THROW_ASSIGN(infile, arrow::io::ReadableFile::Open(file_path.string()));

    std::unique_ptr<parquet::arrow::FileReader> reader;
    ARROW_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

    std::shared_ptr<arrow::Table> table;
    ARROW_THROW_NOT_OK(reader->ReadTable(&table));
    return table;
}

BOOST_AUTO_TEST_CASE(make_and_write_on_disk_a_regular_table_in_parquet___output_file_content_ok)
{
    // =================
    // Arrange...
    // =================
    std::vector<int> ids = {1, 2, 3};
    std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
    std::vector<double> scores = {85.5, 92.0, 78.0};
    auto table = buildTable(ids, names, scores);

    auto file_path = fs::temp_directory_path() / "test_output.parquet";

    // =================
    // Act...
    // =================
    writeParquet(table, file_path);

    // =================
    // Assert...
    // =================
    BOOST_CHECK(fs::exists(file_path));

    // --- Read back and verify ---
    // chunk_size=1024 and we only have 3 rows: single row group, single chunk per column
    auto read_table = readParquet(file_path);

    BOOST_REQUIRE_EQUAL(read_table->num_rows(), ids.size());
    BOOST_REQUIRE_EQUAL(read_table->num_columns(), 3);

    // Check "id" column (Int32)
    auto column_of_ids = std::static_pointer_cast<arrow::Int32Array>(read_table->column(0)->chunk(0));
    BOOST_CHECK_EQUAL(column_of_ids->Value(0), ids[0]);
    BOOST_CHECK_EQUAL(column_of_ids->Value(1), ids[1]);
    BOOST_CHECK_EQUAL(column_of_ids->Value(2), ids[2]);

    // Check "name" column (Utf8/String)
    auto column_of_names = std::static_pointer_cast<arrow::StringArray>(read_table->column(1)->chunk(0));
    BOOST_CHECK_EQUAL(column_of_names->GetString(0), names[0]);
    BOOST_CHECK_EQUAL(column_of_names->GetString(1), names[1]);
    BOOST_CHECK_EQUAL(column_of_names->GetString(2), names[2]);

    // Check "score" column (Double)
    auto column_of_scores = std::static_pointer_cast<arrow::DoubleArray>(read_table->column(2)->chunk(0));
    BOOST_CHECK_EQUAL(column_of_scores->Value(0), scores[0]);
    BOOST_CHECK_EQUAL(column_of_scores->Value(1), scores[1]);
    BOOST_CHECK_EQUAL(column_of_scores->Value(2), scores[2]);

    fs::remove(file_path);
}

// =======================================================
// Building and writing a nullable table to Parquet
// =======================================================
// "nullable" table : table containing std::optional<T>

// -----------------------------------------------------------------------
// Helper: convert a vector of optional<T> into (values, is_valid) pair
// Arrow needs a flat value buffer (any value for nulls) + a bool mask
// -----------------------------------------------------------------------
template<typename T>
std::pair<std::vector<T>, std::vector<bool>> SplitOptionals(
  const std::vector<std::optional<T>>& input)
{
    std::vector<T> values(input.size());
    std::vector<bool> is_valid(input.size());
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        is_valid[i] = input[i].has_value();
        values[i] = input[i].value_or(T{}); // placeholder for nulls
    }
    return {values, is_valid};
}

std::shared_ptr<arrow::Table> buildNullableTable(std::vector<std::optional<int>> ids,
                                                 std::vector<std::optional<std::string>> names,
                                                 std::vector<std::optional<double>> scores)
{
    // --- 1. Fill builders ---
    arrow::Int32Builder id_builder;
    arrow::StringBuilder name_builder;
    arrow::DoubleBuilder score_builder;

    auto [id_values, id_valid] = SplitOptionals(ids);
    auto [score_values, score_valid] = SplitOptionals(scores);

    // AppendValues(values, is_valid):
    //   is_valid[i] == false  →  null  (empty cell in Parquet)
    //   is_valid[i] == true   →  value written normally
    ARROW_THROW_NOT_OK(id_builder.AppendValues(id_values, id_valid));
    ARROW_THROW_NOT_OK(score_builder.AppendValues(score_values, score_valid));

    // For std::string, AppendValues doesn't support null masks, so we append one by one
    for (const auto& name: names)
    {
        if (name.has_value())
        {
            ARROW_THROW_NOT_OK(name_builder.Append(*name));
        }
        else
        {
            ARROW_THROW_NOT_OK(name_builder.AppendEmptyValue());
        }
    }

    //// --- 2. Finalise arrays ---
    std::shared_ptr<arrow::Array> ids_, names_, scores_;

    ARROW_THROW_ASSIGN(ids_, id_builder.Finish());
    ARROW_THROW_ASSIGN(names_, name_builder.Finish());
    ARROW_THROW_ASSIGN(scores_, score_builder.Finish());

    // --- 3. Define schema ---
    auto schema = arrow::schema({
      arrow::field("id", arrow::int32()),
      arrow::field("name", arrow::utf8()),
      arrow::field("score", arrow::float64()),
    });

    // --- 4. Assemble table ---
    return arrow::Table::Make(schema, {ids_, names_, scores_});
}

BOOST_AUTO_TEST_CASE(make_and_write_on_disk_a_nullable_table_in_parquet___output_file_content_ok)
{
    // =================
    // Arrange...
    // =================
    std::vector<std::optional<int>> ids = {std::nullopt, 2, 3};
    std::vector<std::optional<std::string>> names = {"Alice", std::nullopt, "Charlie"};
    std::vector<std::optional<double>> scores = {85.5, 92.0, std::nullopt};
    auto nullableTable = buildNullableTable(ids, names, scores);

    auto file_path = fs::temp_directory_path() / "test_output_nullable.parquet";

    // =================
    // Act...
    // =================
    writeParquet(nullableTable, file_path);

    // =================
    // Assert...
    // =================
    BOOST_CHECK(fs::exists(file_path));

    // --- Read back and verify ---
    // chunk_size=1024 and we only have 3 rows: single row group, single chunk per column
    auto read_table = readParquet(file_path);

    BOOST_REQUIRE_EQUAL(read_table->num_rows(), ids.size());
    BOOST_REQUIRE_EQUAL(read_table->num_columns(), 3);

    // Check "id" column — ids[0] is nullopt
    auto column_of_ids = std::static_pointer_cast<arrow::Int32Array>(read_table->column(0)->chunk(0));
    BOOST_CHECK(column_of_ids->IsNull(0));
    BOOST_CHECK_EQUAL(column_of_ids->Value(1), *ids[1]);
    BOOST_CHECK_EQUAL(column_of_ids->Value(2), *ids[2]);

    // Check "name" column — names[1] is nullopt, stored as empty string via AppendEmptyValue
    auto column_of_names = std::static_pointer_cast<arrow::StringArray>(read_table->column(1)->chunk(0));
    BOOST_CHECK_EQUAL(column_of_names->GetString(0), *names[0]);
    BOOST_CHECK(column_of_names->GetString(1).empty());
    BOOST_CHECK_EQUAL(column_of_names->GetString(2), *names[2]);

    // Check "score" column — scores[2] is nullopt
    auto column_of_scores = std::static_pointer_cast<arrow::DoubleArray>(read_table->column(2)->chunk(0));
    BOOST_CHECK_EQUAL(column_of_scores->Value(0), *scores[0]);
    BOOST_CHECK_EQUAL(column_of_scores->Value(1), *scores[1]);
    BOOST_CHECK(column_of_scores->IsNull(2));

    fs::remove(file_path);
}
