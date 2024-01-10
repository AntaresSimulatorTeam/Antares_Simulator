/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
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
