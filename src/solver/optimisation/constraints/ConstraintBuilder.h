#pragma once

#include <antares/logs/logs.h>
#include "../opt_structure_probleme_a_resoudre.h"
#include "../opt_rename_problem.h"
#include "../opt_fonctions.h"
#include "../../simulation/sim_structure_probleme_economique.h"

#include <utility>

namespace NewVariable
{

/*!
Factory class that hold variables indices
*/
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

    int NumberOfDispatchableUnits(unsigned int index) const
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
} // namespace NewVariable

// TODO God struct should be decomposed
class ConstraintBuilderData
{
public:
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
    const int32_t& NombreDePasDeTempsPourUneOptimisation;
    const std::vector<int>& NumeroDeVariableStockFinal;
    const std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock;
    std::vector<std::string>& NomDesContraintes;
    const bool& NamedProblems;
    const std::vector<const char*>& NomsDesPays;
    const uint32_t& weekInTheYear;
    const uint32_t& NombreDePasDeTemps;
    uint32_t* NbTermesContraintesPourLesCoutsDeDemarrage = nullptr;
};

/*! \verbatim
this class build up the business object 'Constraint',
Math:
|coeff11 coeff12 .. coeff1n||var1| |sign_1|   |rhs1|       |constraint1||sign_1||rhs1|
|..      ..             ...||....| |......|   |....| <===> |...........||......||....|
|coeffn1 coeffn2 .. coeffnn||varn| |sign_n|   |rhsn|       |constraintn||sign_n||rhsn|

it propose a set of methods  to attach 'Variables' to the Constraint
ex: calling NTCDirect() implies adding Direct NTC Variable to the current Constraint
finally the build() method gather all variables and put them into the matrix
\endverbatim
*/
class ConstraintBuilder
{
public:
    ConstraintBuilder() = delete;
    ConstraintBuilder(std::shared_ptr<ConstraintBuilder>&& other) : data(std::move(other->data))
    {
    }
    explicit ConstraintBuilder(std::shared_ptr<ConstraintBuilderData> data) :
     data(std::move(data))
    {
    }

    ConstraintBuilder& updateHourWithinWeek(unsigned hour)
    {
        hourInWeek_ = hour;
        return *this;
    }

    /** @name variables_method
     *  @brief  Documentation for non obvious methods
     *  @param index: local index of the variable
     *  @param offset: offset from the current time step
     *  @param delta: number of time steps for the variable
     *  @return reference of *this
     */
    //@{
    ConstraintBuilder& DispatchableProduction(unsigned int index,
                                              double coeff,
                                              int offset = 0,
                                              int delta = 0);

    ConstraintBuilder& NumberOfDispatchableUnits(unsigned int index,
                                                 double coeff,
                                                 int offset = 0,
                                                 int delta = 0);

    ConstraintBuilder& NumberStoppingDispatchableUnits(unsigned int index,
                                                       double coeff,
                                                       int offset = 0,
                                                       int delta = 0);

    ConstraintBuilder& NumberStartingDispatchableUnits(unsigned int index,
                                                       double coeff,
                                                       int offset = 0,
                                                       int delta = 0);

    ConstraintBuilder& NumberBreakingDownDispatchableUnits(unsigned int index,
                                                           double coeff,
                                                           int offset = 0,

                                                           int delta = 0);

    ConstraintBuilder& NTCDirect(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& IntercoDirectCost(unsigned int index,
                                         double coeff,
                                         int offset = 0,
                                         int delta = 0);

    ConstraintBuilder& IntercoIndirectCost(unsigned int index,
                                           double coeff,
                                           int offset = 0,
                                           int delta = 0);

    ConstraintBuilder& ShortTermStorageInjection(unsigned int index,
                                                 double coeff,
                                                 int offset = 0,
                                                 int delta = 0);

    ConstraintBuilder& ShortTermStorageWithdrawal(unsigned int index,
                                                  double coeff,
                                                  int offset = 0,
                                                  int delta = 0);

    ConstraintBuilder& ShortTermStorageLevel(unsigned int index,
                                             double coeff,
                                             int offset = 0,
                                             int delta = 0);

    ConstraintBuilder& HydProd(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& HydProdDown(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& HydProdUp(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& Pumping(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& HydroLevel(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& Overflow(unsigned int index, double coeff, int offset = 0, int delta = 0);

    ConstraintBuilder& FinalStorage(unsigned int index,
                                    double coeff,
                                    int offset = 0,
                                    int delta = 0);

    ConstraintBuilder& PositiveUnsuppliedEnergy(unsigned int index,
                                                double coeff,
                                                int offset = 0,
                                                int delta = 0);
    ConstraintBuilder& NegativeUnsuppliedEnergy(unsigned int index,
                                                double coeff,
                                                int offset = 0,
                                                int delta = 0);

    ConstraintBuilder& LayerStorage(unsigned area,
                                    unsigned layer,
                                    double coeff,
                                    int offset = 0,
                                    int delta = 0);
    //@}

    class ConstraintBuilderInvalidOperator : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /*!
       @brief set the operator of the constraint (sign)
       @param op: the operator of the constraint
       @return reference of *this
    */
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

    /*!
     @brief set the sign of the constraint to '=',
     building a constraint equal to rhs
     @return reference of *this
    */
    ConstraintBuilder& equalTo()
    {
        operator_ = '=';
        return *this;
    }

    /*!
    @brief set the sign of the constraint to '<',
    building a constraint less than rhs
    @return reference of *this
    */
    ConstraintBuilder& lessThan()
    {
        operator_ = '<';
        return *this;
    }

    /*!
     @brief set the sign of the constraint to '>',
      building a constraint greather than rhs
     @return reference of *this
    */
    ConstraintBuilder& greaterThan()
    {
        operator_ = '>';
        return *this;
    }

    /*!
      @brief add the constraint in the matrix
      @return
     */
    void build();

    int NumberOfVariables() const
    {
        return nombreDeTermes_;
    }

    std::shared_ptr<ConstraintBuilderData> data;

private:
    void OPT_ChargerLaContrainteDansLaMatriceDesContraintes();

    void OPT_AugmenterLaTailleDeLaMatriceDesContraintes();

    unsigned int hourInWeek_ = 0;

    char operator_ = '=';
    int nombreDeTermes_ = 0;

    int GetShiftedTimeStep(int offset, int delta) const;
    void AddVariable(int index, double coeff);

    /*!
     * @brief
     * @param offset: offset from the current time step
     * @param delta: number of time steps for the variable
     * @return VariableManager object
     */
    NewVariable::VariableManager GetVariableManager(int offset = 0, int delta = 0) const;
};

/*! factory class to build a Constraint */
class ConstraintFactory
{
public:
    ConstraintFactory() = delete;
    explicit ConstraintFactory(std::shared_ptr<ConstraintBuilder> builder) : builder(builder)
    {
    }
    std::shared_ptr<ConstraintBuilder> builder;
};

// Helper functions
inline void new_exportPaliers(const PALIERS_THERMIQUES& PaliersThermiquesDuPays,
                              std::shared_ptr<ConstraintBuilder> newConstraintBuilder)
{
    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        newConstraintBuilder->DispatchableProduction(palier, -1.0);
    }
}

class BindingConstraintData
{
public:
    const char& TypeDeContrainteCouplante;
    const int& NombreDInterconnexionsDansLaContrainteCouplante;
    const std::vector<int>& NumeroDeLInterconnexion;
    const std::vector<double>& PoidsDeLInterconnexion;
    const std::vector<int>& OffsetTemporelSurLInterco;
    const int& NombreDePaliersDispatchDansLaContrainteCouplante;
    const std::vector<int>& PaysDuPalierDispatch;
    const std::vector<int>& NumeroDuPalierDispatch;
    const std::vector<double>& PoidsDuPalierDispatch;
    const std::vector<int>& OffsetTemporelSurLePalierDispatch;
    const char& SensDeLaContrainteCouplante;
    const char* const& NomDeLaContrainteCouplante;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
};

struct StartUpCostsData
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays;
    int cluster;
    int clusterIndex;
    int pdt;
    bool Simulation;
};