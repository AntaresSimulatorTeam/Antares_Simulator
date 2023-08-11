#include "constraint_builder.h"

ConstraintBuilder& ConstraintBuilder::AddVariable(int var, double coeff)
{
    std::vector<double>& Pi = ProblemeAResoudre_->Pi;
    std::vector<int>& Colonne = ProblemeAResoudre_->Colonne;

    if (var >= 0)
    {
        Pi[nombreDeTermes_] = coeff;
        Colonne[nombreDeTermes_] = var;
        nombreDeTermes_++;
    }
    return *this;
}
[[nodiscard]] int ConstraintBuilder::build(char constraint_operator)
{
    std::vector<double>& Pi = ProblemeAResoudre_->Pi;
    std::vector<int>& Colonne = ProblemeAResoudre_->Colonne;
    if (nombreDeTermes_ > 0)
    {
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          ProblemeAResoudre_, Pi, Colonne, nombreDeTermes_, constraint_operator);
        return ProblemeAResoudre_->NombreDeContraintes;
    }
    return -1;
}
