#include "ConstraintBuilder.h"

void ConstraintBuilder::build()
{
    if (nombreDeTermes_ > 0)
    {
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes();
    }
    nombreDeTermes_ = 0;
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

ConstraintBuilder& ConstraintBuilder::DispatchableProduction(unsigned int index,
                                                                   double coeff,
                                                                   int offset,
                                                                   int delta)
{
    AddVariable(variableManager_.DispatchableProduction(index, hourInWeek_, offset, delta), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberOfDispatchableUnits(unsigned int index, double coeff)
{
    AddVariable(variableManager_.NumberOfDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStoppingDispatchableUnits(unsigned int index,
                                                                      double coeff)
{
    AddVariable(variableManager_.NumberStoppingDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStartingDispatchableUnits(unsigned int index,
                                                                      double coeff)
{
    AddVariable(variableManager_.NumberStartingDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberBreakingDownDispatchableUnits(unsigned int index,
                                                                          double coeff)
{
    AddVariable(variableManager_.NumberBreakingDownDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NTCDirect(unsigned int index,
                                                double coeff,
                                                int offset,
                                                int delta)
{
    AddVariable(variableManager_.NTCDirect(index, hourInWeek_, offset, delta), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoDirectCost(unsigned int index, double coeff)
{
    AddVariable(variableManager_.IntercoDirectCost(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoIndirectCost(unsigned int index, double coeff)
{
    AddVariable(variableManager_.IntercoIndirectCost(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageInjection(unsigned int index, double coeff)
{
    AddVariable(variableManager_.ShortTermStorageInjection(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageWithdrawal(unsigned int index, double coeff)
{
    AddVariable(variableManager_.ShortTermStorageWithdrawal(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageLevel(unsigned int index,
                                                            double coeff,
                                                            int offset,
                                                            int delta)
{
    AddVariable(variableManager_.ShortTermStorageLevel(index, hourInWeek_, offset, delta), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProd(unsigned int index, double coeff)
{
    AddVariable(variableManager_.HydProd(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdDown(unsigned int index, double coeff)
{
    AddVariable(variableManager_.HydProdDown(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdUp(unsigned int index, double coeff)
{
    AddVariable(variableManager_.HydProdUp(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Pumping(unsigned int index, double coeff)
{
    AddVariable(variableManager_.Pumping(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydroLevel(unsigned int index, double coeff)
{
    AddVariable(variableManager_.HydroLevel(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Overflow(unsigned int index, double coeff)
{
    AddVariable(variableManager_.Overflow(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::FinalStorage(unsigned int index, double coeff)
{
    AddVariable(variableManager_.FinalStorage(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::PositiveUnsuppliedEnergy(unsigned int index, double coeff)
{
    AddVariable(variableManager_.PositiveUnsuppliedEnergy(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NegativeUnsuppliedEnergy(unsigned int index, double coeff)
{
    AddVariable(variableManager_.NegativeUnsuppliedEnergy(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::LayerStorage(unsigned area, unsigned layer, double coeff)
{
    AddVariable(variableManager_.LayerStorage(area, layer, hourInWeek_), coeff);
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