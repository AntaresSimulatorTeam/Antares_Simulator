#include "files-system.h"

namespace fs = std::filesystem;

fs::path generateAndCreateDirName(const std::string& dirName)
{
    fs::path working_dir = fs::temp_directory_path() / dirName;
    fs::remove_all(working_dir);
    fs::create_directories(working_dir);
    return working_dir;
}

void createFolder(const stringT& path, const stringT& folder_name)
{
    fs::path folder_path = fs::path(path.c_str()) / folder_name.c_str();

    try
    {
        fs::create_directory(folder_path);
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Exception creating folder '" + folder_name + "': " + e.what() + "\n";
    }
}

void createFile(const stringT& folder_path, const stringT& file_name)
{
    // Construct the full path to the file
    fs::path path = fs::path(folder_path.c_str()) / file_name.c_str();

    // Create an output file stream
    std::ofstream outputFile(path);

    try
    {
        if (outputFile.is_open())
        {
            // File was successfully created and is open
            outputFile << "This is a sample content." << std::endl;
            outputFile.close();
            std::cout << "File " + file_name + " is created in " + folder_path + "\n";
        }
        else
        {
            // Failed to create or open the file
            std::error_code ec = std::make_error_code(std::errc::io_error);
            throw fs::filesystem_error("Failed to create the file.", ec);
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Error creating file: " << file_name << "/" << e.what() << "\n";
    }
}

void removeFolder(stringT& path, stringT& folder_name)
{
    fs::path folder_path = fs::path(path.c_str()) / folder_name.c_str();
    if (fs::exists(folder_path))
    {
        try
        {
            fs::remove_all(folder_path);
            std::cout << "Folder " + folder_name + " at " + folder_path.string()
                           + " deleted.\n";
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Exception deleting folder '" + folder_name + "': " + e.what() + "\n";
        }
    }
}