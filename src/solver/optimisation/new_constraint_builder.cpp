#include "new_constraint_builder.h"

void NewConstraintBuilder::build()
{
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
        // Matrix
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes();
    }
    nombreDeTermes_ = 0;
}

int NewConstraintBuilder::getVariableIndex(const NewVariable::Variant& variable,
                                           int shift,
                                           bool wrap,
                                           int delta) const
{
    auto pdt = hourInWeek_ + shift;
    const auto nbTimeSteps = data->NombreDePasDeTempsPourUneOptimisation;

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
    const NewVariable::NewVisitor visitor(data->CorrespondanceVarNativesVarOptim[pdt],
                                          data->NumeroDeVariableStockFinal,
                                          data->NumeroDeVariableDeTrancheDeStock);
    return std::visit(visitor, variable);
}

NewConstraintBuilder& NewConstraintBuilder::include(NewVariable::Variant var,
                                                    double coeff,
                                                    int shift,
                                                    bool wrap,
                                                    int delta)
{
    int varIndex = getVariableIndex(var, shift, wrap, delta);
    if (varIndex >= 0)
    {
        data->Pi[nombreDeTermes_] = coeff;
        data->Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
    return *this;
}

void NewConstraintBuilder::OPT_ChargerLaContrainteDansLaMatriceDesContraintes()
{
    data->IndicesDebutDeLigne[data->nombreDeContraintes]
      = data->nombreDeTermesDansLaMatriceDeContrainte;
    for (int i = 0; i < nombreDeTermes_; i++)
    {
        data->CoefficientsDeLaMatriceDesContraintes[data->nombreDeTermesDansLaMatriceDeContrainte]
          = data->Pi[i];
        data->IndicesColonnes[data->nombreDeTermesDansLaMatriceDeContrainte] = data->Colonne[i];
        data->nombreDeTermesDansLaMatriceDeContrainte++;
        if (data->nombreDeTermesDansLaMatriceDeContrainte
            == data->NombreDeTermesAllouesDansLaMatriceDesContraintes)
        {
            OPT_AugmenterLaTailleDeLaMatriceDesContraintes();
        }
    }
    data->NombreDeTermesDesLignes[data->nombreDeContraintes] = nombreDeTermes_;

    data->Sens[data->nombreDeContraintes] = operator_;
    data->nombreDeContraintes++;

    return;
}

void NewConstraintBuilder::OPT_AugmenterLaTailleDeLaMatriceDesContraintes()
{
    int NbTermes = data->NombreDeTermesAllouesDansLaMatriceDesContraintes;
    NbTermes += data->IncrementDAllocationMatriceDesContraintes;

    logs.info();
    logs.info() << " Expected Number of Non-zero terms in Problem Matrix : increased to : "
                << NbTermes;
    logs.info();

    data->CoefficientsDeLaMatriceDesContraintes.resize(NbTermes);

    data->IndicesColonnes.resize(NbTermes);

    data->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
}