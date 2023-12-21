#pragma once
#include "ConstraintBuilder.h"

struct BindingConstraintDayData
{
    const std::vector<CONTRAINTES_COUPLANTES>& MatriceDesContraintesCouplantes;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES>&
      CorrespondanceCntNativesCntOptimJournalieres;
    const int32_t& NombreDePasDeTempsDUneJournee;
    std::vector<int32_t>& NumeroDeJourDuPasDeTemps;
};

/*!
 * represent 'Daily Binding Constraint' type
 */

class BindingConstraintDay : private ConstraintFactory
{
public:
    BindingConstraintDay(ConstraintBuilder& builder,
                         BindingConstraintDayData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante);

private:
    BindingConstraintDayData& data;
};
