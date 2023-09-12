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

struct ConstraintBuilderData
{
    std::vector<double>& Pi;
    std::vector<int>& Colonne;
    int& nombreDeContraintes;
    int& nombreDeTermesDansLaMatriceDeContrainte;
    std::vector<int>& IndicesDebutDeLigne;
    std::vector<double>& CoefficientsDeLaMatriceDesContraintes;
    std::vector<int>& IndicesColonnes;
    int& NombreDeTermesAllouesDansLaMatriceDesContraintes; // TODO Check if ref is needed
    std::vector<int>& NombreDeTermesDesLignes;
    std::string& Sens;
    int& IncrementDAllocationMatriceDesContraintes;
    const std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim;
    const int32_t NombreDePasDeTempsPourUneOptimisation;
    const std::vector<int>& NumeroDeVariableStockFinal;
    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
    std::vector<std::string>& NomDesContraintes;
    const bool NamedProblems;
    const std::vector<const char*> NomsDesPays;
    const uint32_t weekInTheYear;
    const uint32_t NombreDePasDeTemps;
};
class ConstraintBuilder
{
public:
    ConstraintBuilder(ConstraintBuilderData& data) : data(data)
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

public:
    ConstraintBuilderData& data;

private:
    void OPT_ChargerLaContrainteDansLaMatriceDesContraintes();

    void OPT_AugmenterLaTailleDeLaMatriceDesContraintes();

    unsigned int hourInWeek_ = 0;

    char operator_;
    // double rhs_ = 0;
    int nombreDeTermes_ = 0;
    // ConstraintNamer ConstraintNameManager;
};

class Constraint
{
public:
    explicit Constraint(ConstraintBuilder& builder) : builder(builder)
    {
    }
    ConstraintBuilder& builder;
};

// #TODO move this function to a suitable place
// Helper functions
inline void exportPaliers(const PALIERS_THERMIQUES& PaliersThermiquesDuPays,
                          ConstraintBuilder& constraintBuilder)
{
    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        constraintBuilder.include(Variable::DispatchableProduction(palier), -1.0);
    }
}

struct BindingConstraintData
{
    const char TypeDeContrainteCouplante;
    const int NombreDInterconnexionsDansLaContrainteCouplante;
    const std::vector<int>& NumeroDeLInterconnexion;
    const std::vector<double>& PoidsDeLInterconnexion;
    const std::vector<int>& OffsetTemporelSurLInterco;
    const int NombreDePaliersDispatchDansLaContrainteCouplante;
    const std::vector<int>& PaysDuPalierDispatch;
    const std::vector<int>& NumeroDuPalierDispatch;
    const std::vector<double>& PoidsDuPalierDispatch;
    const std::vector<int>& OffsetTemporelSurLePalierDispatch;
    const char SensDeLaContrainteCouplante;
    const char* NomDeLaContrainteCouplante;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
};

inline ConstraintBuilder GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
  const PROBLEME_HEBDO* problemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    ConstraintBuilderData data{ProblemeAResoudre->Pi,
                               ProblemeAResoudre->Colonne,
                               ProblemeAResoudre->NombreDeContraintes,
                               ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes,
                               ProblemeAResoudre->IndicesDebutDeLigne,
                               ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes,
                               ProblemeAResoudre->IndicesColonnes,
                               ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes,
                               ProblemeAResoudre->NombreDeTermesDesLignes,
                               ProblemeAResoudre->Sens,
                               ProblemeAResoudre->IncrementDAllocationMatriceDesContraintes,
                               problemeHebdo->CorrespondanceVarNativesVarOptim,
                               problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
                               problemeHebdo->NumeroDeVariableStockFinal,
                               problemeHebdo->NumeroDeVariableDeTrancheDeStock,
                               ProblemeAResoudre->NomDesContraintes,
                               problemeHebdo->NamedProblems,
                               problemeHebdo->NomsDesPays,
                               problemeHebdo->weekInTheYear,
                               problemeHebdo->NombreDePasDeTemps};

    return ConstraintBuilder(data);
}

inline ConstraintBuilder GetConstraintBuilderFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();
    return GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo,
                                                                   ProblemeAResoudre);
}
