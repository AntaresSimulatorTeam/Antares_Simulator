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
#include <yuni/core/string.h>
#include <yuni/io/file.h>

#include "private/immediate_file_writer.h"
#include <antares/io/file.h>
#include <antares/logs/logs.h>

namespace fs = std::filesystem;

namespace Antares
{
namespace Solver
{
ImmediateFileResultWriter::ImmediateFileResultWriter(const char* folderOutput) :
 pOutputFolder(folderOutput)
{
}

ImmediateFileResultWriter::~ImmediateFileResultWriter() = default;

static bool prepareDirectoryHierarchy(const fs::path& root,
                                      const fs::path& entryPath,
                                      fs::path& output)
{
    fs::path fullPath = root / entryPath;

    output = fullPath;

    fullPath.remove_filename(); // only create directories

    if (fs::exists(fullPath))
        return true;

    return fs::create_directories(fullPath);
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addEntryFromBuffer(const std::string& entryPath,
                                                   Yuni::Clob& entryContent)
{
    fs::path output;
    if (prepareDirectoryHierarchy(pOutputFolder, entryPath, output))
        IOFileSetContent(output.string(), entryContent);
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addEntryFromBuffer(const std::string& entryPath,
                                                   std::string& entryContent)
{
    fs::path output;
    if (prepareDirectoryHierarchy(pOutputFolder, entryPath, output))
        IOFileSetContent(output.string(), entryContent);
}

void ImmediateFileResultWriter::addEntryFromFile(const fs::path& entryPath,
                                                 const fs::path& filePath)
{
    fs::path fullPath;
    if (!prepareDirectoryHierarchy(pOutputFolder, entryPath, fullPath))
        return;

    std::error_code ec;
    fs::copy(filePath, fullPath, ec);

    if (ec)
    {
        if (ec == std::errc::no_such_file_or_directory)
            logs.error() << filePath.string() << ": file does not exist";
        else
            logs.error() << "Error: " << ec.message();
    }
}

void ImmediateFileResultWriter::flush()
{}

bool ImmediateFileResultWriter::needsTheJobQueue() const
{
    return false;
}

void ImmediateFileResultWriter::finalize(bool /*verbose*/)
{
    // Do nothing
}

void NullResultWriter::addEntryFromBuffer(const std::string&, Yuni::Clob&)
{}
void NullResultWriter::addEntryFromBuffer(const std::string&, std::string&)
{}
void NullResultWriter::addEntryFromFile(const fs::path&, const fs::path&)
{}
void NullResultWriter::flush()
{}

bool NullResultWriter::needsTheJobQueue() const
{
    return false;
}
void NullResultWriter::finalize(bool)
{}
} // namespace Solver
} // namespace Antares
