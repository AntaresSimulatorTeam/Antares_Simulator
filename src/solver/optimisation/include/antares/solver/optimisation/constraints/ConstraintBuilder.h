// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/optimisation/variables/VariableManagement.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "../opt_fonctions.h"
#include "../opt_rename_problem.h"

// TODO God struct should be decomposed
class ConstraintBuilderData final
{
public:
    std::vector<double>& Pi;
    std::vector<int>& Colonne;
    int& nombreDeContraintes;
    int& nombreDeTermesDansLaMatriceDeContrainte;
    std::vector<int>& IndicesDebutDeLigne;
    std::vector<double>& CoefficientsDeLaMatriceDesContraintes;
    std::vector<int>& IndicesColonnes;
    std::vector<int>& NombreDeTermesDesLignes;
    std::string& Sens;
    int& IncrementDAllocationMatriceDesContraintes;
    std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim;
    const int32_t& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeVariableStockFinal;
    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
    std::vector<std::string>& NomDesContraintes;
    const std::vector<const char*>& NomsDesPays;
    const uint32_t& weekInTheYear;
    const uint32_t& NombreDePasDeTemps;
    std::vector<std::optional<Antares::Optimization::LegacyVariableInfo>>& LegacyConstraintsInfo;
};

/*! \verbatim
this class build up the business object 'Constraint',
Math:
|coeff11 coeff12 .. coeff1n||var1| |sign_1|   |rhs1|       |constraint1||sign_1||rhs1|
|..      ..             ...||....| |......|   |....| <===> |...........||......||....|
|coeffn1 coeffn2 .. coeffnn||varn| |sign_n|   |rhsn|       |constraintn||sign_n||rhsn|

it propose a set of methods  to attach 'Variables' to the Constraint
ex: calling DirectFlow() implies adding Direct Flux Variable to the current Constraint
finally the build() method gather all variables and put them into the matrix
\endverbatim
*/
class ConstraintBuilder final
{
public:
    ConstraintBuilder() = delete;

    explicit ConstraintBuilder(ConstraintBuilderData& data):
        data(data),
        variableManager_(data.CorrespondanceVarNativesVarOptim,
                         data.NumeroDeVariableStockFinal,
                         data.NumeroDeVariableDeTrancheDeStock,
                         data.NombreDePasDeTempsPourUneOptimisation)
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

    ConstraintBuilder& RunningThermalClusterReserveParticipation(unsigned int index,
                                                                 double coeff,
                                                                 int offset = 0,
                                                                 int delta = 0);

    ConstraintBuilder& OffThermalClusterReserveParticipation(unsigned int index,
                                                             double coeff,
                                                             int offset = 0,
                                                             int delta = 0);

    ConstraintBuilder& ThermalClusterReserveParticipation(unsigned int index,
                                                          double coeff,
                                                          int offset = 0,
                                                          int delta = 0);

    ConstraintBuilder& STStorageClusterReserveParticipation(ReserveType type,
                                                            unsigned int index,
                                                            double coeff,
                                                            int offset = 0,
                                                            int delta = 0);

    ConstraintBuilder& STStorageReleaseClusterReserveParticipation(unsigned int index,
                                                                   double coeff,
                                                                   int offset = 0,
                                                                   int delta = 0);

    ConstraintBuilder& STStorageStoreClusterReserveParticipation(unsigned int index,
                                                                 double coeff,
                                                                 int offset = 0,
                                                                 int delta = 0);

    ConstraintBuilder& HydroReserveParticipation(ReserveType type,
                                                 unsigned int index,
                                                 double coeff,
                                                 int offset = 0,
                                                 int delta = 0);

    ConstraintBuilder& HydroReleaseReserveParticipation(unsigned int index,
                                                        double coeff,
                                                        int offset = 0,
                                                        int delta = 0);

    ConstraintBuilder& HydroStoreReserveParticipation(unsigned int index,
                                                      double coeff,
                                                      int offset = 0,
                                                      int delta = 0);

    ConstraintBuilder& InternalUnsatisfiedReserve(unsigned int pays,
                                                  double coeff,
                                                  int offset = 0,
                                                  int delta = 0);

    ConstraintBuilder& InternalExcessReserve(unsigned int pays,
                                             double coeff,
                                             int offset = 0,
                                             int delta = 0);

    ConstraintBuilder& NumberOfDispatchableUnits(unsigned int index, double coeff);

    ConstraintBuilder& NumberStoppingDispatchableUnits(unsigned int index, double coeff);

    ConstraintBuilder& NumberStartingDispatchableUnits(unsigned int index, double coeff);

    ConstraintBuilder& NumberBreakingDownDispatchableUnits(unsigned int index, double coeff);

    ConstraintBuilder& DirectFlow(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& PositiveDirectFlow(unsigned int index, double coeff);

    ConstraintBuilder& PositiveIndirectFlow(unsigned int index, double coeff);

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

    ConstraintBuilder& ShortTermStorageOverflow(unsigned int index,
                                                double coeff,
                                                int offset = 0,
                                                int delta = 0);

    ConstraintBuilder& ShortTermCostVariationInjection(unsigned int index,
                                                       double coeff,
                                                       int offset = 0,
                                                       int delta = 0);
    ConstraintBuilder& ShortTermCostVariationWithdrawal(unsigned int index,
                                                        double coeff,
                                                        int offset = 0,
                                                        int delta = 0);

    ConstraintBuilder& HydProd(unsigned int index, double coeff);

    ConstraintBuilder& HydProdDown(unsigned int index, double coeff);

    ConstraintBuilder& HydProdUp(unsigned int index, double coeff);

    ConstraintBuilder& Pumping(unsigned int index, double coeff);

    ConstraintBuilder& HydroLevel(unsigned int index, double coeff);

    ConstraintBuilder& Overflow(unsigned int index, double coeff);

    ConstraintBuilder& FinalStorage(unsigned int index, double coeff);

    ConstraintBuilder& UnsuppliedEnergy(unsigned int index, double coeff);

    ConstraintBuilder& Spillage(unsigned int index, double coeff);

    ConstraintBuilder& LayerStorage(unsigned area, unsigned layer, double coeff);

    //@}

    class ConstraintBuilderInvalidOperator final: public std::runtime_error
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
        {
            throw ConstraintBuilderInvalidOperator(std::string("Invalid operator: ") + op);
        }

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

    ConstraintBuilderData& data;

private:
    void OPT_ChargerLaContrainteDansLaMatriceDesContraintes();

    unsigned int hourInWeek_ = 0;

    char operator_ = '=';
    int nombreDeTermes_ = 0;

    void AddVariable(int index, double coeff);

    /*!
     * @brief
     * @param offset: offset from the current time step
     * @param delta: number of time steps for the variable
     * @return VariableManager object
     */
    VariableManagement::VariableManager variableManager_;
};

/*! factory class to build a Constraint */
class ConstraintFactory
{
public:
    ConstraintFactory() = delete;

    explicit ConstraintFactory(ConstraintBuilder& builder):
        builder(builder)
    {
    }

    ConstraintBuilder& builder;
};

// Helper functions
inline void ExportPaliers(const PALIERS_THERMIQUES& PaliersThermiquesDuPays,
                          ConstraintBuilder& newConstraintBuilder)
{
    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier = PaliersThermiquesDuPays
                             .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        newConstraintBuilder.DispatchableProduction(palier, -1.0);
    }
}

class BindingConstraintData final
{
public:
    const char& TypeDeContrainteCouplante;
    const int& NombreDInterconnexionsDansLaContrainteCouplante;
    const std::vector<int>& NumeroDeLInterconnexion;
    const std::vector<double>& PoidsDeLInterconnexion;
    const std::vector<int>& OffsetTemporelSurLInterco;
    const int& NombreDePaliersDispatchDansLaContrainteCouplante;
    const std::vector<int>& PaysDuPalierDispatch;
    const std::vector<int>& NumeroDuPalierDispatch;
    const std::vector<double>& PoidsDuPalierDispatch;
    const std::vector<int>& OffsetTemporelSurLePalierDispatch;
    const char& SensDeLaContrainteCouplante;
    const char* const& NomDeLaContrainteCouplante;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
};

struct StartUpCostsData
{
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    bool Simulation;
};

struct ShortTermStorageData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;

    const std::vector<::AREA_INPUT>& ShortTermStorage;
};

struct ShortTermStorageCumulativeConstraintData: ShortTermStorageData
{
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires;
};

using namespace Antares::Data;

struct ReserveData
{
    bool Simulation;
    std::vector<::AREA_RESERVES_VECTOR>& areaReserves;
    std::vector<PALIERS_THERMIQUES>& thermalClusters;
    std::vector<::AREA_INPUT>& shortTermStorageOfArea;
    std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES>& hydroOfArea;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;

    int countNumberOfConstraintsForThermalReserves(int pays, int cluster)
    {
        int count = 0;
        for (auto type: {ReserveType::UP, ReserveType::DOWN})
        {
            if (std::ranges::any_of(areaReserves[pays].areaCapacityReservations | filter(type),
                                    [&](const auto& r) {
                                        return r.AllThermalReservesParticipation.count(cluster) > 0;
                                    }))
            {
                count++;
            }
        }
        return count;
    }

    int countNumberOfConstraintsForHydroReserves(int pays,
                                                 bool accountForGlobalActivationDuration = false)
    {
        int count = 0;
        for (auto type: {ReserveType::UP, ReserveType::DOWN})
        {
            if (!accountForGlobalActivationDuration
                || areaReserves[pays].referenceGlobalActivationDuration[type])
            {
                if (std::ranges::any_of(areaReserves[pays].areaCapacityReservations | filter(type),
                                        [&](const auto& r)
                                        { return r.AllHydroReservesParticipation.size() > 0; }))
                {
                    count++;
                }
            }
        }

        return count;
    }

    int countNumberOfConstraintsForSTStorageReserves(
      int pays,
      int cluster,
      bool accountForGlobalActivationDuration = false)
    {
        int count = 0;
        for (auto type: {ReserveType::UP, ReserveType::DOWN})
        {
            if (!accountForGlobalActivationDuration
                || areaReserves[pays].referenceGlobalActivationDuration[type])
            {
                if (std::ranges::any_of(areaReserves[pays].areaCapacityReservations | filter(type),
                                        [&](const auto& r) {
                                            return r.AllSTStorageReservesParticipation.count(
                                                     cluster)
                                                   > 0;
                                        }))
                {
                    count++;
                }
            }
        }
        return count;
    }

    static void addRunningThermalClusterReserveParticipationToBuilder(ReserveData& data,
                                                                      int pays,
                                                                      int cluster,
                                                                      ReserveType type,
                                                                      ConstraintBuilder& builder)
    {
        for (const auto& capacityReservation:
             data.areaReserves[pays].areaCapacityReservations | filter(type))
        {
            if (capacityReservation.AllThermalReservesParticipation.contains(cluster))
            {
                auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation.at(
                  cluster);
                builder.RunningThermalClusterReserveParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }
        }
    }
};
