// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

#include <antares/array/matrix-io.h>
#include <antares/array/matrix.h>
#include <antares/logs/logs.h>

namespace
{
using Clock = std::chrono::steady_clock;

constexpr unsigned int defaultIterations = 3;
constexpr unsigned int defaultHeight = 8760;

std::atomic<std::uint64_t> benchmarkSink = 0;

void consume(std::uint64_t value)
{
    benchmarkSink.fetch_add(value, std::memory_order_relaxed);
}

template<class Operation>
void runBenchmark(std::string_view name,
                  unsigned int width,
                  unsigned int height,
                  unsigned int iterations,
                  Operation&& operation)
{
    operation();

    const auto start = Clock::now();
    for (unsigned int i = 0; i < iterations; ++i)
    {
        operation();
    }
    const auto elapsed = std::chrono::duration<double, std::milli>(Clock::now() - start).count();

    std::cout << name << ',' << width << ',' << height << ',' << iterations << ',' << std::fixed
              << std::setprecision(3) << elapsed << ',' << elapsed / iterations << '\n';
}

std::string makeCsv(unsigned int width, unsigned int height)
{
    std::string result;
    result.reserve(static_cast<std::size_t>(width) * height * 8);

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            if (x != 0)
            {
                result += '\t';
            }

            const auto value = (x * 17 + y * 13) % 1000;
            result += std::to_string(static_cast<double>(value) / 10.0 + 0.25);
        }
        result += '\n';
    }

    return result;
}

void benchmarkLoads(unsigned int width, unsigned int height, unsigned int iterations)
{
    const std::string csv = makeCsv(width, height);
    const auto filename = std::filesystem::temp_directory_path() / "antares-matrix-benchmark.txt";
    {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        file.write(csv.data(), static_cast<std::streamsize>(csv.size()));
    }
    Antares::Matrix matrix;

    runBenchmark("load-double",
                 width,
                 height,
                 iterations,
                 [&]
                 {
                     const bool loaded = Antares::MatrixIO::load(matrix, filename, width, height);
                     if (!loaded || matrix.width() != width || matrix.height() != height)
                     {
                         std::cerr << "matrix load failed\n";
                         std::exit(EXIT_FAILURE);
                     }
                     consume(static_cast<std::uint64_t>(matrix[width - 1][height - 1]));
                 });
    std::filesystem::remove(filename);
}

void benchmarkAllocation(unsigned int width, unsigned int height, unsigned int iterations)
{
    runBenchmark("resize",
                 width,
                 height,
                 iterations,
                 [=]
                 {
                     Antares::Matrix matrix;
                     matrix.resize(width, height);
                     consume(matrix.width() + matrix.height());
                 });

    runBenchmark("reset",
                 width,
                 height,
                 iterations,
                 [=]
                 {
                     Antares::Matrix matrix;
                     matrix.reset(width, height);
                     consume(matrix.width() + matrix.height());
                 });
}

void benchmarkSerialization(unsigned int width, unsigned int height, unsigned int iterations)
{
    Antares::Matrix matrix(width, height);
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            matrix[x][y] = static_cast<double>((x * 17 + y * 13) % 1000) / 10.0 + 0.25;
        }
    }

    runBenchmark(
      "save-fresh-buffer",
      width,
      height,
      iterations,
      [&]
      {
          std::string output;
          Antares::MatrixIO::saveToBuffer(matrix, output, 6, false, std::identity{}, true);
          consume(output.size());
      });

    std::string reusedOutput;
    runBenchmark(
      "save-reused-buffer",
      width,
      height,
      iterations,
      [&]
      {
          Antares::MatrixIO::saveToBuffer(matrix, reusedOutput, 6, false, std::identity{}, true);
          consume(reusedOutput.size());
      });
}

void benchmarkBulkOperations(unsigned int width, unsigned int height, unsigned int iterations)
{
    Antares::Matrix matrix(width, height);
    Antares::Matrix source(width, height);
    source.fill(2.5);
    std::vector<double> column(height, 1.5);

    runBenchmark("zero",
                 width,
                 height,
                 iterations,
                 [&]
                 {
                     matrix.zero();
                     consume(static_cast<std::uint64_t>(matrix[0][0]));
                 });

    runBenchmark("fill",
                 width,
                 height,
                 iterations,
                 [&]
                 {
                     matrix.fill(3.5);
                     consume(static_cast<std::uint64_t>(matrix[0][0]));
                 });

    runBenchmark("copy",
                 width,
                 height,
                 iterations,
                 [&]
                 {
                     matrix.copyFrom(source);
                     consume(static_cast<std::uint64_t>(matrix[0][0]));
                 });

    runBenchmark("paste-column",
                 width,
                 height,
                 iterations,
                 [&]
                 {
                     matrix.pasteToColumn(0, column);
                     consume(static_cast<std::uint64_t>(matrix[0][0]));
                 });
}

unsigned int parseIterations(int argc, char** argv)
{
    if (argc == 1)
    {
        return defaultIterations;
    }

    if (argc != 3 || std::string_view(argv[1]) != "--iterations")
    {
        std::cerr << "usage: matrix-benchmark [--iterations N]\n";
        std::exit(EXIT_FAILURE);
    }

    const auto iterations = std::strtoul(argv[2], nullptr, 10);
    if (iterations == 0 || iterations > std::numeric_limits<unsigned int>::max())
    {
        std::cerr << "iterations must be between 1 and " << std::numeric_limits<unsigned int>::max()
                  << '\n';
        std::exit(EXIT_FAILURE);
    }
    return static_cast<unsigned int>(iterations);
}
} // namespace

int main(int argc, char** argv)
{
    const unsigned int iterations = parseIterations(argc, argv);
    Antares::logs.verbosityLevel = Antares::Logs::Verbosity::Error::level;

    std::cout << "case,width,height,iterations,total_ms,ms_per_iteration\n";

    for (const unsigned int width: {1U, 10U, 100U})
    {
        benchmarkAllocation(width, defaultHeight, iterations);
        benchmarkLoads(width, defaultHeight, iterations);
        benchmarkSerialization(width, defaultHeight, iterations);
    }

    benchmarkBulkOperations(100, defaultHeight, iterations);

    std::cerr << "benchmark sink: " << benchmarkSink.load(std::memory_order_relaxed) << '\n';
    return 0;
}
