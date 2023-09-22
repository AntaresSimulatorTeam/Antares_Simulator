//
// Created by marechaljas on 10/08/23.
//

#pragma once

namespace Antares::Data
{
// Format of results
enum ResultFormat
{
    // Store outputs as files inside directories
    legacyFilesDirectories = 0,
    // Store outputs inside a single zip archive
    zipArchive
};
} // namespace Antares::Data