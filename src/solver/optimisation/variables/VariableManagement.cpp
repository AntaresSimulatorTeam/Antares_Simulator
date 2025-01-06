#include "VariableManagement.h"

using namespace VariableManagement;

VariableManager::VariableManager(
  std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
  std::vector<int>& NumeroDeVariableStockFinal,
  std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
  const int32_t& NombreDePasDeTempsPourUneOptimisation):
    CorrespondanceVarNativesVarOptim_(CorrespondanceVarNativesVarOptim),
    NumeroDeVariableStockFinal_(NumeroDeVariableStockFinal),
    NumeroDeVariableDeTrancheDeStock_(NumeroDeVariableDeTrancheDeStock),
    NombreDePasDeTempsPourUneOptimisation_(NombreDePasDeTempsPourUneOptimisation)
{
}

int VariableManager::GetShiftedTimeStep(int offset, int delta, unsigned int hourInWeek) const
{
    int pdt = hourInWeek + offset;
    const int nbTimeSteps = NombreDePasDeTempsPourUneOptimisation_;

    if (offset != 0)
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

int& VariableManager::DispatchableProduction(unsigned int index,
                                             unsigned int hourInWeek,
                                             int offset,
                                             int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);

    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDuPalierThermique[index];
}

int& VariableManager::NumberOfDispatchableUnits(unsigned int index,
                                                unsigned int hourInWeek,
                                                int offset,
                                                int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index];
}

int& VariableManager::NumberStoppingDispatchableUnits(unsigned int index,
                                                      unsigned int hourInWeek,
                                                      int offset,
                                                      int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index];
}

int& VariableManager::NumberStartingDispatchableUnits(unsigned int index,
                                                      unsigned int hourInWeek,
                                                      int offset,
                                                      int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index];
}

int& VariableManager::NumberBreakingDownDispatchableUnits(unsigned int index,
                                                          unsigned int hourInWeek,
                                                          int offset,
                                                          int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index];
}

int& VariableManager::NTCDirect(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDeLInterconnexion[index];
}

int& VariableManager::IntercoDirectCost(unsigned int index,
                                        unsigned int hourInWeek,
                                        int offset,
                                        int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index];
}

int& VariableManager::IntercoIndirectCost(unsigned int index,
                                          unsigned int hourInWeek,
                                          int offset,
                                          int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index];
}

int& VariableManager::ShortTermStorageInjection(unsigned int index,
                                                unsigned int hourInWeek,
                                                int offset,
                                                int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.InjectionVariable[index];
}

int& VariableManager::ShortTermStorageWithdrawal(unsigned int index,
                                                 unsigned int hourInWeek,
                                                 int offset,
                                                 int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.WithdrawalVariable[index];
}

int& VariableManager::ShortTermStorageLevel(unsigned int index,
                                            unsigned int hourInWeek,
                                            int offset,
                                            int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.LevelVariable[index];
}

int& VariableManager::ShortTermStorageCostVariationInjection(unsigned int index,
                                                             unsigned int hourInWeek,
                                                             int offset,
                                                             int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .SIM_ShortTermStorage.CostVariationInjection[index];
}

int& VariableManager::ShortTermStorageCostVariationWithdrawal(unsigned int index,
                                                              unsigned int hourInWeek,
                                                              int offset,
                                                              int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .SIM_ShortTermStorage.CostVariationWithdrawal[index];
}

int& VariableManager::HydProd(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDeLaProdHyd[index];
}

int& VariableManager::HydProdDown(unsigned int index,
                                  unsigned int hourInWeek,
                                  int offset,
                                  int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesVariationHydALaBaisse[index];
}

int& VariableManager::HydProdUp(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesVariationHydALaHausse[index];
}

int& VariableManager::Pumping(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDePompage[index];
}

int& VariableManager::HydroLevel(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDeNiveau[index];
}

int& VariableManager::Overflow(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDeDebordement[index];
}

int& VariableManager::FinalStorage(unsigned int index)
{
    return NumeroDeVariableStockFinal_[index];
}

int& VariableManager::LayerStorage(unsigned area, unsigned layer)
{
    return NumeroDeVariableDeTrancheDeStock_[area][layer];
}

int& VariableManager::PositiveUnsuppliedEnergy(unsigned int index,
                                               unsigned int hourInWeek,
                                               int offset,
                                               int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDefaillancePositive[index];
}

int& VariableManager::NegativeUnsuppliedEnergy(unsigned int index,
                                               unsigned int hourInWeek,
                                               int offset,
                                               int delta)
{
    auto pdt = GetShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDefaillanceNegative[index];
}
