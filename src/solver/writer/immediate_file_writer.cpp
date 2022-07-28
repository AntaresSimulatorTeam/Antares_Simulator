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

// Write to file immediately, creating directories if needed
void ImmediateFileResultWriter::addJob(const std::string& entryPath,
                                       Yuni::Clob& entryContent)
{
    Yuni::String output;
    output << pOutputFolder << Yuni::IO::Separator << entryPath.c_str();
    createDirectoryHierarchy(pOutputFolder, entryPath.c_str());
    IOFileSetContent(output, entryContent);
}

bool ImmediateFileResultWriter::needsTheJobQueue() const
{
    return false;
}
} // namespace Solver
} // namespace Antares
