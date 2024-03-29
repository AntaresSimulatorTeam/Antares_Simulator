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


// Create directory hierarchy (incl. root)
// Don't complain if directories already exist
// Example. Assuming /root exists, `createDirectoryHierarchy("/root", "a/b/c");`
// Creates /root/a, /root/a/b, /root/a/b/c
static bool createDirectory(const Yuni::String& path)
{
    using namespace Yuni;
    if (!IO::Directory::Exists(path))
    {
        const bool ret = IO::Directory::Create(path);

        if (!ret)
        {
            Antares::logs.error() << "Error creating directory " << path;
            return false;
        }
    }
    return true;
}

static bool createDirectoryHierarchy(const Yuni::String& root, const Yuni::String& toCreate)
{
    using namespace Yuni;
    String::Vector dirs;
    toCreate.split(dirs, IO::SeparatorAsString);
    String currentDir = root;

    if (!createDirectory(root))
        return false;

    // Remove file component
    dirs.pop_back();

    for (auto& dir : dirs)
    {
        currentDir << Yuni::IO::Separator << dir;
        if (!createDirectory(currentDir))
            return false;
    }
    return true;
}

namespace Antares
{
namespace Solver
{
ImmediateFileResultWriter::ImmediateFileResultWriter(const char* folderOutput) :
 pOutputFolder(folderOutput)
{
}

ImmediateFileResultWriter::~ImmediateFileResultWriter() = default;

static bool prepareDirectoryHierarchy(const YString& root,
                                      const std::string& entryPath,
                                      Yuni::String& output)
{
    output << root << Yuni::IO::Separator << entryPath.c_str();
    return createDirectoryHierarchy(root, entryPath.c_str());
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addEntryFromBuffer(const std::string& entryPath,
                                                   Yuni::Clob& entryContent)
{
    Yuni::String output;
    if (prepareDirectoryHierarchy(pOutputFolder, entryPath, output))
        IOFileSetContent(output, entryContent);
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addEntryFromBuffer(const std::string& entryPath,
                                                   std::string& entryContent)
{
    Yuni::String output;
    if (prepareDirectoryHierarchy(pOutputFolder, entryPath, output))
        IOFileSetContent(output, entryContent);
}

void ImmediateFileResultWriter::addEntryFromFile(const std::string& entryPath,
                                                 const std::string& filePath)
{
    Yuni::String fullPath;
    if (!prepareDirectoryHierarchy(pOutputFolder, entryPath, fullPath))
        return;

    switch (Yuni::IO::File::Copy(filePath.c_str(), fullPath))
    {
        using namespace Yuni::IO;
    case errNone:
        break;
    case errNotFound:
        logs.error() << filePath << ": file does not exist";
        break;
    case errReadFailed:
        logs.error() << "Read failed '" << filePath << "'";
        break;
    case errWriteFailed:
        logs.error() << "Write failed '" << fullPath << "'";
        break;
    default:
        logs.error() << "Unhandled error";
        break;
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
void NullResultWriter::addEntryFromFile(const std::string&, const std::string&)
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
