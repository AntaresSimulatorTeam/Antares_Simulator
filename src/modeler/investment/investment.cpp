/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/modeler/investment/investment.h"

#include <antares/logs/logs.h>

namespace Antares::Modeler::Investment
{

bool Investment::markVariablesForInvestment(Modeler::Data& modelerData)
{
    const auto& optimConfig = modelerData.investmentOptimConfig.value();

    bool ret = true;
    for (const auto& model: optimConfig.models())
    {
        bool modelFound = false;
        for (auto& [_, component]: modelerData.system->Components())
        {
            if (component.getModel()->Id() == model.id())
            {
                modelFound = true;
                for (const auto& modelDecompositionVar: model.modelDecomposition().variables())
                {
                    /*component.getModel()*/
                    /*  ->Variables()*/
                    /*  .at(modelDecompositionVar.id())*/
                    /*  .setLocation(modelDecompositionVar.location());*/
                }
            }
        }

        if (!modelFound)
        {
            logs.warning() << "No component found for investment model with ID '" + model.id()
                                + "'.";
            ret = false;
        }
    }

    return ret;
}

} // namespace Antares::Modeler::Investment
