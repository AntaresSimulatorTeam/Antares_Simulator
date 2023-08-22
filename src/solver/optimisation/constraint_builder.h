#pragma once

#include <antares/logs/logs.h>
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_rename_problem.h"
#include "opt_fonctions.h"

// TODO remove relative include
#include "../simulation/sim_structure_probleme_economique.h"

// TODO namespace
enum class Variable
{
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

// enum class Constraint {
//   FlowDissociation,
//   AreaBalance,
//   FictiveLoads,
//   HydroPower,
//   HydroPowerSmoothingUsingVariationSum,
//   HydroPowerSmoothingUsingVariationMaxDown,
//   HydroPowerSmoothingUsingVariationMaxUp,
//   MinHydroPower,
//   MaxHydroPower,
//   MaxPumping,
//   AreaHydroLevel,
//   FinalStockEquivalent,
//   FinalStockExpression,
//   NbUnitsOutageLessThanNbUnitsStop,
//   NbDispUnitsMinBoundSinceMinUpTime,
//   MinDownTime,
//   PMaxDispatchableGeneration,
//   PMinDispatchableGeneration,
//   ConsistenceNODU,
//   ShortTermStorageLevel,
//   BindingConstraintHour,
//   BindingConstraintDay,
//   BindingConstraintWeek
// };

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

    void updateHourWithinWeek(unsigned hour)
    {
        hourInWeek_ = hour;
    }

    inline ConstraintBuilder& thermalCluster(unsigned index)
    {
        thermalClusterIndex_ = index;
        return *this;
    }

    inline ConstraintBuilder& shortTermStorage(unsigned index)
    {
        shortTermStorageIndex_ = index;
        return *this;
    }

    inline ConstraintBuilder& area(unsigned index)
    {
        areaIndex_ = index;
        return *this;
    }

    inline ConstraintBuilder& link(unsigned index)
    {
        linkIndex_ = index;
        return *this;
    }

    inline ConstraintBuilder& layer(unsigned index)
    {
        layerIndex_ = index;
        return *this;
    }

private:
    int getVariableIndex(Variable variable, int timeShift, bool wrap) const
    {
        int pdt = hourInWeek_ + timeShift;
        // TODO remove 168
        if (wrap)
        {
            pdt %= 168;
            if (pdt < 0)
                pdt += 168;
        }
        // TODO remove 168
        if (pdt < 0 || pdt >= 168)
        {
            return -1;
        }

        const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar = varNative[pdt];
        try
        {
            switch (variable)
            {
            case Variable::DispatchableProduction:
                return nativeOptimVar.NumeroDeVariableDuPalierThermique.at(thermalClusterIndex_);
            case Variable::NODU:
                return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique.at(
                  thermalClusterIndex_);
            case Variable::NumberStoppingDispatchableUnits:
                return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
                  .at(thermalClusterIndex_);
            case Variable::NumberStartingDispatchableUnits:
                return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
                  .at(thermalClusterIndex_);
            case Variable::NumberBreakingDownDispatchableUnits:
                return nativeOptimVar
                  .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique.at(
                    thermalClusterIndex_);
            case Variable::NTCDirect:
                return nativeOptimVar.NumeroDeVariableDeLInterconnexion.at(linkIndex_);
            case Variable::IntercoDirectCost:
                return nativeOptimVar.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion.at(
                  linkIndex_);
            case Variable::IntercoIndirectCost:
                return nativeOptimVar.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion.at(
                  linkIndex_);
            case Variable::ShortTermStorageInjection:
                return nativeOptimVar.SIM_ShortTermStorage.InjectionVariable.at(
                  shortTermStorageIndex_);
            case Variable::ShortTermStorageWithdrawal:
                return nativeOptimVar.SIM_ShortTermStorage.WithdrawalVariable.at(
                  shortTermStorageIndex_);
            case Variable::ShortTermStorageLevel:
                return nativeOptimVar.SIM_ShortTermStorage.LevelVariable.at(shortTermStorageIndex_);
            case Variable::HydProd:
                return nativeOptimVar.NumeroDeVariablesDeLaProdHyd.at(areaIndex_);
            case Variable::HydProdDown:
                return nativeOptimVar.NumeroDeVariablesVariationHydALaBaisse.at(areaIndex_);
            case Variable::HydProdUp:
                return nativeOptimVar.NumeroDeVariablesVariationHydALaHausse.at(areaIndex_);
            case Variable::Pumping:
                return nativeOptimVar.NumeroDeVariablesDePompage.at(areaIndex_);
            case Variable::HydroLevel:
                return nativeOptimVar.NumeroDeVariablesDeNiveau.at(areaIndex_);
            case Variable::Overflow:
                return nativeOptimVar.NumeroDeVariablesDeDebordement.at(areaIndex_);
            case Variable::FinalStorage:
                return problemeHebdo.NumeroDeVariableStockFinal.at(areaIndex_);
            case Variable::LayerStorage:
                return problemeHebdo.NumeroDeVariableDeTrancheDeStock.at(areaIndex_)
                  .at(layerIndex_);
            case Variable::PositiveUnsuppliedEnergy:
                return nativeOptimVar.NumeroDeVariableDefaillancePositive.at(areaIndex_);
            case Variable::NegativeUnsuppliedEnergy:
                return nativeOptimVar.NumeroDeVariableDefaillanceNegative.at(areaIndex_);
            default:
                return -1;
            }
        }
        catch (const std::out_of_range&)
        {
            Antares::logs.error() << "Wrong index for variable XXX";
            throw;
        }
    }

public:
    ConstraintBuilder& include(Variable var, double coeff, int timeShift = 0, bool wrap = false)
    {
        std::vector<double>& Pi = problemeAResoudre.Pi;
        std::vector<int>& Colonne = problemeAResoudre.Colonne;
        int varIndex = getVariableIndex(var, timeShift, wrap);
        if (varIndex >= 0)
        {
            Pi[nombreDeTermes_] = coeff;
            Colonne[nombreDeTermes_] = varIndex;
            nombreDeTermes_++;
        }
        return *this;
    }
    ConstraintBuilder& equal(double rhs)
    {
        operator_ = '=';
        rhs_ = rhs;
        return *this;
    }
    ConstraintBuilder& less(double rhs)
    {
        operator_ = '<';
        rhs_ = rhs;
        return *this;
    }
    ConstraintBuilder& greater(double rhs)
    {
        operator_ = '>';
        rhs_ = rhs;
        return *this;
    }

    void build();

private:
    PROBLEME_HEBDO& problemeHebdo;
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre;
    const std::vector<CORRESPONDANCES_DES_VARIABLES>& varNative;

    unsigned int hourInWeek_ = 0;

    unsigned int thermalClusterIndex_ = 0;
    unsigned int shortTermStorageIndex_ = 0;
    unsigned int areaIndex_ = 0;
    unsigned int linkIndex_ = 0;
    unsigned int layerIndex_ = 0;

    char operator_;
    double rhs_ = 0;
    int nombreDeTermes_ = 0;
};

class Constraint
{
public:
    Constraint(PROBLEME_HEBDO* problemeHebdo) :
     problemeHebdo(problemeHebdo),
     builder(*problemeHebdo, problemeHebdo->CorrespondanceVarNativesVarOptim)
    {
    }

protected:
    PROBLEME_HEBDO* problemeHebdo; // TODO remove
    ConstraintBuilder builder;
};
