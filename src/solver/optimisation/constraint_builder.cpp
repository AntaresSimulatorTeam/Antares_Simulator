#include "constraint_builder.h"

void ConstraintBuilder::build()
{
    std::vector<double>& Pi = problemeAResoudre.Pi;
    std::vector<int>& Colonne = problemeAResoudre.Colonne;
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
                // Matrix
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          &problemeAResoudre, Pi, Colonne, nombreDeTermes_, operator_);
    }
    nombreDeTermes_ = 0;
}

int ConstraintBuilder::getVariableIndex(const Variable::Variant& variable,
                                        int shift,
                                        bool wrap,
                                        int delta) const
{
    int pdt = hourInWeek_ + shift;
    const int nbTimeSteps = problemeHebdo.NombreDePasDeTempsPourUneOptimisation;
    // if (wrap)
    // {
    //     pdt %= nbTimeSteps;
    //     if (pdt < 0)
    //         pdt += problemeHebdo.NombreDePasDeTemps;
    // }
    // if (pdt < 0 || pdt >= nbTimeSteps)
    // {
    //     return -1;
    // }

    if (wrap)
    {
        if (shift >= 0)
        {
            pdt = (pdt) % nbTimeSteps;
        }
        else
        {
            pdt = (pdt + delta) % nbTimeSteps;
        }
    }
    const Variable::ConstraintVisitor visitor(varNative[pdt],
                                              problemeHebdo.NumeroDeVariableStockFinal,
                                              problemeHebdo.NumeroDeVariableDeTrancheDeStock);
    return std::visit(visitor, variable);
}

ConstraintBuilder& ConstraintBuilder::include(Variable::Variant var,
                                              double coeff,
                                              int shift,
                                              bool wrap,
                                              int delta)
{
    std::vector<double>& Pi = problemeAResoudre.Pi;
    std::vector<int>& Colonne = problemeAResoudre.Colonne;
    int varIndex = getVariableIndex(var, shift, wrap, delta);
    if (varIndex >= 0)
    {
        Pi[nombreDeTermes_] = coeff;
        Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
    return *this;
}