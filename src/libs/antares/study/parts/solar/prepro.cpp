/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/study/parts/solar/prepro.h"

#include <yuni/yuni.h>

#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares
{
namespace Data
{
namespace Solar
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

} // namespace Solar
} // namespace Data
} // namespace Antares
