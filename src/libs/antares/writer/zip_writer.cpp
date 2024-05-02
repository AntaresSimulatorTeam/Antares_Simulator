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
#include <memory>
#include <yuni/io/file.h> // Yuni::IO::File::LoadFromFile

#include "private/zip_writer.h"
#include "antares/logs/logs.h"
#include <antares/benchmarking/timer.h>
#include <antares/benchmarking/DurationCollector.h>

extern "C"
{
#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>
}

#include <ctime> // std::time
#include <utility>
#include <sstream>

namespace fs = std::filesystem;

namespace Antares::Solver
{

namespace
{
void logErrorAndThrow(const std::string& errorMessage)
{
    logs.error() << errorMessage;
    throw IOError(errorMessage);
}
}

// Class ZipWriteJob
template<class ContentT>
ZipWriteJob<ContentT>::ZipWriteJob(ZipWriter& writer,
                                   std::string entryPath,
                                   ContentT& content,
                                   Benchmarking::IDurationCollector& duration_collector) :
 pZipHandle(writer.pZipHandle),
 pZipMutex(writer.pZipMutex),
 pState(writer.pState),
 pEntryPath(std::move(entryPath)),
 pContent(std::move(content)),
 pDurationCollector(duration_collector)
{
}

static std::unique_ptr<mz_zip_file> createInfo(const std::string& entryPath)
{
    auto info = std::make_unique<mz_zip_file>();
    memset(info.get(), 0, sizeof(mz_zip_file));
    info->filename = entryPath.c_str();
    info->zip64 = MZ_ZIP64_FORCE;
    info->compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    info->modified_date = info->creation_date = std::time(0);
    return info;
}

template<class ContentT>
void ZipWriteJob<ContentT>::writeEntry()
{
    // Don't write data if finalize() has been called
    if (pState != ZipState::can_receive_data)
        return;

    auto file_info = createInfo(pEntryPath);

    Benchmarking::Timer timer_wait;
    std::lock_guard guard(pZipMutex); // Wait
    timer_wait.stop();
    pDurationCollector.addDuration("zip_wait", timer_wait.get_duration());

    Benchmarking::Timer timer_write;

    if (int32_t ret = mz_zip_writer_entry_open(pZipHandle, file_info.get()); ret != MZ_OK)
    {
        logErrorAndThrow("Error opening entry " + pEntryPath + " (" + std::to_string(ret) + ")");
    }
    int32_t bw = mz_zip_writer_entry_write(pZipHandle, pContent.data(), pContent.size());
    if (static_cast<unsigned int>(bw) != pContent.size())
    {
        logErrorAndThrow("Error writing entry " + pEntryPath + "(written = " + std::to_string(bw)
                                                + ", size = " + std::to_string(pContent.size()) + ")");
    }

    timer_write.stop();
    pDurationCollector.addDuration("zip_write", timer_write.get_duration());
}

// Class ZipWriter
ZipWriter::ZipWriter(std::shared_ptr<Yuni::Job::QueueService> qs,
                     const char* archivePath,
                     Benchmarking::IDurationCollector& duration_collector) :
 pQueueService(qs),
 pState(ZipState::can_receive_data),
 pArchivePath(std::string(archivePath) + ".zip"),
 pDurationCollector(duration_collector)
{
    pZipHandle = mz_zip_writer_create();
    if (int32_t ret = mz_zip_writer_open_file(pZipHandle, pArchivePath.c_str(), 0, 0); ret != MZ_OK)
    {
        logErrorAndThrow("Error opening zip file " + pArchivePath + " (" + std::to_string(ret) + ")");
    }
    // TODO : make level of compression configurable
    mz_zip_writer_set_compress_level(pZipHandle, MZ_COMPRESS_LEVEL_FAST);
}

ZipWriter::~ZipWriter()
{
    if (!pZipHandle)
        return;

    try
    {
        this->finalize(false);
    }
    catch (...)
    {
        // Catch all, do nothing
    }
}

void ZipWriter::addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent)
{
    addEntryFromBufferHelper<Yuni::Clob>(entryPath, entryContent);
}

void ZipWriter::addEntryFromBuffer(const std::string& entryPath, std::string& entryContent)
{
    addEntryFromBufferHelper<std::string>(entryPath, entryContent);
}

void ZipWriter::addEntryFromFile(const fs::path& entryPath, const fs::path& filePath)
{
    // Read file into buffer immediately, write into archive async
    Yuni::Clob buffer;
    switch (Yuni::IO::File::LoadFromFile(buffer, filePath.c_str()))
    {
        using namespace Yuni::IO;
    case errNone:
        addEntryFromBufferHelper<Yuni::Clob>(entryPath.string(), buffer);
        break;
    case errNotFound:
        logErrorAndThrow(filePath.string() + ": file does not exist");
        break;
   case errReadFailed:
        logErrorAndThrow("Read failed '" + filePath.string() + "'");
        break;
    case errMemoryLimit:
        logErrorAndThrow("Size limit hit for file '" + filePath.string() + "'");
        break;
    default:
        logErrorAndThrow("Unhandled error");
        break;
    }
}

bool ZipWriter::needsTheJobQueue() const
{
    return true;
}

void ZipWriter::finalize(bool verbose)
{
    //wait for completion of pending writing tasks
    flush();

    // Prevent new jobs from being submitted
    pState = ZipState::blocking;

    if (!pZipHandle)
        return;

    if (verbose)
        logs.notice() << "Writing results...";

    std::lock_guard guard(pZipMutex);
    if (int ret = mz_zip_writer_close(pZipHandle); ret != MZ_OK && verbose)
        logs.warning() << "Error closing the zip file " << pArchivePath << " (" << ret << ")";

    mz_zip_writer_delete(&pZipHandle);
    pZipHandle = nullptr;

    if (verbose)
        logs.notice() << "Done";
}

void ZipWriter::flush()
{
    pendingTasks_.join();
}

} // namespace Antares::Solver
