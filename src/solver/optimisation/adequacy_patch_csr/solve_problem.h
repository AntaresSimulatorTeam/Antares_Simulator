
#pragma once

#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/study/parameters/adq-patch-params.h"

using namespace Antares::Data::AdequacyPatch;

bool ADQ_PATCH_CSR(PROBLEME_ANTARES_A_RESOUDRE&,
                   HourlyCSRProblem&,
                   const AdqPatchParams&,
                   unsigned int week,
                   int year);
