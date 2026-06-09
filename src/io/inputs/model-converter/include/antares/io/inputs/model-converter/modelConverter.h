// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/library.h"

namespace Antares::IO::Inputs::ModelConverter
{
ModelerStudy::SystemModel::Library convert(const YmlModel::Library& library);
} // namespace Antares::IO::Inputs::ModelConverter
