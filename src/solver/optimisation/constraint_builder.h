#pragma once
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_fonctions.h"

// TODO namespace
enum class Variable {
  DispatchableProduction,
  NODU,
  NumberStoppingDispatchableUnits,
  NumberStartingDispatchableUnits,
  NumberBreakingDownDispatchableUnits,
  NTCDirect,
  IntercoDirectCost,
  IntercoIndirectCost,
  ShortTermStorageInjection,
  ShortTermStorageWithdrawal,
  ShortTermStorageLevel,
  HydProd,
  HydProdDown,
  HydProdUp,
  Pumping,
  HydroLevel,
  Overflow,
  FinalStorage,
  LayerStorage,
  PositiveUnsuppliedEnergy,
  NegativeUnsuppliedEnergy,
};

enum class Constraint {
  FlowDissociation,
  AreaBalance,
  FictiveLoads,
  HydroPower,
  HydroPowerSmoothingUsingVariationSum,
  HydroPowerSmoothingUsingVariationMaxDown,
  HydroPowerSmoothingUsingVariationMaxUp,
  MinHydroPower,
  MaxHydroPower,
  MaxPumping,
  AreaHydroLevel,
  FinalStockEquivalent,
  FinalStockExpression,
  NbUnitsOutageLessThanNbUnitsStop,
  NbDispUnitsMinBoundSinceMinUpTime,
  MinDownTime,
  PMaxDispatchableGeneration,
  PMinDispatchableGeneration,
  ConsistenceNODU,
  ShortTermStorageLevel,
  BindingConstraintHour,
  BindingConstraintDay,
  BindingConstraintWeek
};

class ConstraintBuilder
{
public:
    ConstraintBuilder(PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre,
                      std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
                      std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim) :
      problemeAResoudre(ProblemeAResoudre),
      varNative(CorrespondanceVarNativesVarOptim),
      cntNative(CorrespondanceCntNativesCntOptim)
    {
    }

    void updateHourWithinWeek(unsigned hour)
    {
        hourInWeek_ = hour;
    }

    void updateIndex(unsigned index)
    {
        index_ = index;
    }

    int getVariableIndex(Variable variable, int timeShift) const
    {
      int pdt = hourInWeek_ + timeShift;
      // TODO remove 168
      if (pdt < 0 || pdt > 168)
      {
          return -1;
      }

      CORRESPONDANCES_DES_VARIABLES& nativeOptimVar = varNative[pdt];
      switch (variable)
      {
      case Variable::DispatchableProduction:
        return nativeOptimVar.NumeroDeVariableDuPalierThermique[index_];
      case Variable::NODU:
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index_];
      // TODO rest of cases
      // case NumberStoppingDispatchableUnits:
      // case NumberStartingDispatchableUnits:
      // case NumberBreakingDownDispatchableUnits:
      // case NTCDirect:
      // case IntercoDirectCost:
      // case IntercoIndirectCost:
      // case ShortTermStorageInjection:
      // case ShortTermStorageWithdrawal:
      // case ShortTermStorageLevel:
      // case HydProd:
      // case HydProdDown:
      // case HydProdUp:
      // case Pumping:
      // case HydroLevel:
      // case Overflow:
      // case FinalStorage:
      // case LayerStorage:
      // case PositiveUnsuppliedEnergy:
      // case NegativeUnsuppliedEnergy:
      default:
        return -1;
    }
    }

    ConstraintBuilder& include(Variable var, double coeff, int timeShift = 0)
    {
      std::vector<double>& Pi = problemeAResoudre.Pi;
      std::vector<int>& Colonne = problemeAResoudre.Colonne;
      int varIndex = getVariableIndex(var, timeShift);
      if (varIndex >= 0)
      {
        Pi[nombreDeTermes_] = coeff;
        Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
      }
      return *this;
    }
    ConstraintBuilder& equal(double)
    {
      operator_ = '=';
      return *this;
    }
    ConstraintBuilder& less(double)
    {
      operator_ = '<';
      return *this;
    }
    ConstraintBuilder& greater(double)
    {
      operator_ = '>';
      return *this;
    }

    [[nodiscard]] int build();

private:
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre;
    std::vector<CORRESPONDANCES_DES_VARIABLES>& varNative;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& cntNative;

    unsigned int hourInWeek_ = 0;
    // TODO: vérifier que l'index correspond bien à l'objet concerné (zone, lien, cluster thermique, etc.)
    unsigned int index_ = 0;
    char operator_;

    int nombreDeTermes_ = 0;
};
