#include "VariableManagement.h"

using namespace VariableManagement;

VariableManagerFactory::VariableManagerFactory(
  const std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
  const std::vector<int>& NumeroDeVariableStockFinal,
  const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
  const int32_t& NombreDePasDeTempsPourUneOptimisation) :
 CorrespondanceVarNativesVarOptim_(CorrespondanceVarNativesVarOptim),
 NumeroDeVariableStockFinal_(NumeroDeVariableStockFinal),
 NumeroDeVariableDeTrancheDeStock_(NumeroDeVariableDeTrancheDeStock),
 NombreDePasDeTempsPourUneOptimisation_(NombreDePasDeTempsPourUneOptimisation)
{
}

int VariableManagerFactory::GetShiftedTimeStep(int offset, int delta) const
{
    int pdt = hourInWeek_ + offset;
    const int nbTimeSteps = NombreDePasDeTempsPourUneOptimisation_;

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

VariableManager VariableManagerFactory::GetVariableManager(unsigned int hourInWeek,
                                                           int offset,
                                                           int delta)
{
    hourInWeek_ = hourInWeek;
    auto pdt = GetShiftedTimeStep(offset, delta);
    return VariableManagement::VariableManager(CorrespondanceVarNativesVarOptim_[pdt],
                                               NumeroDeVariableStockFinal_,
                                               NumeroDeVariableDeTrancheDeStock_);
}
