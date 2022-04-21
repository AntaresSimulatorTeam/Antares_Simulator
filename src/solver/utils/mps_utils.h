#pragma once

extern "C"
{
#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "srs_api.h"
}

void OPT_dump_spx_fixed_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace);
void OPT_dump_spx_variable_part(const PROBLEME_SIMPLEXE* Pb, uint numSpace);

void OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(void*, uint);
