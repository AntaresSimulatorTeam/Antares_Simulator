#include "help-functions.h"

void createFolder(const my_string& path, const my_string& folder_name)
{
    fs::path folder_path = fs::path(path.c_str()) / folder_name.c_str();
    fs::create_directory(folder_path);
}

bool createFile(const my_string& folder_path, const my_string& file_name)
{
    // Construct the full path to the file
    fs::path path = fs::path(folder_path.c_str()) / file_name.c_str();

    // Create an output file stream
    std::ofstream outputFile(path);

    if (outputFile.is_open())
    {
        // File was successfully created and is open
        outputFile << "This is a sample content." << std::endl;
        outputFile.close();
        return true;
    }
    else
    {
        // Failed to create or open the file
        return false;
    }
}

void InstantiateMatrix(Matrix<double, Yuni::sint32>& matrix, double seed, uint type)
{
    for (uint i = 0; i < matrix.width; i++)
    {
        for (uint hours = 0; hours < type; hours++)
        {
            if (hours != 0 && hours != type - 1)
                matrix[i][hours] = seed;
            else if (hours == 0)
                matrix[i][hours] = seed + 1;
            else if (hours == type - 1)
                matrix[i][hours] = seed + 2;
        }
    }
}

void InstantiateColumn(Matrix<double>::ColumnType& col, double seed, uint type)
{
    for (uint days = 0; days < type; days++)
    {
        if (days != 0 && days != type - 1)
            col[days] = seed;
        else if (days == 0)
            col[days] = seed + 1;
        else if (days == type - 1)
            col[days] = seed + 2;
    }
}