/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "ConstraintBuilder.h"

void ConstraintBuilder::build()
{
    std::vector<double>& Pi = problemeAResoudre.Pi;
    std::vector<int>& Colonne = problemeAResoudre.Colonne;

    if (nombreDeTermes_ > 0)
    {
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          &problemeAResoudre, Pi, Colonne, nombreDeTermes_, operator_);
    }
    nombreDeTermes_ = 0;
}

int ConstraintBuilder::GetShiftedTimeStep(int offset, int delta) const
{
    int pdt = hourInWeek_ + offset;
    const int nbTimeSteps = problemeHebdo.NombreDePasDeTempsPourUneOptimisation;

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
    std::vector<double>& Pi = problemeAResoudre.Pi;
    std::vector<int>& Colonne = problemeAResoudre.Colonne;
    if (varIndex >= 0)
    {
        Pi[nombreDeTermes_] = coeff;
        Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
}

Variable::VariableManager ConstraintBuilder::GetVariableManager(int offset, int delta) const
{
    auto pdt = GetShiftedTimeStep(offset, delta);
    return Variable::VariableManager(varNative[pdt],
                                     problemeHebdo.NumeroDeVariableStockFinal,
                                     problemeHebdo.NumeroDeVariableDeTrancheDeStock);
}

ConstraintBuilder& ConstraintBuilder::DispatchableProduction(unsigned int index,
                                                             double coeff,
                                                             int offset,
                                                             int delta)
{
    AddVariable(GetVariableManager(offset, delta).DispatchableProduction(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberOfDispatchableUnits(unsigned int index,
                                                                double coeff,
                                                                int offset,
                                                                int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberOfDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStoppingDispatchableUnits(unsigned int index,
                                                                      double coeff,
                                                                      int offset,
                                                                      int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberStoppingDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStartingDispatchableUnits(unsigned int index,
                                                                      double coeff,
                                                                      int offset,
                                                                      int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberStartingDispatchableUnits(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberBreakingDownDispatchableUnits(unsigned int index,
                                                                          double coeff,
                                                                          int offset,
                                                                          int delta)
{
    AddVariable(GetVariableManager(offset, delta).NumberBreakingDownDispatchableUnits(index),
                coeff);
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

ConstraintBuilder& ConstraintBuilder::IntercoDirectCost(unsigned int index,
                                                        double coeff,
                                                        int offset,
                                                        int delta)
{
    AddVariable(GetVariableManager(offset, delta).IntercoDirectCost(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoIndirectCost(unsigned int index,
                                                          double coeff,
                                                          int offset,
                                                          int delta)
{
    AddVariable(GetVariableManager(offset, delta).IntercoIndirectCost(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageInjection(unsigned int index,
                                                                double coeff,
                                                                int offset,
                                                                int delta)
{
    AddVariable(GetVariableManager(offset, delta).ShortTermStorageInjection(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageWithdrawal(unsigned int index,
                                                                 double coeff,
                                                                 int offset,
                                                                 int delta)
{
    AddVariable(GetVariableManager(offset, delta).ShortTermStorageWithdrawal(index), coeff);
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

ConstraintBuilder& ConstraintBuilder::HydProd(unsigned int index,
                                              double coeff,
                                              int offset,
                                              int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydProd(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdDown(unsigned int index,
                                                  double coeff,
                                                  int offset,
                                                  int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydProdDown(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdUp(unsigned int index,
                                                double coeff,
                                                int offset,
                                                int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydProdUp(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Pumping(unsigned int index,
                                              double coeff,
                                              int offset,
                                              int delta)
{
    AddVariable(GetVariableManager(offset, delta).Pumping(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydroLevel(unsigned int index,
                                                 double coeff,
                                                 int offset,
                                                 int delta)
{
    AddVariable(GetVariableManager(offset, delta).HydroLevel(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Overflow(unsigned int index,
                                               double coeff,
                                               int offset,
                                               int delta)
{
    AddVariable(GetVariableManager(offset, delta).Overflow(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::FinalStorage(unsigned int index,
                                                   double coeff,
                                                   int offset,
                                                   int delta)
{
    AddVariable(GetVariableManager(offset, delta).FinalStorage(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::PositiveUnsuppliedEnergy(unsigned int index,
                                                               double coeff,
                                                               int offset,
                                                               int delta)
{
    AddVariable(GetVariableManager(offset, delta).PositiveUnsuppliedEnergy(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NegativeUnsuppliedEnergy(unsigned int index,
                                                               double coeff,
                                                               int offset,
                                                               int delta)
{
    AddVariable(GetVariableManager(offset, delta).NegativeUnsuppliedEnergy(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::LayerStorage(unsigned area,
                                                   unsigned layer,
                                                   double coeff,
                                                   int offset,
                                                   int delta)
{
    AddVariable(GetVariableManager(offset, delta).LayerStorage(area, layer), coeff);
    return *this;
}
