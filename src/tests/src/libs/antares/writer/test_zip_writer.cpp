//
// Created by leclercsyl on 06/10/23.
//
#define BOOST_TEST_MODULE test-writer tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include "yuni/job/queue/service.h"
#include "antares/writer/i_writer.h"
#include "antares/writer/writer_factory.h"
#include "antares/benchmarking/DurationCollector.h"
#include "utils.h"

extern "C"
{
#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>
}

using namespace Yuni::Job;
using Benchmarking::NullDurationCollector;
using Benchmarking::IDurationCollector;
using Antares::Solver::IResultWriter;

// Handles lifetime of necessary objects
struct TestContext
{
    std::shared_ptr<QueueService> threadPool;
    std::unique_ptr<IDurationCollector> durationCollector;
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
    int length = name.size();
    if (name.size() > ext.size() && name.substr(length - ext.size()) == ext) {
        return name.substr(0, length - ext.size());
    }
    return name;
}


TestContext createContext(const std::filesystem::path zipPath, int threadCount)
{
    auto threadPool = createThreadPool(threadCount);
    std::unique_ptr<IDurationCollector> durationCollector = std::make_unique<Benchmarking::NullDurationCollector>();
    std::string archiveName = zipPath.string();
    auto writer = Antares::Solver::resultWriterFactory(
            Antares::Data::zipArchive,
            removeExtension(zipPath.string(), ".zip"),
            threadPool,
            *durationCollector
        );
    return {
        threadPool,
        std::move(durationCollector),
        writer
    };
}

using ZipReaderHandle = void*;

void checkZipContent(ZipReaderHandle handle, const std::string& path, const std::string& expectedContent)
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
    auto context = createContext(zipPath, 2);
    std::string content1 = "test-content1";
    std::string content2 = "test-content2";
    context.writer->addEntryFromBuffer("test-path", content1);
    context.writer->addEntryFromBuffer("test-second-path", content2);
    context.writer->flush();
    context.writer->finalize(true);

    // Check content is correct
    ZipReaderHandle readerHandle;
    mz_zip_reader_create(&readerHandle);
    BOOST_CHECK(mz_zip_reader_open_file(readerHandle, zipPath.c_str()) == MZ_OK);
    BOOST_CHECK(mz_zip_reader_open_file(readerHandle, zipPath.c_str()) == MZ_OK);
    checkZipContent(readerHandle, "test-path", "test-content1");
    checkZipContent(readerHandle, "test-second-path", "test-content2");
    mz_zip_reader_close(readerHandle);
}
