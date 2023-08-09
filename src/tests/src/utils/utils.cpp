#include "utils.h"

namespace fs = std::filesystem;

fs::path generateAndCreateDirName(std::string dirName)
{
    fs::path working_dir = fs::temp_directory_path() / dirName;
    fs::remove(working_dir);
    fs::create_directories(working_dir);
    return working_dir;
}
