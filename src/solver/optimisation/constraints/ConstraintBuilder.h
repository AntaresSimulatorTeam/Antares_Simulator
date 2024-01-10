/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
#pragma once

#include <antares/logs/logs.h>
#include "../opt_structure_probleme_a_resoudre.h"
#include "../opt_rename_problem.h"
#include "../opt_fonctions.h"
#include "../../simulation/sim_structure_probleme_economique.h"

#include <utility>

namespace Variable
{

/*!
Factory class that hold variables indices
*/
class VariableManager
{
public:
    VariableManager(const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar,
                    const std::vector<int>& NumeroDeVariableStockFinal,
                    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock) :
     nativeOptimVar(nativeOptimVar),
     NumeroDeVariableStockFinal(NumeroDeVariableStockFinal),
     NumeroDeVariableDeTrancheDeStock(NumeroDeVariableDeTrancheDeStock)
    {
    }

    int DispatchableProduction(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuPalierThermique[index];
    }

    int NumberOfDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index];
    }

    int NumberStoppingDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index];
    }

    int NumberStartingDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index];
    }

    int NumberBreakingDownDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar
          .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index];
    }

    int NTCDirect(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDeLInterconnexion[index];
    }

    int IntercoDirectCost(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index];
    }

    int IntercoIndirectCost(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index];
    }

    int ShortTermStorageInjection(unsigned int index) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.InjectionVariable[index];
    }

    int ShortTermStorageWithdrawal(unsigned int index) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.WithdrawalVariable[index];
    }

    int ShortTermStorageLevel(unsigned int index) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.LevelVariable[index];
    }

    int HydProd(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDeLaProdHyd[index];
    }

    int HydProdDown(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesVariationHydALaBaisse[index];
    }

    int HydProdUp(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesVariationHydALaHausse[index];
    }

    int Pumping(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDePompage[index];
    }

    int HydroLevel(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDeNiveau[index];
    }

    int Overflow(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDeDebordement[index];
    }

    int FinalStorage(unsigned int index) const
    {
        return NumeroDeVariableStockFinal[index];
    }

    int LayerStorage(unsigned area, unsigned layer) const
    {
        return NumeroDeVariableDeTrancheDeStock[area][layer];
    }

    int PositiveUnsuppliedEnergy(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDefaillancePositive[index];
    }

    int NegativeUnsuppliedEnergy(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDefaillanceNegative[index];
    }

private:
    const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar;
    const std::vector<int>& NumeroDeVariableStockFinal;
    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
};

} // namespace Variable

/*! \verbatim
this class build up the business object 'Constraint',
Math:
|coeff11 coeff12 .. coeff1n||var1| |sign_1|   |rhs1|       |constraint1||sign_1||rhs1|
|..      ..             ...||....| |......|   |....| <===> |...........||......||....|
|coeffn1 coeffn2 .. coeffnn||varn| |sign_n|   |rhsn|       |constraintn||sign_n||rhsn|

it propose a set of methods  to attach 'Variables' to the Constraint
ex: calling NTCDirect() implies adding Direct NTC Variable to the current Constraint
finally the build() method gather all variables and put them into the matrix
\endverbatim
*/
class ConstraintBuilder
{
public:
    ConstraintBuilder(
      PROBLEME_HEBDO& problemeHebdo,
      const std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim) :
     problemeHebdo(problemeHebdo),
     problemeAResoudre(*problemeHebdo.ProblemeAResoudre),
     varNative(CorrespondanceVarNativesVarOptim)
    {
    }

    ConstraintBuilder& updateHourWithinWeek(unsigned hour)
    {
        hourInWeek_ = hour;
        return *this;
    }

    /** @name variables_method
     *  @brief  Documentation for non obvious methods
     *  @param index: local index of the variable
     *  @param offset: offset from the current time step
     *  @param delta: number of time steps for the variable
     *  @return reference of *this
     */
    //@{
    ConstraintBuilder& DispatchableProduction(unsigned int index,
                                              double coeff,
                                              int offset = 0,
                                              int delta = 0);

    ConstraintBuilder& NumberOfDispatchableUnits(unsigned int index,
                                                 double coeff,
                                                 int offset = 0,
                                                 int delta = 0);

    ConstraintBuilder& NumberStoppingDispatchableUnits(unsigned int index,
                                                       double coeff,
                                                       int offset = 0,
                                                       int delta = 0);

    ConstraintBuilder& NumberStartingDispatchableUnits(unsigned int index,
                                                       double coeff,
                                                       int offset = 0,
                                                       int delta = 0);

    ConstraintBuilder& NumberBreakingDownDispatchableUnits(unsigned int index,
                                                           double coeff,
                                                           int offset = 0,

                                                           int delta = 0);

    ConstraintBuilder& NTCDirect(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& IntercoDirectCost(unsigned int index,
                                         double coeff,
                                         int offset = 0,
                                         int delta = 0);

    ConstraintBuilder& IntercoIndirectCost(unsigned int index,
                                           double coeff,
                                           int offset = 0,
                                           int delta = 0);

    ConstraintBuilder& ShortTermStorageInjection(unsigned int index,
                                                 double coeff,
                                                 int offset = 0,
                                                 int delta = 0);

    ConstraintBuilder& ShortTermStorageWithdrawal(unsigned int index,
                                                  double coeff,
                                                  int offset = 0,
                                                  int delta = 0);

    ConstraintBuilder& ShortTermStorageLevel(unsigned int index,
                                             double coeff,
                                             int offset = 0,
                                             int delta = 0);

    ConstraintBuilder& HydProd(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& HydProdDown(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& HydProdUp(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& Pumping(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& HydroLevel(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& Overflow(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& FinalStorage(unsigned int index,
                                    double coeff,
                                    int offset = 0,
                                    int delta = 0);

    ConstraintBuilder& PositiveUnsuppliedEnergy(unsigned int index,
                                                double coeff,
                                                int offset = 0,
                                                int delta = 0);
    ConstraintBuilder& NegativeUnsuppliedEnergy(unsigned int index,
                                                double coeff,
                                                int offset = 0,
                                                int delta = 0);

    ConstraintBuilder& LayerStorage(unsigned area,
                                    unsigned layer,
                                    double coeff,
                                    int offset = 0,
                                    int delta = 0);
    //@}

    class ConstraintBuilderInvalidOperator : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /*!
       @brief set the operator of the constraint (sign)
       @param op: the operator of the constraint
       @return reference of *this
    */
    ConstraintBuilder& SetOperator(char op)
    {
        if (op == '<' || op == '=' || op == '>')
        {
            operator_ = op;
        }
        else
            throw ConstraintBuilderInvalidOperator(std::string("Invalid operator: ") + op);

        return *this;
    }

    /*!
     @brief set the sign of the constraint to '=',
     building a constraint equal to rhs
     @return reference of *this
    */
    ConstraintBuilder& equalTo()
    {
        operator_ = '=';
        return *this;
    }

    /*!
    @brief set the sign of the constraint to '<',
    building a constraint less than rhs
    @return reference of *this
    */
    ConstraintBuilder& lessThan()
    {
        operator_ = '<';
        return *this;
    }

    /*!
     @brief set the sign of the constraint to '>',
      building a constraint greather than rhs
     @return reference of *this
    */
    ConstraintBuilder& greaterThan()
    {
        operator_ = '>';
        return *this;
    }

    /*!
      @brief add the constraint in the matrix
      @return
     */
    void build();

    int NumberOfVariables() const
    {
        return nombreDeTermes_;
    }

private:
    PROBLEME_HEBDO& problemeHebdo;
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre;
    const std::vector<CORRESPONDANCES_DES_VARIABLES>& varNative;

    unsigned int hourInWeek_ = 0;

    char operator_ = '=';
    int nombreDeTermes_ = 0;

    int GetShiftedTimeStep(int offset, int delta) const;
    void AddVariable(int index, double coeff);

    /*!
     * @brief
     * @param offset: offset from the current time step
     * @param delta: number of time steps for the variable
     * @return VariableManager object
     */
    Variable::VariableManager GetVariableManager(int offset = 0, int delta = 0) const;
};

/*! factory class to build a Constraint */
class ConstraintFactory
{
public:
    explicit ConstraintFactory(PROBLEME_HEBDO* problemeHebdo) :
     problemeHebdo(problemeHebdo),
     builder(*problemeHebdo, problemeHebdo->CorrespondanceVarNativesVarOptim)
    {
    }

    PROBLEME_HEBDO* problemeHebdo;
    ConstraintBuilder builder;
};

// Helper functions
inline void exportPaliers(const PROBLEME_HEBDO& problemeHebdo,
                          ConstraintBuilder& constraintBuilder,
                          int pays)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo.PaliersThermiquesDuPays[pays];

    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        constraintBuilder.DispatchableProduction(palier, -1.0);
    }
}
