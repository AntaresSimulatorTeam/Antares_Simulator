// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/wind/prepro.h"

#include <antares/logs/logs.h>
#include "antares/study/study.h"

namespace Antares::Data::Wind
{
Prepro::Prepro():
    xcast(timeSeriesWind)
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

} // namespace Antares::Data::Wind
