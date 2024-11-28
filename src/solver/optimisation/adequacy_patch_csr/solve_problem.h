
#pragma once

#include "antares/study/parameters/adq-patch-params.h"

#include "../opt_structure_probleme_a_resoudre.h"
#include "hourly_csr_problem.h"

using namespace Antares::Data::AdequacyPatch;

bool ADQ_PATCH_CSR(PROBLEME_ANTARES_A_RESOUDRE&,
                   HourlyCSRProblem&,
                   const AdqPatchParams&,
                   unsigned int week,
                   int year);
