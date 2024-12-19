#include <filesystem>
#include <fstream>

#include <antares/io/file.h>

#include "encoder.hxx"

namespace Antares::Solver
{
ModelerParameters parseModelerParameters(const std::filesystem::path& path)
{
    const auto contents = Antares::IO::readFile(path);
    YAML::Node root = YAML::Load(contents);
    return root.as<ModelerParameters>();
}
} // namespace Antares::Solver
