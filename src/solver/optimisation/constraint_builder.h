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
    ConstraintBuilder(PROBLEME_HEBDO& problemeHebdo,
                      std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim) :
      problemeHebdo(problemeHebdo),
      problemeAResoudre(*problemeHebdo.ProblemeAResoudre),
      varNative(CorrespondanceVarNativesVarOptim)
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
      if (pdt < 0 || pdt >= 168)
      {
          return -1;
      }

      const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar = varNative[pdt];
      switch (variable)
      {
      case Variable::DispatchableProduction:
        return nativeOptimVar.NumeroDeVariableDuPalierThermique[index_];
      case Variable::NODU:
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index_];
      case Variable::NumberStoppingDispatchableUnits:
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index_];
      case Variable::NumberStartingDispatchableUnits:
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index_];
      case Variable::NumberBreakingDownDispatchableUnits:
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index_];
      case Variable::NTCDirect:
        return nativeOptimVar.NumeroDeVariableDeLInterconnexion[index_];
      case Variable::IntercoDirectCost:
        return nativeOptimVar.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index_];
      case Variable::IntercoIndirectCost:
        return nativeOptimVar.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index_];
      case Variable::ShortTermStorageInjection:
        return nativeOptimVar.SIM_ShortTermStorage.InjectionVariable[index_];
      case Variable::ShortTermStorageWithdrawal:
        return nativeOptimVar.SIM_ShortTermStorage.WithdrawalVariable[index_];
      case Variable::ShortTermStorageLevel:
        return nativeOptimVar.SIM_ShortTermStorage.LevelVariable[index_];
      case Variable::HydProd:
        return nativeOptimVar.NumeroDeVariablesDeLaProdHyd[index_];
      case Variable::HydProdDown:
        return nativeOptimVar.NumeroDeVariablesVariationHydALaBaisse[index_];
      case Variable::HydProdUp:
        return nativeOptimVar.NumeroDeVariablesVariationHydALaHausse[index_];
      case Variable::Pumping:
        return nativeOptimVar.NumeroDeVariablesDePompage[index_];
      case Variable::HydroLevel:
        return nativeOptimVar.NumeroDeVariablesDeNiveau[index_];
      case Variable::Overflow:
        return nativeOptimVar.NumeroDeVariablesDeDebordement[index_];
      case Variable::FinalStorage:
        return problemeHebdo.NumeroDeVariableStockFinal[index_];
      case Variable::LayerStorage:
        return problemeHebdo.NumeroDeVariableDeTrancheDeStock[index_][0]; // TODO FIXMEEE
      case Variable::PositiveUnsuppliedEnergy:
        return nativeOptimVar.NumeroDeVariableDefaillancePositive[index_];
      case Variable::NegativeUnsuppliedEnergy:
        return nativeOptimVar.NumeroDeVariableDefaillanceNegative[index_];
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

    void build(Constraint cnt);

private:
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre;
    PROBLEME_HEBDO& problemeHebdo;
    std::vector<CORRESPONDANCES_DES_VARIABLES>& varNative;

    unsigned int hourInWeek_ = 0;
    // TODO: vérifier que l'index correspond bien à l'objet concerné (zone, lien, cluster thermique, etc.)
    unsigned int index_ = 0;
    char operator_;

    int nombreDeTermes_ = 0;
};
