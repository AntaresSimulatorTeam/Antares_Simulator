#include <antares/solver/hydro/probleme_spx_wrapper.h>

#include "spx_fonctions.h"

void LibererProbleme::operator()(PROBLEME_SPX* p) const
{
    SPX_LibererProbleme(p);
}
