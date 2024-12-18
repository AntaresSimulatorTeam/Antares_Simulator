#include <filesystem>
#include <fstream>

#include "encoder.hxx"

namespace Antares::Solver
{
static std::string loadFileToString(const std::filesystem::path& filePath)
{
    return std::string(std::istreambuf_iterator<char>(std::ifstream(filePath).rdbuf()),
                       std::istreambuf_iterator<char>());
}

ModelerParameters parseModelerParameters(const std::filesystem::path& path)
{
    auto contents = loadFileToString(path);
    YAML::Node root = YAML::Load(contents);
    return root.as<ModelerParameters>();
}
} // namespace Antares::Solver
