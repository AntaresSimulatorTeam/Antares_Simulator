#include "new_constraint_builder.h"

void NewConstraintBuilder::build()
{
    if (nombreDeTermes_ > 0)
    {
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes();
    }
    nombreDeTermes_ = 0;
}

int NewConstraintBuilder::GetShiftedTimeStep(int offset, int delta) const
{
    int pdt = hourInWeek_ + offset;
    const int nbTimeSteps = data->NombreDePasDeTempsPourUneOptimisation;

    if (const bool shifted_timestep = offset != 0; shifted_timestep)
    {
        if (offset >= 0)
        {
            pdt = pdt % nbTimeSteps;
        }
        else
        {
            pdt = (pdt + delta) % nbTimeSteps;
        }
    }
    return pdt;
}

void NewConstraintBuilder::AddVariable(int varIndex, double coeff)
{
    if (varIndex >= 0)
    {
        data->Pi[nombreDeTermes_] = coeff;
        data->Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
}
NewVariable::NewVariableManager NewConstraintBuilder::GetVariableManager(int offset,
                                                                         int delta) const
{
    auto pdt = GetShiftedTimeStep(offset, delta);
    return NewVariable::NewVariableManager(data->CorrespondanceVarNativesVarOptim[pdt],
                                           data->NumeroDeVariableStockFinal,
                                           data->NumeroDeVariableDeTrancheDeStock);
}
NewConstraintBuilder& NewConstraintBuilder::DispatchableProduction(unsigned int index,
                                                                   double coeff,
                                                                   int offset,
                                                                   int delta)
{
    AddVariable(GetVariableManager(offset, delta).DispatchableProduction(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::NumberOfDispatchableUnits(unsigned int index,
                                                                      double coeff,
                                                                      int offset,
                                                                      int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberOfDispatchableUnits(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::NumberStoppingDispatchableUnits(unsigned int index,
                                                                            double coeff,
                                                                            int offset,
                                                                            int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberStoppingDispatchableUnits(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::NumberStartingDispatchableUnits(unsigned int index,
                                                                            double coeff,
                                                                            int offset,
                                                                            int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberStartingDispatchableUnits(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::NumberBreakingDownDispatchableUnits(unsigned int index,
                                                                                double coeff,
                                                                                int offset,
                                                                                int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberBreakingDownDispatchableUnits(index),
                coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::NTCDirect(unsigned int index,
                                                      double coeff,
                                                      int offset,
                                                      int delta)
{
    AddVariable(GetVariableManager(offset, delta).NTCDirect(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::IntercoDirectCost(unsigned int index,
                                                              double coeff,
                                                              int offset,
                                                              int delta)
{
    AddVariable(GetVariableManager(offset, delta).IntercoDirectCost(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::IntercoIndirectCost(unsigned int index,
                                                                double coeff,
                                                                int offset,
                                                                int delta)
{
    AddVariable(GetVariableManager(offset, delta).IntercoIndirectCost(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::ShortTermStorageInjection(unsigned int index,
                                                                      double coeff,
                                                                      int offset,
                                                                      int delta)
{
    AddVariable(GetVariableManager(offset, delta).ShortTermStorageInjection(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::ShortTermStorageWithdrawal(unsigned int index,
                                                                       double coeff,
                                                                       int offset,
                                                                       int delta)
{
    AddVariable(GetVariableManager(offset, delta).ShortTermStorageWithdrawal(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::ShortTermStorageLevel(unsigned int index,
                                                                  double coeff,
                                                                  int offset,
                                                                  int delta)
{
    AddVariable(GetVariableManager(offset, delta).ShortTermStorageLevel(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::HydProd(unsigned int index,
                                                    double coeff,
                                                    int offset,
                                                    int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydProd(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::HydProdDown(unsigned int index,
                                                        double coeff,
                                                        int offset,
                                                        int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydProdDown(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::HydProdUp(unsigned int index,
                                                      double coeff,
                                                      int offset,
                                                      int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydProdUp(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::Pumping(unsigned int index,
                                                    double coeff,
                                                    int offset,
                                                    int delta)
{
    AddVariable(GetVariableManager(offset, delta).Pumping(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::HydroLevel(unsigned int index,
                                                       double coeff,
                                                       int offset,
                                                       int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydroLevel(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::Overflow(unsigned int index,
                                                     double coeff,
                                                     int offset,
                                                     int delta)
{
    AddVariable(GetVariableManager(offset, delta).Overflow(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::FinalStorage(unsigned int index,
                                                         double coeff,
                                                         int offset,
                                                         int delta)
{
    AddVariable(GetVariableManager(offset, delta).FinalStorage(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::PositiveUnsuppliedEnergy(unsigned int index,
                                                                     double coeff,
                                                                     int offset,
                                                                     int delta)
{
    AddVariable(GetVariableManager(offset, delta).PositiveUnsuppliedEnergy(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::NegativeUnsuppliedEnergy(unsigned int index,
                                                                     double coeff,
                                                                     int offset,
                                                                     int delta)
{
    AddVariable(GetVariableManager(offset, delta).NegativeUnsuppliedEnergy(index), coeff);
    return *this;
}

NewConstraintBuilder& NewConstraintBuilder::LayerStorage(unsigned area,
                                                         unsigned layer,
                                                         double coeff,
                                                         int offset,
                                                         int delta)
{
    AddVariable(GetVariableManager(offset, delta).LayerStorage(area, layer), coeff);
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