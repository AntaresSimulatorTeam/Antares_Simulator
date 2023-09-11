#include "constraint_builder.h"

void ConstraintBuilder::build()
{
    std::vector<double>& Pi = problemeAResoudre->Pi;
    std::vector<int>& Colonne = problemeAResoudre->Colonne;
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
                // Matrix
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes();
    }
    nombreDeTermes_ = 0;
}

int ConstraintBuilder::getVariableIndex(const Variable::Variant& variable,
                                        int shift,
                                        bool wrap,
                                        int delta) const
{
    int pdt = hourInWeek_ + shift;
    const int nbTimeSteps = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    // if (wrap)
    // {
    //     pdt %= nbTimeSteps;
    //     if (pdt < 0)
    //         pdt += problemeHebdo->NombreDePasDeTemps;
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
    const Variable::Visitor visitor(varNative[pdt],
                                    problemeHebdo->NumeroDeVariableStockFinal,
                                    problemeHebdo->NumeroDeVariableDeTrancheDeStock);
    return std::visit(visitor, variable);
}

ConstraintBuilder& ConstraintBuilder::include(Variable::Variant var,
                                              double coeff,
                                              int shift,
                                              bool wrap,
                                              int delta)
{
    std::vector<double>& Pi = problemeAResoudre->Pi;
    std::vector<int>& Colonne = problemeAResoudre->Colonne;
    int varIndex = getVariableIndex(var, shift, wrap, delta);
    if (varIndex >= 0)
    {
        Pi[nombreDeTermes_] = coeff;
        Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
    return *this;
}

void ConstraintBuilder::OPT_ChargerLaContrainteDansLaMatriceDesContraintes()
{
    int& nombreDeContraintes = problemeAResoudre->NombreDeContraintes;
    int& nombreDeTermesDansLaMatriceDeContrainte
      = problemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes;
    std::vector<double>& Pi = problemeAResoudre->Pi;
    std::vector<int>& Colonne = problemeAResoudre->Colonne;

    problemeAResoudre->IndicesDebutDeLigne[nombreDeContraintes]
      = nombreDeTermesDansLaMatriceDeContrainte;
    for (int i = 0; i < nombreDeTermes_; i++)
    {
        problemeAResoudre
          ->CoefficientsDeLaMatriceDesContraintes[nombreDeTermesDansLaMatriceDeContrainte]
          = Pi[i];
        problemeAResoudre->IndicesColonnes[nombreDeTermesDansLaMatriceDeContrainte] = Colonne[i];
        nombreDeTermesDansLaMatriceDeContrainte++;
        if (nombreDeTermesDansLaMatriceDeContrainte
            == problemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes)
        {
            OPT_AugmenterLaTailleDeLaMatriceDesContraintes();
        }
    }
    problemeAResoudre->NombreDeTermesDesLignes[nombreDeContraintes] = nombreDeTermes_;

    problemeAResoudre->Sens[nombreDeContraintes] = operator_;
    nombreDeContraintes++;

    return;
}

void ConstraintBuilder::OPT_AugmenterLaTailleDeLaMatriceDesContraintes()
{
    int NbTermes = problemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes;
    NbTermes += problemeAResoudre->IncrementDAllocationMatriceDesContraintes;

    logs.info();
    logs.info() << " Expected Number of Non-zero terms in Problem Matrix : increased to : "
                << NbTermes;
    logs.info();

    problemeAResoudre->CoefficientsDeLaMatriceDesContraintes.resize(NbTermes);

    problemeAResoudre->IndicesColonnes.resize(NbTermes);

    problemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
}