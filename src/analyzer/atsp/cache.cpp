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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "atsp.h"

using namespace Yuni;

#define SEP Yuni::Core::IO::Separator

namespace Antares
{
void ATSP::cacheCreate()
{
    pCacheMatrix = new Matrix<>[pArea.size()];
    pCacheMemoryUsed = sizeof(Matrix<>) * pArea.size();
    pCacheLastValidIndex = 0;
}

void ATSP::cacheDestroy()
{
    delete[] pCacheMatrix;
    pCacheMatrix = nullptr;
}

void ATSP::cacheClear()
{
    for (uint i = 0; i != pCacheLastValidIndex; ++i)
    {
        pCacheMatrix[i].clear();
    }
    pCacheMemoryUsed = sizeof(Matrix<>) * pArea.size();
    pCacheLastValidIndex = 0;
}

} // namespace Antares
