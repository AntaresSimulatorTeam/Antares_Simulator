// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

#include "parser.h"

namespace Antares::IO::Inputs::SystemConverter
{

struct TwoFieldsOfSameRole final: std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

struct ConnectingPortToItSelf final: std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

ModelerStudy::SystemModel::System convert(
  const YmlSystem::System& ymlSystem,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries);

} // namespace Antares::IO::Inputs::SystemConverter
