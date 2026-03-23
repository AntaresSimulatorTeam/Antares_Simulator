// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/decoders.h"

std::string getBaseTree(const std::filesystem::path& nodeTagPath)
{
    return printPathTree(nodeTagPath);
}
