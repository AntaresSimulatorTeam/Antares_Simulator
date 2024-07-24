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
#include "antares/solver/utils/name_translator.h"

#include <algorithm>
#include <iterator>

char** RealName::translate(const std::vector<std::string>& src, std::vector<char*>& pointerVec)
{
    std::transform(src.begin(),
                   src.end(),
                   std::back_inserter(pointerVec),
                   [](const std::string& str)
                   { return str.empty() ? nullptr : const_cast<char*>(str.data()); });
    return pointerVec.data();
}

char** NullName::translate(const std::vector<std::string>& src, std::vector<char*>& pointerVec)
{
    pointerVec.assign(src.size(), nullptr);
    return pointerVec.data();
}

std::unique_ptr<NameTranslator> NameTranslator::create(bool useRealNames)
{
    if (useRealNames)
    {
        return std::make_unique<RealName>();
    }
    else
    {
        return std::make_unique<NullName>();
    }
}
