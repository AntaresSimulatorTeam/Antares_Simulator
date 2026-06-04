// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/solver/hydro/probleme_spx_wrapper.h>

#include "spx_fonctions.h"

void LibererProbleme::operator()(PROBLEME_SPX* p) const
{
    SPX_LibererProbleme(p);
}
