#pragma once

#include "named_problem.h"

void OPT_dump_spx_fixed_part(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME& Pb,
                             uint numSpace,
                             bool incudeNames);
void OPT_dump_spx_variable_part(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME& Pb,
                                uint numSpace,
                                bool includeNames);

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void*, uint, bool);
