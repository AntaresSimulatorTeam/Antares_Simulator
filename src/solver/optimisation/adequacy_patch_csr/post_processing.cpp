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
#include "antares/solver/optimisation/adequacy_patch_csr/post_processing.h"

#include <cmath>

namespace Antares::Data::AdequacyPatch
{
double recomputeDTG_MRG(bool triggered, double dtgMrg, double ens)
{
    if (triggered)
    {
        return std::max(0.0, dtgMrg - ens);
    }
    else
    {
        return dtgMrg;
    }
}

double recomputeENS_MRG(bool triggered, double dtgMrg, double ens)
{
    if (triggered)
    {
        return std::max(0.0, ens - dtgMrg);
    }
    else
    {
        return ens;
    }
}

double recomputeMRGPrice(double ensCsr, double originalCost, double unsuppliedEnergyCost)
{
    if (ensCsr > 0.5)
    {
        return -unsuppliedEnergyCost;
    }
    else
    {
        return originalCost;
    }
}
} // namespace Antares::Data::AdequacyPatch
