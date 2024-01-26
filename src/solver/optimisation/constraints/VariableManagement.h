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
    VariableManager(const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar,
                    const std::vector<int>& NumeroDeVariableStockFinal,
                    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock) :
     nativeOptimVar_(nativeOptimVar),
     NumeroDeVariableStockFinal(NumeroDeVariableStockFinal),
     NumeroDeVariableDeTrancheDeStock(NumeroDeVariableDeTrancheDeStock)
    {
    }

    int DispatchableProduction(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariableDuPalierThermique[index];
    }

    int NumberOfDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index];
    }

    int NumberStoppingDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar_
          .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index];
    }

    int NumberStartingDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar_
          .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index];
    }

    int NumberBreakingDownDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar_
          .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index];
    }

    int NTCDirect(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariableDeLInterconnexion[index];
    }

    int IntercoDirectCost(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index];
    }

    int IntercoIndirectCost(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index];
    }

    int ShortTermStorageInjection(unsigned int index) const
    {
        return nativeOptimVar_.SIM_ShortTermStorage.InjectionVariable[index];
    }

    int ShortTermStorageWithdrawal(unsigned int index) const
    {
        return nativeOptimVar_.SIM_ShortTermStorage.WithdrawalVariable[index];
    }

    int ShortTermStorageLevel(unsigned int index) const
    {
        return nativeOptimVar_.SIM_ShortTermStorage.LevelVariable[index];
    }

    int HydProd(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariablesDeLaProdHyd[index];
    }

    int HydProdDown(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariablesVariationHydALaBaisse[index];
    }

    int HydProdUp(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariablesVariationHydALaHausse[index];
    }

    int Pumping(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariablesDePompage[index];
    }

    int HydroLevel(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariablesDeNiveau[index];
    }

    int Overflow(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariablesDeDebordement[index];
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
        return nativeOptimVar_.NumeroDeVariableDefaillancePositive[index];
    }

    int NegativeUnsuppliedEnergy(unsigned int index) const
    {
        return nativeOptimVar_.NumeroDeVariableDefaillanceNegative[index];
    }

private:
    const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar_;
    const std::vector<int>& NumeroDeVariableStockFinal;
    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
};

class VariableManagerFactory
{
public:
    VariableManagerFactory(
      const std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
      const std::vector<int>& NumeroDeVariableStockFinal,
      std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
      const int32_t& NombreDePasDeTempsPourUneOptimisation);

    int GetShiftedTimeStep(int offset, int delta) const;

    VariableManager GetVariableManager(unsigned int hourInWeek, int offset, int delta);

private:
    const std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim_;
    const std::vector<int>& NumeroDeVariableStockFinal_;
    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock_;
    const int32_t& NombreDePasDeTempsPourUneOptimisation_;
    unsigned int hourInWeek_ = 0;
};
} // namespace VariableManagement