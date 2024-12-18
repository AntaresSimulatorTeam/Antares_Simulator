/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <memory>
#include <stdexcept>

class CumulationConstraint
{
public:
    virtual void build(ConstraintBuilder& builder,
                       unsigned int index,
                       const ::ShortTermStorage::PROPERTIES& input) const
      = 0;
    virtual std::string name() const = 0;
    virtual ~CumulationConstraint() = default;
};

class WithdrawalCumulationConstraint: public CumulationConstraint
{
public:
    void build(ConstraintBuilder& builder,
               unsigned int index,
               const ::ShortTermStorage::PROPERTIES&) const override
    {
        builder.ShortTermStorageWithdrawal(index, 1.0);
    }

    std::string name() const override
    {
        return "WithdrawalSum";
    }

    ~WithdrawalCumulationConstraint() override = default;
};

class InjectionCumulationConstraint: public CumulationConstraint
{
public:
    void build(ConstraintBuilder& builder,
               unsigned int index,
               const ::ShortTermStorage::PROPERTIES&) const override
    {
        builder.ShortTermStorageInjection(index, 1.0);
    }

    std::string name() const override
    {
        return "InjectionSum";
    }

    ~InjectionCumulationConstraint() override = default;
};

class NettingCumulationConstraint: public CumulationConstraint
{
public:
    void build(ConstraintBuilder& builder,
               unsigned int index,
               const ::ShortTermStorage::PROPERTIES& input) const override
    {
        builder.ShortTermStorageInjection(index, input.injectionEfficiency)
          .ShortTermStorageWithdrawal(index, -input.withdrawalEfficiency);
    }

    std::string name() const override
    {
        return "NettingSum";
    }

    ~NettingCumulationConstraint() override = default;
};

std::unique_ptr<CumulationConstraint> cumulationConstraintFromVariable(const std::string& variable)
{
    if (variable == "withdrawal")
    {
        return std::make_unique<WithdrawalCumulationConstraint>();
    }
    else if (variable == "injection")
    {
        return std::make_unique<InjectionCumulationConstraint>();
    }
    else if (variable == "netting")
    {
        return std::make_unique<NettingCumulationConstraint>();
    }
    throw std::invalid_argument("Invalid cumulation constraint type");
}

char ConvertSense(const std::string& sense)
{
    if (sense == "greater")
    {
        return '>';
    }
    else if (sense == "less")
    {
        return '<';
    }
    else
    {
        return '=';
    }
}

void ShortTermStorageCumulation::add(int pays)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    for (const auto& storage: data.ShortTermStorage[pays])
    {
        for (const auto& constraint: storage.additional_constraints)
        {
            // sum (var[h]) sign rhs, h in list provided by user where:
            // var = injection for InjectionCumulationConstraint
            // var = withdrawal for WithdrawalCumulationConstraint
            // var = injectionEfficiency * injection - withdrawalEfficiency * withdrawal for Netting
            auto constraintHelper = cumulationConstraintFromVariable(constraint.variable);
            namer.ShortTermStorageCumulation(constraintHelper->name(),
                                             builder.data.nombreDeContraintes,
                                             storage.name,
                                             constraint.name);
            const auto index = storage.clusterGlobalIndex;
            data.CorrespondanceCntNativesCntOptimHebdomadaires
              .ShortTermStorageCumulation[constraint.globalIndex]
              = builder.data.nombreDeContraintes;

            for (const auto& hour: constraint.hours)
            {
                builder.updateHourWithinWeek(hour - 1);
                constraintHelper->build(builder, index, storage);
            }
            builder.SetOperator(ConvertSense(constraint.operatorType)).build();
        }
    }
}
