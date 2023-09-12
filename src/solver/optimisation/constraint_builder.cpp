#include "constraint_builder.h"

void ConstraintBuilder::build()
{
    std::vector<double>& Pi = data.Pi;
    std::vector<int>& Colonne = data.Colonne;
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
    const int nbTimeSteps = data.NombreDePasDeTempsPourUneOptimisation;
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
    const Variable::Visitor visitor(data.CorrespondanceVarNativesVarOptim[pdt],
                                    data.NumeroDeVariableStockFinal,
                                    data.NumeroDeVariableDeTrancheDeStock);
    return std::visit(visitor, variable);
}

ConstraintBuilder& ConstraintBuilder::include(Variable::Variant var,
                                              double coeff,
                                              int shift,
                                              bool wrap,
                                              int delta)
{
    std::vector<double>& Pi = data.Pi;
    std::vector<int>& Colonne = data.Colonne;
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
    int& nombreDeContraintes = data.nombreDeContraintes;
    int& nombreDeTermesDansLaMatriceDeContrainte = data.nombreDeTermesDansLaMatriceDeContrainte;
    std::vector<double>& Pi = data.Pi;
    std::vector<int>& Colonne = data.Colonne;

    data.IndicesDebutDeLigne[nombreDeContraintes] = nombreDeTermesDansLaMatriceDeContrainte;
    for (int i = 0; i < nombreDeTermes_; i++)
    {
        data.CoefficientsDeLaMatriceDesContraintes[nombreDeTermesDansLaMatriceDeContrainte] = Pi[i];
        data.IndicesColonnes[nombreDeTermesDansLaMatriceDeContrainte] = Colonne[i];
        nombreDeTermesDansLaMatriceDeContrainte++;
        if (nombreDeTermesDansLaMatriceDeContrainte
            == data.NombreDeTermesAllouesDansLaMatriceDesContraintes)
        {
            OPT_AugmenterLaTailleDeLaMatriceDesContraintes();
        }
    }
    data.NombreDeTermesDesLignes[nombreDeContraintes] = nombreDeTermes_;

    data.Sens[nombreDeContraintes] = operator_;
    nombreDeContraintes++;

    return;
}

void ConstraintBuilder::OPT_AugmenterLaTailleDeLaMatriceDesContraintes()
{
    int NbTermes = data.NombreDeTermesAllouesDansLaMatriceDesContraintes;
    NbTermes += data.IncrementDAllocationMatriceDesContraintes;

    logs.info();
    logs.info() << " Expected Number of Non-zero terms in Problem Matrix : increased to : "
                << NbTermes;
    logs.info();

    data.CoefficientsDeLaMatriceDesContraintes.resize(NbTermes);

    data.IndicesColonnes.resize(NbTermes);

    data.NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
}