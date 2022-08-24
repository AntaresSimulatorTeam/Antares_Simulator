#include <yuni/core/string.h>
#include <yuni/io/file.h>
#include <antares/io/file.h> // IOFileSetContent

#include "immediate_file_writer.h"

// Create directory hierarchy (incl. root)
// Don't complain if directories already exist
// Example. Assuming /root exists, `createDirectoryHierarchy("/root", "a/b/c");`
// Creates /root/a, /root/a/b, /root/a/b/c
static void createDirectoryHierarchy(const Yuni::String& root, const Yuni::String& toCreate)
{
    using namespace Yuni;
    String::Vector dirs;
    toCreate.split(dirs, IO::SeparatorAsString);
    String currentDir = root;

    if (!IO::Directory::Exists(root))
        IO::Directory::Create(root);

    // Remove file component
    dirs.pop_back();

    for (auto& dir : dirs)
    {
        currentDir << Yuni::IO::Separator << dir;

        if (!IO::Directory::Exists(currentDir))
            IO::Directory::Create(currentDir);
    }
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

static Yuni::String prepareDirectoryHierarchy(const YString& root, const std::string& entryPath)
{
    Yuni::String output;
    output << root << Yuni::IO::Separator << entryPath.c_str();
    createDirectoryHierarchy(root, entryPath.c_str());
    return output;
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addJob(const std::string& entryPath, Yuni::Clob& entryContent)
{
    auto output = prepareDirectoryHierarchy(pOutputFolder, entryPath);
    IOFileSetContent(output, entryContent);
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addJob(const std::string& entryPath, std::string& entryContent)
{
    auto output = prepareDirectoryHierarchy(pOutputFolder, entryPath);
    IOFileSetContent(output, entryContent);
}

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addJob(const std::string& entryPath, Antares::IniFile& entryContent)
{
    auto output = prepareDirectoryHierarchy(pOutputFolder, entryPath);
    std::string buffer;
    entryContent.saveToString(buffer);
    IOFileSetContent(output, buffer);
}

bool ImmediateFileResultWriter::needsTheJobQueue() const
{
    return false;
}

void ImmediateFileResultWriter::finalize(bool /*verbose*/)
{
    // Do nothing
}
} // namespace Solver
} // namespace Antares
