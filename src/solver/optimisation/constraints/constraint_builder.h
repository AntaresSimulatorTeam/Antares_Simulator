#pragma once

#include <antares/logs/logs.h>
#include "../opt_structure_probleme_a_resoudre.h"
#include "../opt_rename_problem.h"
#include "../opt_fonctions.h"

// TODO remove relative include
#include "../../simulation/sim_structure_probleme_economique.h"

#include <utility>

namespace Variable
{

class VariableManager
{
public:
    VariableManager(const CORRESPONDANCES_DES_VARIABLES& nativeOptimVar,
                    const std::vector<int>& NumeroDeVariableStockFinal,
                    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock) :
     nativeOptimVar(nativeOptimVar),
     NumeroDeVariableStockFinal(NumeroDeVariableStockFinal),
     NumeroDeVariableDeTrancheDeStock(NumeroDeVariableDeTrancheDeStock)
    {
    }

    int DispatchableProduction(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuPalierThermique[index];
    }

    int NODU(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index];
    }

    int NumberStoppingDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index];
    }

    int NumberStartingDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index];
    }

    int NumberBreakingDownDispatchableUnits(unsigned int index) const
    {
        return nativeOptimVar
          .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index];
    }

    int NTCDirect(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDeLInterconnexion[index];
    }

    int IntercoDirectCost(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index];
    }

    int IntercoIndirectCost(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index];
    }

    int ShortTermStorageInjection(unsigned int index) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.InjectionVariable[index];
    }

    int ShortTermStorageWithdrawal(unsigned int index) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.WithdrawalVariable[index];
    }

    int ShortTermStorageLevel(unsigned int index) const
    {
        return nativeOptimVar.SIM_ShortTermStorage.LevelVariable[index];
    }

    int HydProd(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDeLaProdHyd[index];
    }

    int HydProdDown(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesVariationHydALaBaisse[index];
    }

    int HydProdUp(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesVariationHydALaHausse[index];
    }

    int Pumping(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDePompage[index];
    }

    int HydroLevel(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDeNiveau[index];
    }

    int Overflow(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariablesDeDebordement[index];
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
        return nativeOptimVar.NumeroDeVariableDefaillancePositive[index];
    }

    int NegativeUnsuppliedEnergy(unsigned int index) const
    {
        return nativeOptimVar.NumeroDeVariableDefaillanceNegative[index];
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

public:
    ConstraintBuilder& DispatchableProduction(unsigned int index,
                                              double coeff,
                                              int shift = 0,
                                              bool wrap = false,
                                              int delta = 0)
    ;

    ConstraintBuilder& NODU(unsigned int index,
                            double coeff,
                            int shift = 0,
                            bool wrap = false,
                            int delta = 0);

    ConstraintBuilder& NumberStoppingDispatchableUnits(unsigned int index,
                                                       double coeff,
                                                       int shift = 0,
                                                       bool wrap = false,
                                                       int delta = 0);

    ConstraintBuilder& NumberStartingDispatchableUnits(unsigned int index,
                                                       double coeff,
                                                       int shift = 0,
                                                       bool wrap = false,
                                                       int delta = 0);

    ConstraintBuilder& NumberBreakingDownDispatchableUnits(unsigned int index,
                                                           double coeff,
                                                           int shift = 0,
                                                           bool wrap = false,
                                                           int delta = 0);

    ConstraintBuilder& NTCDirect(unsigned int index,
                                 double coeff,
                                 int shift = 0,
                                 bool wrap = false,
                                 int delta = 0);

    ConstraintBuilder& IntercoDirectCost(unsigned int index,
                                         double coeff,
                                         int shift = 0,
                                         bool wrap = false,
                                         int delta = 0);

    ConstraintBuilder& IntercoIndirectCost(unsigned int index,
                                         double coeff,
                                         int shift = 0,
                                         bool wrap = false,
                                         int delta = 0);

    ConstraintBuilder& ShortTermStorageInjection(unsigned int index,
                                         double coeff,
                                         int shift = 0,
                                         bool wrap = false,
                                         int delta = 0);

    ConstraintBuilder& ShortTermStorageWithdrawal(unsigned int index,
                                                  double coeff,
                                                  int shift = 0,
                                                  bool wrap = false,
                                                  int delta = 0);

    ConstraintBuilder& ShortTermStorageLevel(unsigned int index,
                                                  double coeff,
                                                  int shift = 0,
                                                  bool wrap = false,
                                                  int delta = 0);

    ConstraintBuilder& HydProd(unsigned int index,
                               double coeff,
                               int shift = 0,
                               bool wrap = false,
                               int delta = 0);

    ConstraintBuilder& HydProdDown(unsigned int index,
                                   double coeff,
                                   int shift = 0,
                                   bool wrap = false,
                                   int delta = 0);

    ConstraintBuilder& HydProdUp(unsigned int index,
                                 double coeff,
                                 int shift = 0,
                                 bool wrap = false,
                                 int delta = 0);

    ConstraintBuilder& Pumping(unsigned int index,
                               double coeff,
                               int shift = 0,
                               bool wrap = false,
                               int delta = 0);

    ConstraintBuilder& HydroLevel(unsigned int index,
                                  double coeff,
                                  int shift = 0,
                                  bool wrap = false,
                                  int delta = 0);

    ConstraintBuilder& Overflow(unsigned int index,
                                double coeff,
                                int shift = 0,
                                bool wrap = false,
                                int delta = 0);

    ConstraintBuilder& FinalStorage(unsigned int index,
                                    double coeff,
                                    int shift = 0,
                                    bool wrap = false,
                                    int delta = 0);

    ConstraintBuilder& PositiveUnsuppliedEnergy(unsigned int index,
                                                double coeff,
                                                int shift = 0,
                                                bool wrap = false,
                                                int delta = 0);

    ConstraintBuilder& NegativeUnsuppliedEnergy(unsigned int index,
                                                double coeff,
                                                int shift = 0,
                                                bool wrap = false,
                                                int delta = 0);
    ConstraintBuilder& LayerStorage(unsigned area,
                                    unsigned layer,
                                    double coeff,
                                    int shift = 0,
                                    bool wrap = false,
                                    int delta = 0);

    class ConstraintBuilderInvalidOperator : public std::runtime_error
    {
    public:
        ConstraintBuilderInvalidOperator(const std::string& error_message) :
         std::runtime_error(error_message)
        {
        }
    };

    ConstraintBuilder& SetOperator(char op)
    {
        if (op == '<' || op == '=' || op == '>')
        {
            operator_ = op;
        }
        else
            throw ConstraintBuilderInvalidOperator(std::string("Invalid operator: ") + op);

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

    int GetShiftedTimeStep(int shift, bool wrap, int delta) const;
    void AddVariable(int index, double coeff);
    Variable::VariableManager GetVariableManager( int shift = 0, bool wrap = false, int delta = 0);
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
        constraintBuilder.DispatchableProduction(palier, -1.0);
    }
}
