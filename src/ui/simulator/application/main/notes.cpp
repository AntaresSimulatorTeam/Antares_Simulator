// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "../main.h"
#include "../../windows/notes.h"
#include "../../windows/sets.h"

using namespace Yuni;

namespace Antares::Forms
{
void ApplWnd::loadUserNotes()
{
    assert(pUserNotes);
    pUserNotes->loadFromStudy();
}

void ApplWnd::saveUserNotes()
{
    assert(pUserNotes);
    pUserNotes->saveToStudy();
}

void ApplWnd::loadSets()
{
    assert(pSets);
    pSets->loadFromStudy();
}

void ApplWnd::saveSets()
{
    assert(pSets);
    pSets->saveToStudy();
}

} // namespace Antares::Forms
