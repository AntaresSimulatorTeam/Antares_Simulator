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
#pragma once

#include <mutex>
#include <string>

#include <yuni/core/string.h>
#include <yuni/job/queue/service.h>

#include <antares/benchmarking/DurationCollector.h>
#include "antares/concurrency/concurrency.h"
#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
enum class ZipState
{
    can_receive_data,
    blocking
};

class ZipWriter;

/*!
 * In charge of writing one entry into the underlying zip.
 * May be used as a function object.
 */
template<class ContentT>
class ZipWriteJob
{
public:
    ZipWriteJob(ZipWriter& writer,
                std::string entryPath,
                ContentT& content,
                Benchmarking::DurationCollector& duration_collector);
    void writeEntry();

    void operator()()
    {
        writeEntry();
    }

private:
    // Pointer to Zip handle
    void* pZipHandle;
    // Protect pZipHandle against concurrent writes, since minizip-ng isn't thread-safe
    std::mutex& pZipMutex;
    // State
    ZipState& pState;
    // Entry path for the new file within the zip archive
    const std::string pEntryPath;
    // Content of the new file
    ContentT pContent;
    // Benchmarking. How long do we wait ? How long does the zip write take ?
    Benchmarking::DurationCollector& pDurationCollector;
};

class ZipWriter: public IResultWriter
{
public:
    ZipWriter(std::shared_ptr<Yuni::Job::QueueService> qs,
              std::filesystem::path archivePath,
              Benchmarking::DurationCollector& duration_collector);
    virtual ~ZipWriter();
    void addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent) override;
    void addEntryFromBuffer(const std::string& entryPath, std::string& entryContent) override;
    void addEntryFromFile(const std::filesystem::path& entryPath,
                          const std::filesystem::path& filePath) override;
    void flush() override;
    bool needsTheJobQueue() const override;
    void finalize(bool verbose) override;
    friend class ZipWriteJob<Yuni::Clob>;
    friend class ZipWriteJob<std::string>;

private:
    // Queue where jobs will be appended
    std::shared_ptr<Yuni::Job::QueueService> pQueueService;
    // Prevent concurrent writes to the zip file
    std::mutex pZipMutex;
    // minizip-ng requires a void* as a zip handle.
    void* pZipHandle;
    // State, to allow/prevent new jobs being added to the queue
    ZipState pState;
    // Absolute path to the archive
    const std::filesystem::path pArchivePath;
    // Benchmarking. Passed to jobs
    Benchmarking::DurationCollector& pDurationCollector;

    Concurrency::FutureSet pendingTasks_;

private:
    template<class ContentType>
    void addEntryFromBufferHelper(const std::string& entryPath, ContentType& entryContent);
};
} // namespace Antares::Solver

#include "zip_writer.hxx"
