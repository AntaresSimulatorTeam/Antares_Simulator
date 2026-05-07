// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "immediate_file_writer.h"

#include <filesystem>

#include <antares/io/file.h>
#include <antares/logs/logs.h>

namespace fs = std::filesystem;

namespace Antares::Solver
{
ImmediateFileResultWriter::ImmediateFileResultWriter(const fs::path& folderOutput):
    pOutputFolder(folderOutput)
{
}

ImmediateFileResultWriter::~ImmediateFileResultWriter() = default;

static bool prepareDirectoryHierarchy(const fs::path& root,
                                      const fs::path& entryPath,
                                      fs::path& output)
{
    // Use relative path to remove root dir part
    fs::path fullPath = root / entryPath.relative_path();
    output = fullPath;

    fs::path directory = fullPath.parent_path();
    if (fs::exists(directory))
    {
        return true;
    }

    return fs::create_directories(directory);
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addEntryFromBuffer(const fs::path& entryPath,
                                                   std::string& entryContent)
{
    fs::path output;
    if (prepareDirectoryHierarchy(pOutputFolder, entryPath, output))
    {
        IO::fileSetContent(output.string(), entryContent);
    }
}

void ImmediateFileResultWriter::addEntryFromFile(const fs::path& entryPath,
                                                 const fs::path& filePath)
{
    fs::path fullPath;
    if (!prepareDirectoryHierarchy(pOutputFolder, entryPath, fullPath))
    {
        return;
    }

    std::error_code ec;
    try
    {
        fs::copy(filePath, fullPath, ec);
    }
    catch (const fs::filesystem_error& exc)
    {
        logs.error() << "Error writing " << filePath << " (exception message " << exc.what() << ")";
    }

    if (ec)
    {
        logs.error() << "Error writing " << filePath << " (message " << ec.message() << ")";
    }
}

void ImmediateFileResultWriter::flush()
{
}

bool ImmediateFileResultWriter::needsTheJobQueue() const
{
    return false;
}

void ImmediateFileResultWriter::finalize(bool /*verbose*/)
{
    // Do nothing
}
} // namespace Antares::Solver
