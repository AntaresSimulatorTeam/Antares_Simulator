#include "constraint_builder.h"

// ConstraintBuilder& ConstraintBuilder::AddVariable(int var, double coeff)
// {
//     std::vector<double>& Pi = ProblemeAResoudre_->Pi;
//     std::vector<int>& Colonne = ProblemeAResoudre_->Colonne;

//     if (var >= 0)
//     {
//         Pi[nombreDeTermes_] = coeff;
//         Colonne[nombreDeTermes_] = var;
//         nombreDeTermes_++;
//     }
//     return *this;
// }
int ConstraintBuilder::build()
{
    std::vector<double>& Pi = problemeAResoudre.Pi;   // TODO const
    std::vector<int>& Colonne = problemeAResoudre.Colonne;  // TODO const
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          &problemeAResoudre, Pi, Colonne, nombreDeTermes_, operator_);
        return problemeAResoudre.NombreDeContraintes;
    }
    nombreDeTermes_ = 0;
    return -1;
}
