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

#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"

#include <functional>
#include <memory>
#include <stdexcept>

struct CumulationConstraint
{
    const std::string name;
    const std::function<void(unsigned int)> build;
};

CumulationConstraint makeCumulationConstraint(const std::string& variable,
                                              ConstraintBuilder& builder,
                                              const ShortTermStorage::PROPERTIES& props)
{
    if (variable == "withdrawal")
    {
        return {"WithdrawalSum",
                [&builder](unsigned int idx) { builder.ShortTermStorageWithdrawal(idx, 1.0); }};
    }
    else if (variable == "injection")
    {
        return {"InjectionSum",
                [&builder](unsigned int idx) { builder.ShortTermStorageInjection(idx, 1.0); }};
    }
    else if (variable == "netting")
    {
        return {"NettingSum",
                [&builder, &props](unsigned int idx)
                {
                    builder.ShortTermStorageInjection(idx, props.injectionEfficiency)
                      .ShortTermStorageWithdrawal(idx, -props.withdrawalEfficiency);
                }};
    }

    throw std::invalid_argument("Invalid cumulation constraint type: " + variable);
}

char ConvertSense(const std::string& sense)
{
    if (sense == "greater")
    {
        return '>';
    }
    if (sense == "less")
    {
        return '<';
    }
    return '=';
}

void ShortTermStorageCumulation::add(int pays)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    for (const auto& storage: data.ShortTermStorage[pays])
    {
        for (const auto& additionalConstraints: storage.additionalConstraints)
        {
            const CumulationConstraint cumulationConstraint = makeCumulationConstraint(
              additionalConstraints->variable,
              builder,
              storage);

            for (const auto& [hours, globalIndex, localIndex]: additionalConstraints->constraints)
            {
                namer.ShortTermStorageCumulation(cumulationConstraint.name,
                                                 builder.data.nombreDeContraintes,
                                                 storage.name,
                                                 additionalConstraints->id + "_"
                                                   + std::to_string(localIndex));

                const auto index = storage.clusterGlobalIndex;
                data.CorrespondanceCntNativesCntOptimHebdomadaires
                  .ShortTermStorageCumulation[globalIndex]
                  = builder.data.nombreDeContraintes;

                for (const auto& hour: hours)
                {
                    builder.updateHourWithinWeek(hour - 1);
                    cumulationConstraint.build(index);
                }
                builder.SetOperator(ConvertSense(additionalConstraints->operatorType)).build();
            }
        }
    }
}
