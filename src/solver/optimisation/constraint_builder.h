#pragma once

#include <antares/logs/logs.h>
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_rename_problem.h"
#include "opt_fonctions.h"

// TODO remove relative include
#include "../simulation/sim_structure_probleme_economique.h"

#include <variant>
#include <utility>

namespace Variable
{
struct SingleIndex
{
    explicit SingleIndex(unsigned index) : index(index)
    {
    }
    unsigned index;
};

struct DispatchableProduction : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct NODU : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct NumberStoppingDispatchableUnits : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct NumberStartingDispatchableUnits : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct NumberBreakingDownDispatchableUnits : public SingleIndex
{
    using SingleIndex::SingleIndex;
};

struct NTCDirect : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct IntercoDirectCost : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct IntercoIndirectCost : public SingleIndex
{
    using SingleIndex::SingleIndex;
};

struct ShortTermStorageInjection : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct ShortTermStorageWithdrawal : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct ShortTermStorageLevel : public SingleIndex
{
    using SingleIndex::SingleIndex;
};

struct HydProd : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct HydProdDown : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct HydProdUp : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct Pumping : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct HydroLevel : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct Overflow : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct FinalStorage : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct PositiveUnsuppliedEnergy : public SingleIndex
{
    using SingleIndex::SingleIndex;
};
struct NegativeUnsuppliedEnergy : public SingleIndex
{
    using SingleIndex::SingleIndex;
};

struct LayerStorage
{
    LayerStorage(unsigned area, unsigned layer) : area(area), layer(layer)
    {
    }
    unsigned area, layer;
};

using Variant = std::variant<DispatchableProduction,
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
                             NegativeUnsuppliedEnergy>;

class Visitor
{
public:
    Visitor(const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar,
            const std::vector<int>& NumeroDeVariableStockFinal,
            const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock) :
     nativeOptimVar(nativeOptimVar),
     NumeroDeVariableStockFinal(NumeroDeVariableStockFinal),
     NumeroDeVariableDeTrancheDeStock(NumeroDeVariableDeTrancheDeStock)
    {
    }

    int operator()(const DispatchableProduction& v) const
    {
        return nativeOptimVar.NumeroDeVariableDuPalierThermique[v.index];
    }
    int operator()(const NODU& v) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[v.index];
    }
    int operator()(const NumberStoppingDispatchableUnits& v) const
    {
        return nativeOptimVar
          .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[v.index];
    }
    int operator()(const NumberStartingDispatchableUnits& v) const
    {
        return nativeOptimVar
          .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[v.index];
    }
    int operator()(const NumberBreakingDownDispatchableUnits& v) const
    {
        return nativeOptimVar
          .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[v.index];
    }
    int operator()(const NTCDirect& v) const
    {
        return nativeOptimVar.NumeroDeVariableDeLInterconnexion[v.index];
    }
    int operator()(const IntercoDirectCost& v) const
    {
        return nativeOptimVar.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[v.index];
    }
    int operator()(const IntercoIndirectCost& v) const
    {
        return nativeOptimVar.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[v.index];
    }
    int operator()(const ShortTermStorageInjection& v) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.InjectionVariable[v.index];
    }
    int operator()(const ShortTermStorageWithdrawal& v) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.WithdrawalVariable[v.index];
    }
    int operator()(const ShortTermStorageLevel& v) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.LevelVariable[v.index];
    }
    int operator()(const HydProd& v) const
    {
        return nativeOptimVar.NumeroDeVariablesDeLaProdHyd[v.index];
    }
    int operator()(const HydProdDown& v) const
    {
        return nativeOptimVar.NumeroDeVariablesVariationHydALaBaisse[v.index];
    }
    int operator()(const HydProdUp& v) const
    {
        return nativeOptimVar.NumeroDeVariablesVariationHydALaHausse[v.index];
    }
    int operator()(const Pumping& v) const
    {
        return nativeOptimVar.NumeroDeVariablesDePompage[v.index];
    }
    int operator()(const HydroLevel& v) const
    {
        return nativeOptimVar.NumeroDeVariablesDeNiveau[v.index];
    }
    int operator()(const Overflow& v) const
    {
        return nativeOptimVar.NumeroDeVariablesDeDebordement[v.index];
    }
    int operator()(const FinalStorage& v) const
    {
        return NumeroDeVariableStockFinal[v.index];
    }
    int operator()(const LayerStorage& v) const
    {
        return NumeroDeVariableDeTrancheDeStock[v.area][v.layer];
    }
    int operator()(const PositiveUnsuppliedEnergy& v) const
    {
        return nativeOptimVar.NumeroDeVariableDefaillancePositive[v.index];
    }
    int operator()(const NegativeUnsuppliedEnergy& v) const
    {
        return nativeOptimVar.NumeroDeVariableDefaillanceNegative[v.index];
    }

private:
    const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar;
    const std::vector<int>& NumeroDeVariableStockFinal;
    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
};
} // namespace Variable

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

    ConstraintBuilder& updateHourWithinWeek(unsigned hour)
    {
        hourInWeek_ = hour;
        return *this;
    }

private:
    int getVariableIndex(const Variable::Variant& variable, int shift, bool wrap, int delta) const;

public:
    ConstraintBuilder& include(Variable::Variant var,
                               double coeff,
                               int shift = 0,
                               bool wrap = false,
                               int delta = 0);

    ConstraintBuilder& operatorRHS(char op)
    {
        if (op == '<' || op == '=' || op == '>')
        {
            operator_ = op;
        }
        else
            throw std::runtime_error("Invalid operator");

        return *this;
    }

    ConstraintBuilder& equalTo()
    {
        operator_ = '=';
        return *this;
    }
    ConstraintBuilder& lessThan()
    {
        operator_ = '<';
        return *this;
    }
    ConstraintBuilder& greaterThan()
    {
        operator_ = '>';
        return *this;
    }

    void build();
    int NumberOfVariables() const
    {
        return nombreDeTermes_;
    }

private:
    PROBLEME_HEBDO& problemeHebdo;
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre;
    const std::vector<CORRESPONDANCES_DES_VARIABLES>& varNative;

    unsigned int hourInWeek_ = 0;

    char operator_;
    double rhs_ = 0;
    int nombreDeTermes_ = 0;
    // ConstraintNamer ConstraintNameManager;
};

class Constraint
{
public:
    explicit Constraint(PROBLEME_HEBDO* problemeHebdo) :
     problemeHebdo(problemeHebdo),
     builder(*problemeHebdo, problemeHebdo->CorrespondanceVarNativesVarOptim)
    {
    }

    PROBLEME_HEBDO* problemeHebdo; // TODO remove
    ConstraintBuilder builder;
};

// #TODO move this function to a suitable place
// Helper functions
inline void exportPaliers(const PROBLEME_HEBDO& problemeHebdo,
                          ConstraintBuilder& constraintBuilder,
                          int pays)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo.PaliersThermiquesDuPays[pays];

    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        constraintBuilder.include(Variable::DispatchableProduction(palier), -1.0);
    }
}
