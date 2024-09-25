/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE test - writer tests
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "yuni/job/queue/service.h"

#include "antares/benchmarking/DurationCollector.h"
#include "antares/writer/i_writer.h"
#include "antares/writer/in_memory_writer.h"
#include "antares/writer/writer_factory.h"

#include "files-system.h"

extern "C"
{
#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>
}

using namespace Yuni::Job;
using Antares::Solver::IResultWriter;
using Benchmarking::DurationCollector;

// Handles lifetime of necessary objects
struct TestContext
{
    std::shared_ptr<QueueService> threadPool;
    std::unique_ptr<DurationCollector> durationCollector;
    std::shared_ptr<IResultWriter> writer;
};

std::shared_ptr<QueueService> createThreadPool(int size)
{
    auto threadPool = std::make_shared<QueueService>();
    threadPool->maximumThreadCount(size);
    threadPool->start();
    return threadPool;
}

std::string removeExtension(const std::string& name, const std::string& ext)
{
    size_t length = name.size();
    if (name.size() > ext.size() && name.substr(length - ext.size()) == ext)
    {
        return name.substr(0, length - ext.size());
    }
    return name;
}

TestContext createContext(const std::filesystem::path zipPath,
                          int threadCount,
                          Antares::Data::ResultFormat fmt)
{
    auto threadPool = createThreadPool(threadCount);
    std::unique_ptr<DurationCollector>
      durationCollector = std::make_unique<Benchmarking::DurationCollector>();
    std::string archiveName = zipPath.string();
    auto writer = Antares::Solver::resultWriterFactory(fmt,
                                                       removeExtension(zipPath.string(), ".zip"),
                                                       threadPool,
                                                       *durationCollector);
    return {threadPool, std::move(durationCollector), writer};
}

using ZipReaderHandle = void*;

BOOST_AUTO_TEST_SUITE(writer)

void checkZipContent(ZipReaderHandle handle,
                     const std::string& path,
                     const std::string& expectedContent)
{
    BOOST_CHECK(mz_zip_reader_locate_entry(handle, path.c_str(), 0) == MZ_OK);
    BOOST_CHECK(mz_zip_reader_entry_open(handle) == MZ_OK);
    char buffer[4096];
    int bytesRead = mz_zip_reader_entry_read(handle, buffer, sizeof(buffer));
    std::string stringRead(buffer, bytesRead);
    BOOST_CHECK(stringRead == expectedContent);
    mz_zip_reader_entry_close(handle);
}

BOOST_AUTO_TEST_CASE(test_zip)
{
    // Writer some content to test.zip, possibly from 2 threads
    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto zipPath = working_tmp_dir / "test.zip";
    auto context = createContext(zipPath, 2, Antares::Data::zipArchive);
    std::string content1 = "test-content1";
    std::string content2 = "test-content2";
    context.writer->addEntryFromBuffer("test-path", content1);
    context.writer->addEntryFromBuffer("test-second-path", content2);
    context.writer->flush();
    context.writer->finalize(true);

    // Check content is correct
    ZipReaderHandle readerHandle = mz_zip_reader_create();
    std::string zipPathStr = zipPath.string();
    BOOST_CHECK(mz_zip_reader_open_file(readerHandle, zipPathStr.c_str()) == MZ_OK);
    checkZipContent(readerHandle, "test-path", "test-content1");
    checkZipContent(readerHandle, "test-second-path", "test-content2");
    mz_zip_reader_close(readerHandle);
}

BOOST_AUTO_TEST_CASE(test_in_memory_concrete)
{
    // Writer some content to test.zip, possibly from 2 threads
    std::string content1 = "test-content1";
    std::string content2 = "test-content2";

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);

    writer.addEntryFromBuffer("folder/test", content1);
    writer.addEntryFromBuffer("test-second-path", content2);
    writer.flush();
    writer.finalize(true);

    const auto& map = writer.getMap();
    BOOST_CHECK(map.at("folder/test") == content1);
    BOOST_CHECK(map.at("test-second-path") == content2);
}

BOOST_AUTO_TEST_CASE(test_in_memory_dyncast)
{
    // Writer some content to test.zip, possibly from 2 threads

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto zipPath = working_tmp_dir / "test.zip";
    auto context = createContext(zipPath /* unused */, 1 /* unused */, Antares::Data::inMemory);

    std::string content1 = "test-content1";
    std::string content2 = "test-content2";
    context.writer->addEntryFromBuffer("folder/test", content1);
    context.writer->addEntryFromBuffer("test-second-path", content2);
    context.writer->flush();
    context.writer->finalize(true);

    auto writer = std::dynamic_pointer_cast<Antares::Solver::InMemoryWriter>(context.writer);
    BOOST_CHECK(writer != nullptr);

    const auto& map = writer->getMap();
    BOOST_CHECK(map.at("folder/test") == content1);
    BOOST_CHECK(map.at("test-second-path") == content2);
}

BOOST_AUTO_TEST_CASE(test_in_memory_sanitize_antislash)
{
    // Writer some content to test.zip, possibly from 2 threads

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    auto zipPath = working_tmp_dir / "test.zip";
    auto context = createContext(zipPath /* unused */, 1 /* unused */, Antares::Data::inMemory);

    std::string content1 = "test-content1";
    context.writer->addEntryFromBuffer("folder\\test", content1);
    context.writer->flush();
    context.writer->finalize(true);

    auto writer = std::dynamic_pointer_cast<Antares::Solver::InMemoryWriter>(context.writer);
    BOOST_CHECK(writer != nullptr);

    const auto& map = writer->getMap();
    BOOST_CHECK(map.at("folder/test") == content1);
}

BOOST_AUTO_TEST_SUITE_END()
