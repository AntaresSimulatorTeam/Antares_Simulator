#pragma once

#include <string>

enum class mpsExportStatus : unsigned char
{
	NO_EXPORT,
	EXPORT_FIRST_OPIM,
	EXPORT_SECOND_OPIM,
	EXPORT_BOTH_OPTIMS
};

std::string mpsExportStatusToString(const mpsExportStatus& mps_export_status);
const char* mpsExportIcon(const mpsExportStatus& mps_export_status);

