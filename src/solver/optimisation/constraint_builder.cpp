#include "constraint_builder.h"

void ConstraintBuilder::build()
{
    const std::vector<double>& Pi = problemeAResoudre.Pi;
    const std::vector<int>& Colonne = problemeAResoudre.Colonne;
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
        // // RHS
        // {
        //   const int idx = problemeAResoudre.NombreDeContraintes;
        //   problemeAResoudre.SecondMembre[idx] = rhs_;
        // }
        // Matrix
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          &problemeAResoudre, Pi, Colonne, nombreDeTermes_, operator_);
    }
    nombreDeTermes_ = 0;
}
