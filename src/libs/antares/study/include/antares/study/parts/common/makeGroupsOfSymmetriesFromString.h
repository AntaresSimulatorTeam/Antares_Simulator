// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <BaseErrorListener.h>
#include <set>
#include <string>
#include <vector>

namespace antlr4
{
class Recognizer;
}

namespace Antares::Data::Symmetries
{
struct SymmetriesError final: std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

std::vector<std::set<std::string>> makeGroupsOfSymmetries(const std::string& symmetriesField);

} // namespace Antares::Data::Symmetries
