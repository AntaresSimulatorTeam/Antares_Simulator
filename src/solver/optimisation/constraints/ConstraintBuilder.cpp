#include "ConstraintBuilder.h"

void ConstraintBuilder::build()
{
    if (nombreDeTermes_ > 0)
    {
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes();
    }
    nombreDeTermes_ = 0;
}

int ConstraintBuilder::GetShiftedTimeStep(int offset, int delta) const
{
    int pdt = hourInWeek_ + offset;
    const int nbTimeSteps = data.NombreDePasDeTempsPourUneOptimisation;

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

void ConstraintBuilder::AddVariable(int varIndex, double coeff)
{
    if (varIndex >= 0)
    {
        data.Pi[nombreDeTermes_] = coeff;
        data.Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
}
NewVariable::VariableManager ConstraintBuilder::GetVariableManager(int offset,
                                                                         int delta) const
{
    auto pdt = GetShiftedTimeStep(offset, delta);
    return NewVariable::VariableManager(data.CorrespondanceVarNativesVarOptim[pdt],
                                        data.NumeroDeVariableStockFinal,
                                        data.NumeroDeVariableDeTrancheDeStock);
}
ConstraintBuilder& ConstraintBuilder::DispatchableProduction(unsigned int index,
                                                                   double coeff,
                                                                   int offset,
                                                                   int delta)
{
    AddVariable(GetVariableManager(offset, delta).DispatchableProduction(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberOfDispatchableUnits(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).NumberOfDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStoppingDispatchableUnits(unsigned int index,
                                                                      double coeff)
{
    AddVariable(GetVariableManager(0, 0).NumberStoppingDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStartingDispatchableUnits(unsigned int index,
                                                                      double coeff)
{
    AddVariable(GetVariableManager(0, 0).NumberStartingDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberBreakingDownDispatchableUnits(unsigned int index,
                                                                          double coeff)
{
    AddVariable(GetVariableManager(0, 0).NumberBreakingDownDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NTCDirect(unsigned int index,
                                                double coeff,
                                                int offset,
                                                int delta)
{
    AddVariable(GetVariableManager(offset, delta).NTCDirect(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoDirectCost(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).IntercoDirectCost(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoIndirectCost(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).IntercoIndirectCost(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageInjection(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).ShortTermStorageInjection(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageWithdrawal(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).ShortTermStorageWithdrawal(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageLevel(unsigned int index,
                                                            double coeff,
                                                            int offset,
                                                            int delta)
{
    AddVariable(GetVariableManager(offset, delta).ShortTermStorageLevel(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProd(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).HydProd(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdDown(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).HydProdDown(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdUp(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).HydProdUp(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Pumping(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).Pumping(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydroLevel(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).HydroLevel(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Overflow(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).Overflow(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::FinalStorage(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).FinalStorage(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::PositiveUnsuppliedEnergy(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).PositiveUnsuppliedEnergy(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NegativeUnsuppliedEnergy(unsigned int index, double coeff)
{
    AddVariable(GetVariableManager(0, 0).NegativeUnsuppliedEnergy(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::LayerStorage(unsigned area, unsigned layer, double coeff)
{
    AddVariable(GetVariableManager(0, 0).LayerStorage(area, layer), coeff);
    return *this;
}

void ConstraintBuilder::OPT_ChargerLaContrainteDansLaMatriceDesContraintes()
{
    data.IndicesDebutDeLigne[data.nombreDeContraintes]
      = data.nombreDeTermesDansLaMatriceDeContrainte;
    for (int i = 0; i < nombreDeTermes_; i++)
    {
        data.CoefficientsDeLaMatriceDesContraintes[data.nombreDeTermesDansLaMatriceDeContrainte]
          = data.Pi[i];
        data.IndicesColonnes[data.nombreDeTermesDansLaMatriceDeContrainte] = data.Colonne[i];
        data.nombreDeTermesDansLaMatriceDeContrainte++;
        if (data.nombreDeTermesDansLaMatriceDeContrainte
            == data.NombreDeTermesAllouesDansLaMatriceDesContraintes)
        {
            OPT_AugmenterLaTailleDeLaMatriceDesContraintes();
        }
    }
    data.NombreDeTermesDesLignes[data.nombreDeContraintes] = nombreDeTermes_;

    data.Sens[data.nombreDeContraintes] = operator_;
    data.nombreDeContraintes++;

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