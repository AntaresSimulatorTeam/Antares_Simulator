#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace VariableManagement
{

/*!
Factory class that hold variables indices
*/
class VariableManager
{
public:
    VariableManager(std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
                    std::vector<int>& NumeroDeVariableStockFinal,
                    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
                    const int32_t& NombreDePasDeTempsPourUneOptimisation);

    int& DispatchableProduction(unsigned int index,
                                unsigned int hourInWeek,
                                int offset = 0,
                                int delta = 0);

    int& NumberOfDispatchableUnits(unsigned int index,
                                   unsigned int hourInWeek,
                                   int offset = 0,
                                   int delta = 0);

    int& NumberStoppingDispatchableUnits(unsigned int index,
                                         unsigned int hourInWeek,
                                         int offset = 0,
                                         int delta = 0);
    int& NumberStartingDispatchableUnits(unsigned int index,
                                         unsigned int hourInWeek,
                                         int offset = 0,
                                         int delta = 0);

    int& NumberBreakingDownDispatchableUnits(unsigned int index,
                                             unsigned int hourInWeek,
                                             int offset = 0,
                                             int delta = 0);

    int& NTCDirect(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& IntercoDirectCost(unsigned int index,
                           unsigned int hourInWeek,
                           int offset = 0,
                           int delta = 0);

    int& IntercoIndirectCost(unsigned int index,
                             unsigned int hourInWeek,
                             int offset = 0,
                             int delta = 0);

    int& ShortTermStorageInjection(unsigned int index,
                                   unsigned int hourInWeek,
                                   int offset = 0,
                                   int delta = 0);

    int& ShortTermStorageWithdrawal(unsigned int index,
                                    unsigned int hourInWeek,
                                    int offset = 0,
                                    int delta = 0);

    int& ShortTermStorageLevel(unsigned int index,
                               unsigned int hourInWeek,
                               int offset = 0,
                               int delta = 0);
    int& ShortTermStorageCostVariationInjection(unsigned int index,
                                                unsigned int hourInWeek,
                                                int offset = 0,
                                                int delta = 0);
    int& ShortTermStorageCostVariationWithdrawal(unsigned int index,
                                                 unsigned int hourInWeek,
                                                 int offset = 0,
                                                 int delta = 0);

    int& HydProd(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& HydProdDown(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& HydProdUp(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& Pumping(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& HydroLevel(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& Overflow(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& FinalStorage(unsigned int index);

    int& LayerStorage(unsigned area, unsigned layer);

    int& PositiveUnsuppliedEnergy(unsigned int index,
                                  unsigned int hourInWeek,
                                  int offset = 0,
                                  int delta = 0);

    int& NegativeUnsuppliedEnergy(unsigned int index,
                                  unsigned int hourInWeek,
                                  int offset = 0,
                                  int delta = 0);

private:
    std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim_;
    std::vector<int>& NumeroDeVariableStockFinal_;
    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock_;
    const int32_t& NombreDePasDeTempsPourUneOptimisation_;
    int GetShiftedTimeStep(int offset, int delta, unsigned int hourInWeek) const;
};

} // namespace VariableManagement
