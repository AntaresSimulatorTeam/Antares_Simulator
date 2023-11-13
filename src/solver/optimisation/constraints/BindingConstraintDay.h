#pragma once
#include "ConstraintBuilder.h"

struct BindingConstraintDayData : public BindingConstraintData
{
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
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante, BindingConstraintDayData& data);
};
