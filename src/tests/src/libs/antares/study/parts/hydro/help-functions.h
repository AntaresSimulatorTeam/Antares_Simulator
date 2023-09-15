#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <matrix.h>

#define SEP Yuni::IO::Separator

using namespace Antares::Data;
using my_string = Yuni::CString<256, false>;
using namespace std;
namespace fs = std::filesystem;

void createFolder(const my_string& path, const my_string& folder_name);
bool createFile(const my_string& folder_path, const my_string& file_name);
void InstantiateMatrix(Matrix<double, Yuni::sint32>& matrix, double seed, uint type);
void InstantiateColumn(Matrix<double>::ColumnType& col, double seed, uint type);
void removeFolder(my_string& path, my_string& folder_name);