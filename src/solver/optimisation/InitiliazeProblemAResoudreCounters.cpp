#include "opt_fonctions.h"
#include "../simulation/sim_structure_probleme_economique.h"

void InitiliazeProblemAResoudreCounters(PROBLEME_HEBDO* problemeHebdo)
{
    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
}