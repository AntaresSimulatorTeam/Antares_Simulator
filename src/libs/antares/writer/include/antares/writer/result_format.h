// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    zipArchive,
    // Store outputs in-memory
    inMemory
};
} // namespace Antares::Data
