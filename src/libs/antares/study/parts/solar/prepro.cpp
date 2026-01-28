// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/solar/prepro.h"

#include <yuni/yuni.h>

#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares::Data::Solar
{
Prepro::Prepro():
    xcast(timeSeriesSolar)
{
}

Prepro::~Prepro()
{
}

bool Prepro::loadFromFolder(const std::filesystem::path& folder)
{
    return xcast.loadFromFolder(folder);
}

bool Prepro::saveToFolder(const AnyString& folder) const
{
    return xcast.saveToFolder(folder);
}

} // namespace Antares::Data::Solar
