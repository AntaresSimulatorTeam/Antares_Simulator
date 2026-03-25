// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/io/inputs/InputError.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

#include "parser.h"

namespace Antares::IO::Inputs::SystemConverter
{

struct ComponentNotFound final: IO::Inputs::InputError
{
    using IO::Inputs::InputError::InputError;
};

struct IncompatiblePortTypes final: IO::Inputs::InputError
{
    using IO::Inputs::InputError::InputError;
};

struct DuplicateComponentId final: IO::Inputs::InputError
{
    using IO::Inputs::InputError::InputError;
};

ModelerStudy::SystemModel::System convert(
  const YmlSystem::System& ymlSystem,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries);

} // namespace Antares::IO::Inputs::SystemConverter
