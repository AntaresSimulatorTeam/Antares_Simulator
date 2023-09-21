#include "ConstraintBuilder.h"

void ConstraintBuilder::build()
{
    std::vector<double>& Pi = problemeAResoudre.Pi;
    std::vector<int>& Colonne = problemeAResoudre.Colonne;
    // TODO check operator_
    if (nombreDeTermes_ > 0)
    {
                // Matrix
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          &problemeAResoudre, Pi, Colonne, nombreDeTermes_, operator_);
    }
    nombreDeTermes_ = 0;
}

int ConstraintBuilder::GetShiftedTimeStep(int shift, bool wrap, int delta) const
{
    int pdt = hourInWeek_ + shift;
    const int nbTimeSteps = problemeHebdo.NombreDePasDeTempsPourUneOptimisation;

    if (wrap)
    {
        if (shift >= 0)
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
Variable::VariableManager ConstraintBuilder::GetVariableManager(int shift, bool wrap, int delta)
{
    auto pdt = GetShiftedTimeStep(shift, wrap, delta);
    return Variable::VariableManager(varNative[pdt],
                                     problemeHebdo.NumeroDeVariableStockFinal,
                                     problemeHebdo.NumeroDeVariableDeTrancheDeStock);
}

ConstraintBuilder& ConstraintBuilder::DispatchableProduction(unsigned int index,
                                                             double coeff,
                                                             int shift,
                                                             bool wrap,
                                                             int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).DispatchableProduction(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NODU(unsigned int index,
                                           double coeff,
                                           int shift,
                                           bool wrap,
                                           int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).NODU(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStoppingDispatchableUnits(unsigned int index,
                                                                      double coeff,
                                                                      int shift,
                                                                      bool wrap,
                                                                      int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).NumberStoppingDispatchableUnits(index),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberStartingDispatchableUnits(unsigned int index,
                                                                      double coeff,
                                                                      int shift,
                                                                      bool wrap,
                                                                      int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).NumberStartingDispatchableUnits(index),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NumberBreakingDownDispatchableUnits(unsigned int index,
                                                                          double coeff,
                                                                          int shift,
                                                                          bool wrap,
                                                                          int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).NumberBreakingDownDispatchableUnits(index),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NTCDirect(unsigned int index,
                                                double coeff,
                                                int shift,
                                                bool wrap,
                                                int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).NTCDirect(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoDirectCost(unsigned int index,
                                                        double coeff,
                                                        int shift,
                                                        bool wrap,
                                                        int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).IntercoDirectCost(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::IntercoIndirectCost(unsigned int index,
                                                        double coeff,
                                                        int shift,
                                                        bool wrap,
                                                        int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).IntercoIndirectCost(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageInjection(unsigned int index,
                                                                double coeff,
                                                                int shift,
                                                                bool wrap,
                                                                int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).ShortTermStorageInjection(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageWithdrawal(unsigned int index,
                                                                 double coeff,
                                                                 int shift,
                                                                 bool wrap,
                                                                 int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).ShortTermStorageWithdrawal(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ShortTermStorageLevel(unsigned int index,
                                                            double coeff,
                                                            int shift,
                                                            bool wrap,
                                                            int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).ShortTermStorageLevel(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProd(unsigned int index,
                                              double coeff,
                                              int shift,
                                              bool wrap,
                                              int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).HydProd(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdDown(unsigned int index,
                                                  double coeff,
                                                  int shift,
                                                  bool wrap,
                                                  int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).HydProdDown(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydProdUp(unsigned int index,
                                                double coeff,
                                                int shift,
                                                bool wrap,
                                                int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).HydProdUp(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Pumping(unsigned int index,
                                              double coeff,
                                              int shift,
                                              bool wrap,
                                              int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).Pumping(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::HydroLevel(unsigned int index,
                                                 double coeff,
                                                 int shift,
                                                 bool wrap,
                                                 int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).HydroLevel(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::Overflow(unsigned int index,
                                               double coeff,
                                               int shift,
                                               bool wrap,
                                               int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).Overflow(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::FinalStorage(unsigned int index,
                                                   double coeff,
                                                   int shift,
                                                   bool wrap,
                                                   int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).FinalStorage(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::PositiveUnsuppliedEnergy(unsigned int index,
                                                               double coeff,
                                                               int shift,
                                                               bool wrap,
                                                               int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).PositiveUnsuppliedEnergy(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::NegativeUnsuppliedEnergy(unsigned int index,
                                                               double coeff,
                                                               int shift,
                                                               bool wrap,
                                                               int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).NegativeUnsuppliedEnergy(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::LayerStorage(unsigned area,
                                                   unsigned layer,
                                                   double coeff,
                                                   int shift,
                                                   bool wrap,
                                                   int delta)
{
    AddVariable(GetVariableManager(shift, wrap, delta).LayerStorage(area, layer), coeff);
    return *this;
}
