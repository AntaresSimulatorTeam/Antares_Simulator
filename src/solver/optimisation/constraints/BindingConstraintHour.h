#pragma once

#include "ConstraintBuilder.h"
struct BindingConstraintHourData
{
    const std::vector<CONTRAINTES_COUPLANTES>& MatriceDesContraintesCouplantes;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
};
/*!
 * represent 'Hourly Binding Constraint' type
 */
class BindingConstraintHour : private ConstraintFactory
{
    public:
        BindingConstraintHour(ConstraintBuilder& builder,
                              BindingConstraintHourData& data) :
         ConstraintFactory(builder), data(data)
        {
        }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param cntCouplante : the binding constraint number
     */
        void add(int pdt, int cntCouplante);

    private:
        BindingConstraintHourData& data;
};
