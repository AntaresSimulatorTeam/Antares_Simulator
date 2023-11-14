#pragma once
#include "ConstraintBuilder.h"

struct BindingConstraintWeekData
{
    const std::vector<CONTRAINTES_COUPLANTES>& MatriceDesContraintesCouplantes;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};
/*!
 * represent 'Hourly Binding Constraint' type
 */

class BindingConstraintWeek : private ConstraintFactory
{
public:
    BindingConstraintWeek(ConstraintBuilder& builder,
                          BindingConstraintWeekData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante);

private:
    BindingConstraintWeekData& data;
};
