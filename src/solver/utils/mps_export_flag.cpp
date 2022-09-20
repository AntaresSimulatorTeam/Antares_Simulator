#include "mps_export_flag.h"
#include "antares/exception/AssertionError.hpp"

std::string mpsExportStatusToString(const mpsExportStatus& mps_export_status)
{
    switch (mps_export_status)
    {
    case mpsExportStatus::NO_EXPORT:
        return "none";
    case mpsExportStatus::EXPORT_FIRST_OPIM:
        return "optim 1";
    case mpsExportStatus::EXPORT_SECOND_OPIM:
        return "optim 2";
    case mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return "both optims";
    default:
        throw AssertionError(
            "Invalid MPS export status : "
            + std::to_string(static_cast<unsigned long>(mps_export_status)));
        return "unknown status";
    }
}

const char* mpsExportIcon(const mpsExportStatus& mps_export_status)
{
    switch (mps_export_status)
    {
    case mpsExportStatus::NO_EXPORT:
        return "images/16x16/light_orange.png";
    case mpsExportStatus::EXPORT_FIRST_OPIM:
        return "images/16x16/light_green.png";
    case mpsExportStatus::EXPORT_SECOND_OPIM:
        return "images/16x16/light_green.png";
    case mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return "images/16x16/light_green.png";
    default:
        throw AssertionError(
            "Invalid MPS export status icon : "
            + std::to_string(static_cast<unsigned long>(mps_export_status)));
        return "";
    }
}