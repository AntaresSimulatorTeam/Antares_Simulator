// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/parser.h"

#include "antares/io/inputs/yml-model/Library.h"

#include "decoders.hxx"

namespace Antares::IO::Inputs::YmlModel
{
Library Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);

    Library library = root["library"].as<Library>();

    return library;
}
} // namespace Antares::IO::Inputs::YmlModel
