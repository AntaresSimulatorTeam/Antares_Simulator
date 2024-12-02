/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"

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

ConstraintBuilder& ConstraintBuilder::ShortTermStorageInjection(unsigned int index,
                                                                double coeff,
                                                                int offset,
                                                                int delta)
{
    AddVariable(variableManager_.ShortTermStorageInjection(index, hourInWeek_, offset, delta),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageWithdrawal(unsigned int index,
                                                                 double coeff,
                                                                 int offset,
                                                                 int delta)
{
    AddVariable(variableManager_.ShortTermStorageWithdrawal(index, hourInWeek_, offset, delta),
                coeff);
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

ConstraintBuilder& ConstraintBuilder::ShortTermCostVariationInjection(unsigned int index,
                                                                      double coeff,
                                                                      int offset,
                                                                      int delta)
{
    AddVariable(
      variableManager_.ShortTermStorageCostVariationInjection(index, hourInWeek_, offset, delta),
      coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermCostVariationWithdrawal(unsigned int index,
                                                                       double coeff,
                                                                       int offset,
                                                                       int delta)
{
    AddVariable(
      variableManager_.ShortTermStorageCostVariationWithdrawal(index, hourInWeek_, offset, delta),
      coeff);
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
    AddVariable(variableManager_.FinalStorage(index), coeff);
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
    AddVariable(variableManager_.LayerStorage(area, layer), coeff);
    return *this;
}

void ConstraintBuilder::OPT_ChargerLaContrainteDansLaMatriceDesContraintes()
{
    data.IndicesDebutDeLigne[data.nombreDeContraintes] = data
                                                           .nombreDeTermesDansLaMatriceDeContrainte;
    for (int i = 0; i < nombreDeTermes_; i++)
    {
        data.CoefficientsDeLaMatriceDesContraintes[data.nombreDeTermesDansLaMatriceDeContrainte]
          = data.Pi[i];
        data.IndicesColonnes[data.nombreDeTermesDansLaMatriceDeContrainte] = data.Colonne[i];
        data.nombreDeTermesDansLaMatriceDeContrainte++;
    }
    data.NombreDeTermesDesLignes[data.nombreDeContraintes] = nombreDeTermes_;

    data.Sens[data.nombreDeContraintes] = operator_;
    data.nombreDeContraintes++;

    return;
}
