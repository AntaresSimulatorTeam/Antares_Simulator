#pragma once

#include "../opt_structure_probleme_a_resoudre.h"
#include "../../simulation/sim_structure_probleme_economique.h"

namespace VariableManagement
{

/*!
Factory class that hold variables indices
*/
class VariableManager
{
public:
    VariableManager(CORRESPONDANCES_DES_VARIABLES& nativeOptimVar,
                    std::vector<int>& NumeroDeVariableStockFinal,
                    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock) :
     nativeOptimVar_(nativeOptimVar),
     NumeroDeVariableStockFinal(NumeroDeVariableStockFinal),
     NumeroDeVariableDeTrancheDeStock(NumeroDeVariableDeTrancheDeStock)
    {
    }

    int& DispatchableProduction(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableDuPalierThermique[index];
    }

    int& NumberOfDispatchableUnits(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index];
    }

    int& NumberStoppingDispatchableUnits(unsigned int index)
    {
        return nativeOptimVar_
          .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index];
    }

    int& NumberStartingDispatchableUnits(unsigned int index)
    {
        return nativeOptimVar_
          .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index];
    }

    int& NumberBreakingDownDispatchableUnits(unsigned int index)
    {
        return nativeOptimVar_
          .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index];
    }

    int& NTCDirect(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableDeLInterconnexion[index];
    }

    int& IntercoDirectCost(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index];
    }

    int& IntercoIndirectCost(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index];
    }

    int& ShortTermStorageInjection(unsigned int index)
    {
        return nativeOptimVar_.SIM_ShortTermStorage.InjectionVariable[index];
    }

    int& ShortTermStorageWithdrawal(unsigned int index)
    {
        return nativeOptimVar_.SIM_ShortTermStorage.WithdrawalVariable[index];
    }

    int& ShortTermStorageLevel(unsigned int index)
    {
        return nativeOptimVar_.SIM_ShortTermStorage.LevelVariable[index];
    }

    int& HydProd(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariablesDeLaProdHyd[index];
    }

    int& HydProdDown(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariablesVariationHydALaBaisse[index];
    }

    int& HydProdUp(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariablesVariationHydALaHausse[index];
    }

    int& Pumping(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariablesDePompage[index];
    }

    int& HydroLevel(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariablesDeNiveau[index];
    }

    int& Overflow(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariablesDeDebordement[index];
    }

    int& FinalStorage(unsigned int index)
    {
        return NumeroDeVariableStockFinal[index];
    }

    int& LayerStorage(unsigned area, unsigned layer)
    {
        return NumeroDeVariableDeTrancheDeStock[area][layer];
    }

    int& PositiveUnsuppliedEnergy(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableDefaillancePositive[index];
    }

    int& NegativeUnsuppliedEnergy(unsigned int index)
    {
        return nativeOptimVar_.NumeroDeVariableDefaillanceNegative[index];
    }

private:
    CORRESPONDANCES_DES_VARIABLES& nativeOptimVar_;
    std::vector<int>& NumeroDeVariableStockFinal;
    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
};

class VariableManagerFactory
{
public:
    VariableManagerFactory(
      std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
      std::vector<int>& NumeroDeVariableStockFinal,
      std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
      const int32_t& NombreDePasDeTempsPourUneOptimisation);

    /*!
     * @brief
     * @param offset: offset from the current time step
     * @param delta: number of time steps for the variable
     * @return VariableManager object
     */
    VariableManager GetVariableManager(unsigned int hourInWeek, int offset, int delta);
    /*!
     * @brief
     * @return VariableManager object
     */
    VariableManager GetVariableManager(unsigned int hourInWeek)
    {
        return GetVariableManager(hourInWeek, 0, 0);
    }

private:
    std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim_;
    std::vector<int>& NumeroDeVariableStockFinal_;
    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock_;
    const int32_t& NombreDePasDeTempsPourUneOptimisation_;
    unsigned int hourInWeek_ = 0;
    int GetShiftedTimeStep(int offset, int delta) const;
};
} // namespace VariableManagement