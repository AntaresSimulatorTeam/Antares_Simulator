#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <matrix.h>

using namespace Antares::Data;
using stringT = std::string;
using namespace std;
namespace fs = std::filesystem;

void createFolder(const stringT& path, const stringT& folder_name);
void createFile(const stringT& folder_path, const stringT& file_name);
void InstantiateMatrix(Matrix<double, int32_t>& matrix, double seed, uint type);
void InstantiateColumn(Matrix<double>::ColumnType& col, double seed, uint type);
void removeFolder(stringT& path, stringT& folder_name);